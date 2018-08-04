#ifndef BOLDDELEGATE_H
#define BOLDDELEGATE_H

#include <QStyledItemDelegate>

class BoldDelegate : public QStyledItemDelegate
{
public:
    BoldDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // BOLDDELEGATE_H
