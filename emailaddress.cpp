#include "emailaddress.h"
#include <QSqlQuery>
#include <QVariant>
EMailAddress::EMailAddress(const QString &email,const QString &name)
    :_email(email),_name(name),_id(0)
{

}

EMailAddress::EMailAddress(const int id, const QString &email, const QString &name)
    :_email(email),_name(name),_id(id)
{

}

void EMailAddress::save(QSqlDatabase &db)
{
    QSqlQuery this_query;
    QSqlQuery adr2_query;
    this_query.prepare("SELECT m.id,m.name,m.email FROM emailaddresses m WHERE m.email = :email");
    adr2_query.prepare("INSERT INTO emailaddresses (name,email) VALUES(:name,:email)");

    if(this->id() == 0){
        this_query.bindValue(":email",_email);
        this_query.exec();
        //check if the mail address exists, if so, we use it
        while(this_query.next()){
            this->setId(this_query.value(0).toInt());
        }
    }
    if(this->id() == 0){
        adr2_query.bindValue(":name",_name);
        adr2_query.bindValue(":email",_email);
        adr2_query.exec();
        _id = adr2_query.lastInsertId().toInt();
    }

}

QString EMailAddress::toString() const
{
    if(!name().isEmpty())
        return name()+" <"+email()+">";
    else
        return email();
}
