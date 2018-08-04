#include "mailboxtreeitem.h"


MailboxTreeItem::MailboxTreeItem(const QList<QVariant> &data, MailboxTreeItem *parent)
{
    m_parentItem = parent;
    m_itemData = data;
}

MailboxTreeItem::~MailboxTreeItem()
{
    qDeleteAll(m_childItems);
}

void MailboxTreeItem::appendChild(MailboxTreeItem *item)
{
    m_childItems.append(item);
}

MailboxTreeItem *MailboxTreeItem::child(int row)
{
    return m_childItems.value(row);
}

int MailboxTreeItem::childCount() const
{
    return m_childItems.count();
}

int MailboxTreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant MailboxTreeItem::data(int column) const
{
    return m_itemData.value(column);
}

MailboxTreeItem *MailboxTreeItem::parentItem()
{
    return m_parentItem;
}

int MailboxTreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<MailboxTreeItem*>(this));

    return 0;
}
