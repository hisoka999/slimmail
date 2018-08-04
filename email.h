#ifndef EMAIL_H
#define EMAIL_H

#include <QString>
#include <QStringList>
#include <QDebug>
#include <QDateTime>
#include <emailaddress.h>
#include <QList>
#include <QSqlDatabase>


class EMail
{
public:
    EMail(const int id,const QString &subject,EMailAddress *from,const QDateTime &date,const bool read,const int mailboxId,const int folderId);

    QList<EMailAddress*> getTo() const;
    void addTo(EMailAddress *value);
    QString getToAsString() const;

    EMailAddress* getFrom() const;
    void setFrom(EMailAddress *value);

    QString getSubject() const;
    void setSubject(const QString &value);

    QDateTime getDate() const;
    void setDate(const QDateTime &value);

    QString getHTMLBody() const;
    void setHTMLBody(const QString &value);

    QString getPlainTextBody() const;
    void setPlainTextBody(const QString &value);

    QString getContentType() const;

    bool isRead() const;

    QString getUID() const;
    void setUID(const QString &value);

    int getID() const;
    void setID(const int value);

    void save(QSqlDatabase &db);

    static QList<EMail *> loadEmails(const int folderId);

private:
    QString subject;
    EMailAddress* from;
    QDateTime date;
    QString htmlBody;
    QString plainTextBody;
    QString contentType;
    QList<EMailAddress*> to;
    QList<EMailAddress*> cc;
    QList<EMailAddress*> bcc;
    bool _read;
    QString uid;
    int id;
    int mailboxId;
    int folderId;
    //QDateTime date;
};

#endif // EMAIL_H
