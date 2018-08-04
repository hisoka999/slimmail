#include "mailboxdirectories.h"

MailboxDirectories::MailboxDirectories(const int id,const QString label, const QString internalName,
                                       const int sync,const int mailboxId, const int parentId)
    :_id(id),_label(label),_internalName(internalName),_sync(sync),_mailboxId(mailboxId),_parent(parentId)
{

}

int MailboxDirectories::numUnread() const
{
    int val = 0;
    for(auto email: emails){
        if(!email->isRead())val++;
    }
    return val;

}
