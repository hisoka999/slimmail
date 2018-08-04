#ifndef EMAILMODEL_H
#define EMAILMODEL_H

#include <QAbstractTableModel>
#include <email.h>

class EMailModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit EMailModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QList<EMail *> getEmails() const;
    void setEmails(const QList<EMail *> &value);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void sort(int column, Qt::SortOrder order);
signals:

public slots:

private:
    QList<EMail*> emails;

};

#endif // EMAILMODEL_H
