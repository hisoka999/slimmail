#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <models/emailmodel.h>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>
#include <QProgressBar>
#include "models/mailboxmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void loadMailboxes(QSqlDatabase &db);
    void on_tableView_activated(const QModelIndex &index);
    void handleResults();
    void startProgress(int maxProgress);
    void progress(int progress);

    void on_treeView_clicked(const QModelIndex &index);

    void on_actionNew_Mail_triggered();

private:
    void initDatabase();
    Ui::MainWindow *ui;
    EMailModel *model;
    MailboxModel* mailboxModel;
    QList<Mailbox*> mailboxes;
    QProgressBar progressBar;
};

#endif // MAINWINDOW_H
