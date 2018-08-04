#include "email.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include <QDebug>
#include <sqlite3.h>

EMail::EMail(const int id,const QString &subject,EMailAddress *from,const QDateTime &date,const bool read,const int mailboxId,const int folderId)
    :id(id),subject(subject),from(from),date(date),_read(read),mailboxId(mailboxId),folderId(folderId)
{
}
QDateTime EMail::getDate() const
{
    return date;
}

void EMail::setDate(const QDateTime &value)
{
    date = value;
}

QString EMail::getSubject() const
{
    return subject;
}

void EMail::setSubject(const QString &value)
{
    subject = value;
}

EMailAddress* EMail::getFrom() const
{
    return from;
}

void EMail::setFrom(EMailAddress* value)
{
    from = value;
}

QList<EMailAddress*> EMail::getTo() const
{
    return to;
}

void EMail::addTo(EMailAddress *value)
{
    to.append(value);
}

QString EMail::getToAsString() const
{
    QString result = "";
    for(EMailAddress* addr: to){
        if(!addr->name().isEmpty())
            result+=addr->name()+" <"+addr->email()+">;";
        else
            result+=addr->email()+";";
    }
    return result.remove(result.length()-1,1);
}

QString EMail::getHTMLBody() const {
    return htmlBody;
}
void EMail::setHTMLBody(const QString &value){
    htmlBody = value;
}

QString EMail::getPlainTextBody() const{
    return plainTextBody;
}
void EMail::setPlainTextBody(const QString &value){
    plainTextBody = value;
}



QString EMail::getContentType() const
{
    return contentType;
}

bool EMail::isRead() const
{
    return _read;
}

QString EMail::getUID() const
{
    return uid;
}

void EMail::setUID(const QString &value)
{
    uid = value;
}

int EMail::getID() const
{
    return id;
}

void EMail::setID(const int value)
{
    id = value;
}

void EMail::save(QSqlDatabase &db)
{
    //if we have an id it means the message exists in the db
    QSqlQuery query(db);
    Q_ASSERT(!query.driver()->hasFeature(QSqlDriver::NamedPlaceholders));

    //find EMail by unique id
    query.prepare("select id from mailbox_mails where uid = :uid");
    query.bindValue(":uid",uid);
    if(!query.exec()){
        qDebug()<<query.lastError();
    }
    while(query.next()){
        id = query.value(0).toInt();
    }
    from->save(db);

    if(id > 0){
        query.prepare("UPDATE mailbox_mails "
                      "SET `from_id` = :from,"
                      " subject = :subject,"
                      " html_body = :html_body,"
                      " text_body = :text_body, "
                      " mailbox_id = :mailbox_id,"
                      " `date` = :date,"
                      " uid = :uid,"
                      " is_read = :is_read,"
                      " dir_id = :folder_id"
                      " WHERE id = :id ");
        query.bindValue(":from",from->id());
        query.bindValue(":subject",subject);
        query.bindValue(":html_body",htmlBody);
        query.bindValue(":text_body",plainTextBody);
        query.bindValue(":mailbox_id",mailboxId);
        query.bindValue(":date",date.toString());
        query.bindValue(":uid",uid);
        query.bindValue(":is_read",_read);
        query.bindValue(":id",id);
        query.bindValue(":folder_id",folderId);
    }else{
        query.prepare("INSERT INTO mailbox_mails (`from_id`,`subject`,`html_body`,`text_body`,`mailbox_id`,`date`,`uid`,`is_read`,`dir_id`) "
                      " VALUES (:pfrom,:psubject,:phtmlbody,:ptextbody,:pmailboxid,:pdate,:puid,:pisread,:pfolderid)");
        query.bindValue(":pfrom",from->id());
        query.bindValue(":psubject",subject);
        query.bindValue(":phtmlbody",htmlBody);
        query.bindValue(":ptextbody",plainTextBody);
        query.bindValue(":pmailboxid",mailboxId);
        query.bindValue(":pdate",date.toString());
        query.bindValue(":puid",uid);
        query.bindValue(":pisread",(_read)?1:0);
        query.bindValue(":pfolderid",folderId);
        //qDebug()<<"sqlite bound values: "<<sqlite3_bind_parameter_count(&query);


    }
    if(!query.exec()){
        qDebug()<<query.lastError();
    }else{
        id = query.lastInsertId().toInt();
    }

    query.prepare("INSERT INTO mail_addresses (mail_id,address_id,type) VALUES (:mail_id,:address_id,:type)");
    for (EMailAddress* adr: to){
        adr->save(db);

        query.bindValue(":mail_id",id);
        query.bindValue(":address_id",adr->id());
        query.bindValue(":type","to");
        query.exec();
    }




}

QList<EMail *> EMail::loadEmails(const int folderId)
{
       QList<EMail*> mails;
       QSqlQuery query;
       QSqlQuery adr_query;
       query.prepare("select m.id,m.subject,m.html_body,m.text_body,m.mailbox_id,m.date,m.uid,m.is_read,m.dir_id,e.id,e.name,e.email "
                     "from mailbox_mails m JOIN emailaddresses e ON m.from_id = e.id "
                     "where m.dir_id = :folder_id");
       adr_query.prepare("SELECT m.name,m.email,adr.type FROM mail_addresses adr JOIN emailaddresses m ON adr.address_id = m.id WHERE mail_id = :mail_id");
       query.bindValue(":folder_id",folderId);
       if(!query.exec()){
           qDebug()<<query.lastError();
       }
       while(query.next()){
           int _id = query.value(0).toInt();

           QString _subject = query.value(1).toString();
           QString _htmlBody = query.value(2).toString();
           QString _textBody = query.value(3).toString();
           int _mailboxId = query.value(4).toInt();
           //QDateTime _date = query.value(6).toDateTime();
           QDateTime _date = QDateTime::fromString(query.value(5).toString());
           qDebug()<<"Date: "<<QDateTime::fromString(query.value(5).toString());
           QString _uid = query.value(6).toString();
           bool _isRead = query.value(7).toBool();
           int _dirId = query.value(8).toInt();
           int _fromId = query.value(9).toInt();
           QString _mailName = query.value(10).toString();
           QString _email = query.value(11).toString();
           EMail *mail = new EMail(_id,_subject,new EMailAddress(_fromId,_email,_mailName),_date,_isRead,_mailboxId,_dirId );
           mail->setHTMLBody(_htmlBody);
           mail->setPlainTextBody(_textBody);
           mail->setUID(_uid);

           adr_query.bindValue(":mail_id",_id);
           adr_query.exec();
           while(adr_query.next()){
               QString _name = adr_query.value(0).toString();
               QString _email = adr_query.value(1).toString();
               QString _type = adr_query.value(2).toString();
               EMailAddress* adr = new EMailAddress(_email,_name);
               if(_type == "to"){
                   mail->addTo(adr);
               }
           }
           mails.append(mail);
       }

       return mails;
}

