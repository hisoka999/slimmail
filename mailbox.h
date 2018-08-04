#ifndef MAILBOX_H
#define MAILBOX_H

#include <QString>
#include <mailboxdirectories.h>
#include <QList>

class Mailbox
{
public:
    Mailbox(const int id, const QString username,const QString name, const QString password, const QString server, const QString protocol, const int port);

    QString username() const{
        return _username;
    }
    QString name() const {
        return _name;
    }

    int id() const{
        return _id;
    }

    QString password() const{
        return _password;
    }

    QString server() const{
        return _server;
    }

    QString protocol() const{
        return _protocol;
    }

    int port() const{
        return _port;
    }

    QList<MailboxDirectories*> directories() const{
        return _directories;
    }

    void addDirectory(MailboxDirectories* dir){
        _directories.append(dir);
    }

    bool existsDirectory(QString internalName){
        for(auto directory : _directories){
            if(directory->internalName() == internalName)
                return true;
        }
        return false;
    }

private:
    int _id;
    QString _username;
    QString _name;
    QString _password;
    QString _server;
    QString _protocol;
    int _port;
    QList<MailboxDirectories*> _directories;
};

#endif // MAILBOX_H
