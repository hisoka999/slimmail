#include "emailview.h"
#include "ui_emailview.h"
#include <QDataWidgetMapper>
#include <QDesktopServices>
#include <QDebug>
#include <QSqlRelationalDelegate>

EMailView::EMailView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EMailView),model(nullptr)
{
    ui->setupUi(this);
    mapper = new QDataWidgetMapper(this);
    //mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    ui->webView->settings()->setAttribute(QWebSettings::JavascriptEnabled,false);


}

EMailView::~EMailView()
{
    delete mapper;
    delete ui;
}

void EMailView::setModel(EMailModel *model)
{
    this->model = model;
    mapper->setModel(model);
    mapper->addMapping(ui->toLineEdit,3);
    mapper->addMapping(ui->fromLineEdit,1);
    mapper->addMapping(ui->subjectLineEdit,0);
    mapper->addMapping(ui->dateDateTimeEdit,2);
    //mapper->setItemDelegate(new QSqlRelationalDelegate(this));

    mapper->toFirst();
}

void EMailView::setCurrentModelIndex(const QModelIndex &index)
{
    mapper->setCurrentModelIndex(index);

    //set mail in the mailview
    EMail *mail = model->getEmails().at(index.row());
    qDebug()<<"selected mail: "<<mail->getSubject();
    QString contentType = "text/html;charset=UTF-8";
    if(!mail->getHTMLBody().isEmpty()){
        ui->webView->setContent(mail->getHTMLBody().toUtf8(),contentType,QUrl());
    }else{
        contentType = "text/plain;charset=UTF-8";
        ui->webView->setContent(mail->getPlainTextBody().toUtf8(),contentType,QUrl());
    }

}

void EMailView::on_webView_linkClicked(const QUrl &arg1)
{
    QDesktopServices::openUrl(arg1);
}
