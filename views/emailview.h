#ifndef EMAILVIEW_H
#define EMAILVIEW_H

#include <QWidget>
#include <QModelIndex>
#include <QDataWidgetMapper>
#include <models/emailmodel.h>
namespace Ui {
class EMailView;
}

class EMailView : public QWidget
{
    Q_OBJECT

public:
    explicit EMailView(QWidget *parent = 0);
    ~EMailView();
    void setModel(EMailModel* model);

public slots:
    void setCurrentModelIndex(const QModelIndex &index);
private slots:
    void on_webView_linkClicked(const QUrl &arg1);

private:
    Ui::EMailView *ui;
    EMailModel* model;
    QDataWidgetMapper *mapper;
};

#endif // EMAILVIEW_H
