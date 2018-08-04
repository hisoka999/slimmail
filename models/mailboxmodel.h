#ifndef MAILBOXMODEL_H
#define MAILBOXMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include "mailboxtreeitem.h"
#include "mailbox.h"

class MailboxModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit MailboxModel(QList<Mailbox*> mailboxes, QObject *parent = 0);
    ~MailboxModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void setupModelData(const QList<Mailbox*> mailboxs, MailboxTreeItem *parent);

    MailboxTreeItem* rootItem;
};

#endif // MAILBOXMODEL_H
