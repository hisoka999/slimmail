#include "mailsyncthread.h"
#include <vmime/vmime.hpp>
#include <vmime/platforms/posix/posixHandler.hpp>
#include <vmime/contentTypeField.hpp>
#include <vector>
#include <iostream>
#include <certverifier.h>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QTextCodec>

MailSyncThread::MailSyncThread(QList<Mailbox*> mailboxes,QObject *parent)
    :QThread(parent),mailboxes(mailboxes)
{

}

void MailSyncThread::run(){
    qRegisterMetaType<QList<EMail*>>();
    try{
        vmime:: platform ::setHandler <vmime::platforms::posix::posixHandler>();
        vmime::charset ch(vmime::charsets::UTF_8);



        for(Mailbox* mailbox: mailboxes){
            vmime::shared_ptr<vmime::net::session> theSession = vmime::net::session::create();
            theSession->getProperties().setProperty("store.imap.auth.username",mailbox->username().toStdString());
            theSession->getProperties().setProperty("store.imap.auth.password",mailbox->password().toStdString());
            theSession->getProperties().setProperty("store.imap.server.address",mailbox->server().toStdString());
            theSession->getProperties().setProperty("store.imap.server.port",mailbox->port());
            theSession->getProperties().setProperty("store.imap.connection.tls",true);
            theSession->getProperties().setProperty("store.imap.connection.tls.required",true);
            theSession->getProperties().setProperty("store.protocol",mailbox->protocol().toStdString());

            vmime::shared_ptr<vmime::net::store> st = theSession->getStore();
            st->setCertificateVerifier(vmime::make_shared<CertVerifier>());
            st->connect();


            //sync folders
            syncFolders(st,mailbox);

            st->disconnect();
            st->connect();
            emit startProgress(mailbox->directories().count());

            for (auto mailboxFolder : mailbox->directories()){
                vmime::net::folder::path path;
                path/=vmime::net::folder::path::component(mailboxFolder->internalName().toStdString());
                vmime::shared_ptr< vmime::net::folder> folder = st->getFolder(path);
                qDebug()<<"t folder: "<<QString::fromStdString(folder->getFullPath().toString("/",ch));

                vmime::net::folderAttributes attribs =  folder->getAttributes();



                if(folder->isOpen()){
                    folder->close(true);
                    qDebug()<<"is open";
                }
                //if(attribs.getFlags() & vmime::net::folderAttributes::Flags::FLAG_NO_OPEN ||mailboxFolder->internalName() == "Trash")
                //    continue;

                folder->open(vmime::net::folder::MODE_READ_WRITE);
                qDebug()<<"open";
                std::vector<vmime::shared_ptr<vmime::net::message>> allMessages = folder->getMessages(vmime::net::messageSet::byUID(mailboxFolder->maxUID().toInt(), "*"));
                // −1 i s a spec ial value to mean ”the number of the l a s t message in the folder”
                folder->fetchMessages(allMessages, vmime::net::fetchAttributes::FLAGS|vmime::net::fetchAttributes::ENVELOPE);
                QList<EMail*> emails = mailboxFolder->getMails();
                QSqlDatabase db = QSqlDatabase();
                db.transaction();
                qDebug()<<"loaded messages: "<<allMessages.size();
                for (unsigned int i = 0 ; i < allMessages . size () ; ++i )
                {
                    vmime::shared_ptr<vmime::net::message> msg = allMessages[i] ;

                    if (QString::fromStdString(msg->getUID()) == mailboxFolder->maxUID())
                        continue;

                    const int flags = msg->getFlags();
                    vmime::shared_ptr<const vmime::header> hdr = msg->getHeader();
                    QString  subject = QString::fromStdString(hdr->Subject()->getValue<vmime::text>()->getConvertedText(ch));
                    QString from  =QString::fromStdString(hdr->From()->getValue<vmime::mailbox>()->getEmail().toString());

                    vmime::shared_ptr<const vmime::addressList> to = hdr->To()->getValue<vmime::addressList>();

                    QString tmp = QString::fromStdString(hdr->Date()->generate());

                    vmime::messageParser mp(msg->getParsedMessage());
                    std::stringstream ss;
                    std::stringstream sspt;
                    QString textPlainPart ="";
                    for ( int j = 0;j<mp.getTextPartCount();++j){
                         vmime::shared_ptr<const vmime::textPart> tp = mp.getTextPartAt(j);

                        // text/html
                        if(tp->getType().getSubType() == vmime::mediaTypes::TEXT_HTML){
                            vmime::shared_ptr<const vmime::htmlTextPart> htp = vmime::dynamicCast<const vmime::htmlTextPart>(tp);
                            vmime::shared_ptr<const vmime::contentHandler> cth =  htp->getText();
                            vmime::utility::outputStreamAdapter out(ss);
                            cth->extract(out);
                        }else{
                            vmime::shared_ptr<const vmime::plainTextPart> ptp = vmime::dynamicCast<const vmime::plainTextPart>(tp);
                            vmime::shared_ptr<const vmime::contentHandler> cth =  ptp->getText();
                            vmime::utility::outputStreamAdapter out(sspt);
                            cth->extract(out);
                            std::string txt =  sspt.str();
                            QTextCodec *codec = QTextCodec::codecForName(ptp->getCharset().getName().c_str());
                            textPlainPart = codec->toUnicode(txt.c_str(),txt.length());
                        }


                    }

                    tmp = tmp.remove(0,6);
                    tmp.chop(6);
                    QLocale c(QLocale::C);
                    QDateTime date = c.toDateTime(tmp,"ddd, d MMM yyyy HH:mm:ss");

                    EMail *mail = new EMail(0,subject,new EMailAddress(from,QString()),date,flags & vmime::net::message::FLAG_SEEN,mailbox->id(),mailboxFolder->id());

                    for(auto address: to->getAddressList()){

                        if(address->isGroup()){
                            vmime::shared_ptr<const vmime::mailboxGroup> mbg = vmime::dynamic_pointer_cast<const vmime::mailboxGroup>(address);
                        }else{
                            vmime::shared_ptr<const vmime::mailbox> mb = vmime::dynamic_pointer_cast<const vmime::mailbox>(address);
                            QString name = QString::fromStdString(mb->getName().getConvertedText(ch));
                            QString email = QString::fromStdString(mb->getEmail().toString());
                            mail->addTo(new EMailAddress( email,name));
                        }

                    }


                    mail->setHTMLBody(QString::fromStdString(ss.str()));
                    mail->setPlainTextBody(textPlainPart);
                    mail->setUID(QString::fromStdString(msg->getUID()));

                    mail->save(db);
                    emails.append(mail);

                    //if(emails.count() > 10)
                    //    break;

                }
                emit progress(1);
                db.commit();
                qDebug()<<"count = "<<emails.count();
                mailboxFolder->setMails(emails);
                folder->close(false);
            }

            //model->setEmails(emails);



        }
        emit resultsReady();
    }catch(vmime::exceptions::authentication_error e){
        std::cout<<e.response()<<std::endl;
        std::cout<<e.what()<<std::endl;
    }



}


