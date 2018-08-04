#ifndef MAILBOXDIRECTORIES_H
#define MAILBOXDIRECTORIES_H

#include <QString>
#include <QList>
#include <email.h>

class MailboxDirectories
{
public:
    MailboxDirectories(const int id,const QString label, const QString internalName,const int sync,const int mailboxId, const int parentId);

    QList<EMail*> getMails(){
        return emails;
    }

    void setMails(QList<EMail*> mails){
        emails = mails;
    }

    QString label() const{
        return _label;
    }

    QString internalName() const{
        return _internalName;
    }
    int id() const {
        return _id;
    }

    int mailboxId() const {
        return _mailboxId;
    }

    QString maxUID() const {
        return _maxUID;
    }

    void setMaxUID(const QString &value){
        _maxUID = value;
    }

    int numUnread() const;
private:
    int _id;
    QString _label;
    QString _internalName;
    int _sync;
    int _mailboxId;
    int _parent;
    QList<EMail*> emails;
    QString _maxUID;
};

#endif // MAILBOXDIRECTORIES_H
