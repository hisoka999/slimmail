#include "mailbox.h"

Mailbox::Mailbox(const int id, const QString username,const QString name
                 , const QString password, const QString server, const QString protocol, const int port)
    :_id(id),_username(username),_name(name),_password(password),_server(server),_protocol(protocol),_port(port)
{

}
