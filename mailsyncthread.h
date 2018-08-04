#ifndef MAILSYNCTHREAD_H
#define MAILSYNCTHREAD_H

#include <QThread>
#include "mailbox.h"
#include "email.h"
#include <QList>
#include <QMetaType>
#include <vmime/vmime.hpp>

Q_DECLARE_METATYPE(QList<EMail*>)
class MailSyncThread : public QThread
{
    Q_OBJECT
public:
    MailSyncThread(QList<Mailbox*> mailboxes,QObject *parent = nullptr);

protected:
    void run() override;

private:
    QList<Mailbox*> mailboxes;
    void syncFolders(vmime::shared_ptr<vmime::net::store> st,Mailbox* mailbox);


signals:
    void resultsReady();
    void startProgress(int maxProgress);
    void progress(int progress);
};

#endif // MAILSYNCTHREAD_H
