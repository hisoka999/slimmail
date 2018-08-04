#include "mailboxmodel.h"
#include "mailboxtreeitem.h"
#include <QDebug>
#include <QFileIconProvider>
#include <QFont>

MailboxModel::MailboxModel(QList<Mailbox*> mailboxes, QObject *parent)
    :QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Title";
    rootItem = new MailboxTreeItem(rootData);
    setupModelData(mailboxes, rootItem);


}
void MailboxModel::setupModelData(const QList<Mailbox*> mailboxs, MailboxTreeItem *parent){

    // todo directories fehlen noch
    for(Mailbox *mailbox : mailboxs){
        QList<QVariant> data;
        data<<mailbox->name();
        qDebug()<<"name: "<<mailbox->name()<<0<<mailbox->name();
        MailboxTreeItem* treeItem = new MailboxTreeItem(data,parent);
        parent->appendChild(treeItem);

        for(auto folder : mailbox->directories()){
            QList<QVariant> folderData;
            folderData<<folder->label()<<folder->numUnread()<<folder->internalName();
            treeItem->appendChild(new MailboxTreeItem(folderData,treeItem));
        }
    }
}

MailboxModel::~MailboxModel()
{
    delete rootItem;
}

int MailboxModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant MailboxModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    MailboxTreeItem *item = static_cast<MailboxTreeItem*>(index.internalPointer());

    if(role != Qt::DisplayRole && role != Qt::DecorationRole){
        if(role == Qt::FontRole){
            if(item->parentItem() == rootItem){
                QFont font;
                font.setBold(true);
                return font;
            }

        }
        return QVariant();


    }
    MailboxTreeItem *parentItem = item->parentItem();

    if(role == Qt::DecorationRole){
        if (parentItem == rootItem)
            return QIcon::fromTheme("server");

        QFileIconProvider pro;

        return pro.icon(QFileIconProvider::Folder).pixmap(QSize(16,16));
    }

    int numUnread =  item->data(1).toInt();
    QString text = item->data(index.column()).toString();
    if(numUnread > 0 && index.column() == 0)
        return QString(text+" ("+QString::number(numUnread)+")");
    return text;
}

Qt::ItemFlags MailboxModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant MailboxModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex MailboxModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    MailboxTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<MailboxTreeItem*>(parent.internalPointer());

    MailboxTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex MailboxModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    MailboxTreeItem *childItem = static_cast<MailboxTreeItem*>(index.internalPointer());
    MailboxTreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int MailboxModel::rowCount(const QModelIndex &parent) const
{
    MailboxTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<MailboxTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}