void MailSyncThread::syncFolders(vmime::shared_ptr<vmime::net::store> st,Mailbox* mailbox){
    vmime::shared_ptr< vmime::net::folder> rootFolder = st->getRootFolder();
    std::vector<vmime::shared_ptr< vmime::net::folder>> folders = rootFolder->getFolders();

    QSqlQuery query;
    query.prepare("INSERT INTO mailbox_dirs (label,internal_name,sync,mailbox_id,parent_id) "
                    "VALUES(:label,:internal_name,:sync,:mailbox,:parent_id);");

    for(auto sub : folders){
        QString folder = QString::fromStdString(sub->getName().getBuffer());
        qDebug()<<"folder: "<<folder;
        if(!mailbox->existsDirectory(folder)){
            mailbox->addDirectory(new MailboxDirectories(0,folder,folder,1,mailbox->id(),0));
            query.bindValue(":label",folder);
            query.bindValue(":internal_name",folder);
            query.bindValue(":sync",1);
            query.bindValue(":mailbox",mailbox->id());
            query.bindValue(":parent_id",0);
            query.exec();
        }
        if(sub->isOpen())
            sub->close(false);
    }
    qDebug()<<"cnt : "<<mailbox->directories().count();
    if(rootFolder->isOpen())
        rootFolder->close(false);
}
