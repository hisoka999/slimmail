#ifndef EMAILADDRESS_H
#define EMAILADDRESS_H

#include <QString>
#include <QSqlDatabase>

class EMailAddress
{
public:
    EMailAddress(const QString &email,const QString &name);

    EMailAddress(const int id,const QString &email,const QString &name);

    QString email() const{
        return _email;
    }

    QString name() const{
        return _name;
    }

    int id() const{
        return _id;
    }

    void setId(const int value){
        _id = value;
    }

    void save(QSqlDatabase &db);

    QString toString() const;
private:
    QString _name;
    QString _email;
    int _id;
};

#endif // EMAILADDRESS_H
