#include "newmailwindow.h"
#include "ui_newmailwindow.h"
#include <QCompleter>
#include <vmime/vmime.hpp>
#include <certverifier.h>
#include <iostream>

NewMailWindow::NewMailWindow(Mailbox* mailbox,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NewMailWindow),_mailbox(mailbox)
{
    ui->setupUi(this);

    fontCombobox = new QFontComboBox(ui->toolBarRichText);
    ui->toolBarRichText->addWidget(fontCombobox);
    connect(fontCombobox,&QFontComboBox::currentFontChanged,this,&NewMailWindow::currentFontChanged);

    adrModel = new QSqlTableModel();

    completer = new DelimitedCompleter(ui->toLineEdit,';',adrModel);
    completer->setCompletionColumn(2);

    ui->fromLineEdit->setText(_mailbox->username());

    ui->toLineEdit->setCompleter(completer);

    adrModel->setTable("emailaddresses");
    adrModel->select();
    on_toLineEdit_textChanged(QString());
}

NewMailWindow::~NewMailWindow()
{
    delete ui;
}

void NewMailWindow::on_actionBold_triggered()
{

}

void NewMailWindow::on_actionBold_triggered(bool checked)
{
    QFont font= ui->textEdit->currentFont();
    font.setBold(checked);
    ui->textEdit->setCurrentFont(font);
}

void NewMailWindow::on_actionUnderlinded_triggered(bool checked)
{
    QFont font = ui->textEdit->currentFont();
    font.setUnderline(checked);
    ui->textEdit->setCurrentFont(font);
}

void NewMailWindow::currentFontChanged(const QFont &font)
{
    ui->textEdit->setCurrentFont(font);
}

void NewMailWindow::on_toLineEdit_textChanged(const QString &arg1)
{
    ui->ccLabel->setVisible(!arg1.isEmpty());
    ui->ccLineEdit->setVisible(!arg1.isEmpty());
}

void NewMailWindow::on_actionSend_triggered()
{
    static vmime::shared_ptr <vmime::net::session> g_session = vmime::net::session::create();
    std::string urlString = "smtp://smtp.silver-boards.com";
    vmime::utility::url url(urlString);
    vmime::shared_ptr <vmime::net::transport> tr;
    url.setUsername(_mailbox->username().toStdString());
    url.setPassword(_mailbox->password().toStdString());
        tr = g_session->getTransport(url);

#if VMIME_HAVE_TLS_SUPPORT

    // Enable TLS support if available
    tr->setProperty("connection.tls", true);

    // Set the time out handler
    //tr->setTimeoutHandlerFactory(vmime::make_shared <timeoutHandlerFactory>());

    // Set the object responsible for verifying certificates, in the
    // case a secured connection is used (TLS/SSL)
    tr->setCertificateVerifier
        (vmime::make_shared <CertVerifier>());

#endif // VMIME_HAVE_TLS_SUPPORT

    // You can also set some properties (see example7 to know the properties
    // available for each service). For example, for SMTP:
    if (!url.getUsername().empty() || !url.getPassword().empty())
        tr->setProperty("options.need-authentication", true);

    //vmime::shared_ptr <std::ostringstream> traceStream = vmime::make_shared <std::ostringstream>();
    //tr->setTracerFactory(vmime::make_shared <myTracerFactory>(traceStream));

    // Information about the mail

    vmime::mailbox from(ui->fromLineEdit->text().toStdString());
    vmime::mailboxList to;

    QString toText =ui->toLineEdit->text();
    QStringList toList = toText.split(";");

    std::cout << "Enter message data, including headers (end with '.' on a single line):" << std::endl;


    // Connect to server
    tr->connect();

    // Send the message
    vmime::string msgData = ui->textEdit->document()->toPlainText().toStdString();
    vmime::utility::inputStreamStringAdapter vis(msgData);
    vmime::messageBuilder mb;
    mb.setSubject(vmime::text(ui->subjectLineEdit->text().toStdString()));
    mb.setExpeditor(from);

    for(QString text : toList){
        //to.appendMailbox(vmime::make_shared <vmime::mailbox>(text.toStdString()));
        mb.getRecipients().appendAddress(vmime::make_shared <vmime::mailbox>(text.toStdString()));
    }


    mb.constructTextPart(vmime::mediaType(vmime::mediaTypes::TEXT,vmime::mediaTypes::TEXT_HTML));

    // Set contents of the tex t parts ; the message i s ava ilable in two formats :
    // HTML and plain tex t . The HTML format also includes an embedded image .
    vmime::shared_ptr<vmime::htmlTextPart> textPart =
    vmime::dynamicCast<vmime::htmlTextPart>(mb.getTextPart ());
    textPart->setCharset(vmime::charsets::UTF_8);
    textPart->setText(vmime::make_shared<vmime::stringContentHandler>(ui->textEdit->document()->toHtml().toStdString()));
    textPart->setPlainText(vmime::make_shared<vmime::stringContentHandler>(ui->textEdit->document()->toPlainText().toStdString()));

    tr->send(mb.construct());

    // Note: you could also write this:
    //     vmime::message msg;
    //     ...
    //     tr->send(&msg);

    // Display connection log
    std::cout << std::endl;
    std::cout << "Connection Trace:" << std::endl;
    std::cout << "=================" << std::endl;
    //std::cout << traceStream->str();

    tr->disconnect();
}
