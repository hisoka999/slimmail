#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <email.h>
#include <sstream>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QProgressBar>
#include "mailbox.h"
#include "models/mailboxmodel.h"
#include "mailsyncthread.h"
#include <newmailwindow.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new EMailModel();
    ui->tableView->setModel(model);
    ui->tableView->hideColumn(3);
    ui->tableView->hideColumn(4);
    ui->tableView->hideColumn(5);
    ui->tableView->hideColumn(6);
    ui->emailView->setModel(model);

    connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            ui->emailView, SLOT(setCurrentModelIndex(QModelIndex)));



    initDatabase();

    this->statusBar()->addWidget(&progressBar);
    //progressBar.hide();

    MailSyncThread *workerThread = new MailSyncThread(mailboxes,this);
    connect(workerThread ,SIGNAL(resultsReady()), this, SLOT(handleResults()));
    connect(workerThread, &MailSyncThread::finished, workerThread, &QObject::deleteLater);
    connect(workerThread, &MailSyncThread::startProgress,this,&MainWindow::startProgress);
    connect(workerThread, &MailSyncThread::progress,this,&MainWindow::progress);
    workerThread->start();


}

void MainWindow::handleResults(){
    if(mailboxModel != nullptr)
        delete mailboxModel;
    mailboxModel = new MailboxModel(mailboxes);
    ui->treeView->setModel(mailboxModel);
    ui->treeView->expandAll();
    statusBar()->removeWidget(&progressBar);
//    model->setEmails(emails);
    //    ui->tableView->repaint();
}

void MainWindow::startProgress(int maxProgress)
{
    progressBar.setVisible(true);
    progressBar.setTextVisible(true);
    progressBar.setRange(0,maxProgress);
    progressBar.setValue(0);

}

void MainWindow::progress(int progress)
{
    progressBar.setValue(progressBar.value()+progress);
}


void MainWindow::loadMailboxes(QSqlDatabase &db){
    QSqlQuery query(db);
    query.exec("SELECT id,name,username,password,protocol,server,port FROM mailboxes ORDER BY id ASC");
    mailboxes.clear();
    QSqlQuery dirs(db);
    dirs.prepare("SELECT d.id,d.label,d.internal_name,d.sync,d.parent_id ,ifnull(max(CAST( m.uid AS INTEGER)),1) max_uid "
                 "FROM mailbox_dirs d LEFT JOIN  mailbox_mails m ON m.dir_id = d.id "
                 "WHERE d.mailbox_id = :mailbox_id "
                 "GROUP BY d.id,d.label,d.internal_name,d.sync,d.parent_id ");

    while(query.next()){
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString username = query.value(2).toString();
        QString password = query.value(3).toString();
        QString protocol = query.value(4).toString();
        QString server = query.value(5).toString();
        int port = query.value(6).toInt();
        qDebug()<<name;

        dirs.bindValue(":mailbox_id",id);
        if(!dirs.exec()){
            qDebug()<<dirs.lastError();
        }

        Mailbox* mb = new Mailbox(id,username,name,password,server,protocol,port);

        while(dirs.next()){
            int dirId = dirs.value(0).toInt();
            QString label = dirs.value(1).toString();
            QString internalName = dirs.value(2).toString();
            int sync = dirs.value(3).toInt();
            int parentId = dirs.value(4).toInt();
            QString maxUID = dirs.value(5).toString();
            MailboxDirectories *dir = new MailboxDirectories(dirId,label,internalName,sync,id,parentId);
            dir->setMaxUID(maxUID);
            dir->setMails(EMail::loadEmails(dirId));
            mb->addDirectory(dir);
        }



        mailboxes.append(mb);
    }
    mailboxModel = new MailboxModel(mailboxes);
    ui->treeView->setModel(mailboxModel);
    ui->treeView->expandAll();
    //ui->treeView->setBackgroundRole(QPalette::Window);



}

void MainWindow::initDatabase()
{
    QApplication::setApplicationVersion("0.1");
    QApplication::setApplicationName("SlimMail");
    QApplication::setOrganizationName("SlimMail");

    QSettings ini(QSettings::IniFormat, QSettings::UserScope,
    QCoreApplication::organizationName(),
    QCoreApplication::applicationName());
    QString dir = QFileInfo(ini.fileName()).absolutePath();

    QDir path = QDir(dir);
    bool newdb= false;
    QFile dbfile(path.absolutePath()+path.separator()+"data.db");
    if (!path.exists(path.absolutePath()))
    {
        path.mkpath(path.absolutePath());
        dbfile.open(QIODevice::WriteOnly);
       dbfile.flush();
       dbfile.close();
       newdb = true;
    }


    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbfile.fileName());
    if (!db.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
            qApp->tr("Unable to establish a database connection.\n\n"
                     "Click Cancel to exit."), QMessageBox::Cancel);


        this->close();
    }
    loadMailboxes(db);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_tableView_activated(const QModelIndex &index)
{


}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{

    QString folder = ((MailboxTreeItem*)index.internalPointer())->data(2).toString();
    for(auto mailbox: mailboxes){
        if(mailbox->name() == folder)
            break;

        for(auto dir : mailbox->directories()){
            if(dir->label() == folder){
                model->setEmails(dir->getMails());
                ui->tableView->horizontalHeader()->setStretchLastSection(true);
                ui->tableView->horizontalHeader()->setAlternatingRowColors(true);
                ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
                ui->tableView->sortByColumn(2,Qt::DescendingOrder);
                break;
            }

        }
    }

    qDebug()<<"selected Mailbox: "<<folder;
}

void MainWindow::on_actionNew_Mail_triggered()
{
    QModelIndex index = ui->treeView->currentIndex();
    QString folder = ((MailboxTreeItem*)index.internalPointer())->data(2).toString();
    Mailbox* currentMailbox = nullptr;
    for(auto mailbox: mailboxes){
        if(mailbox->name() == folder){
            currentMailbox = mailbox;
            break;
        }
        for(auto dir : mailbox->directories()){
            if(dir->label() == folder){
                currentMailbox = mailbox;
            }
        }

    }
    NewMailWindow* win= new NewMailWindow(currentMailbox,this);
    win->show();
}
