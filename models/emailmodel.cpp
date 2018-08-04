#include "emailmodel.h"
#include <QFont>

EMailModel::EMailModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    setHeaderData(0, Qt::Horizontal, QObject::tr("Subject"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("From"));
    setHeaderData(2, Qt::Horizontal, QObject::tr("Date"));

}


int EMailModel::rowCount(const QModelIndex & /*parent*/) const
{
   return emails.count();
}

int EMailModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 6;
}

QVariant EMailModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column()){

        case 1:
            return emails.at(index.row())->getFrom()->toString();
        case 2:
            return emails.at(index.row())->getDate();
        case 0:
            return emails.at(index.row())->getSubject();
        case 3:
            return emails.at(index.row())->getToAsString();
        case 4:
            return emails.at(index.row())->getHTMLBody();
        case 5:
            return emails.at(index.row())->getPlainTextBody();
        default:
            break;
        }
    }
    if(role == Qt::FontRole){
        if(!emails.at(index.row())->isRead()){
            QFont font;
            font.setBold(true);
            return font;
        }

    }


    return QVariant();
}

QVariant EMailModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Subject");

            case 1:
                return tr("From");
            case 2:
                return tr("Date");

            default:
                return QVariant();
        }
    }
    return QVariant();
}

void EMailModel::sort(int column, Qt::SortOrder order)
{
    this->beginResetModel();
    std::sort(emails.begin(),emails.end(),[&](const EMail* v1,const EMail* v2){
        if (order == Qt::DescendingOrder){
            switch(column){
                case 0:
                    return v1->getSubject().compare(v2->getSubject()) == 1;
                case 1:
                    return v1->getFrom()->toString().compare(v2->getFrom()->toString()) == 1;
                case 2:
                    return v1->getDate() > v2->getDate();
            }
        }else{
            switch(column){
                case 0:
                    return v1->getSubject().compare(v2->getSubject()) == -1;
                case 1:
                    return v1->getFrom()->toString().compare(v2->getFrom()->toString()) == -1;
                case 2:
                    return v1->getDate() < v2->getDate();
            }
        }
        return false;
    });
    endResetModel();
}

QList<EMail *> EMailModel::getEmails() const
{
    return emails;
}

void EMailModel::setEmails(const QList<EMail *> &value)
{
    emails = value;
    this->endResetModel();
}
