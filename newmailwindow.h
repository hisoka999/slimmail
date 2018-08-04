#ifndef NEWMAILWINDOW_H
#define NEWMAILWINDOW_H

#include <QMainWindow>
#include <mailbox.h>
#include <QFontComboBox>
#include <QSqlTableModel>
#include <delimitedcompleter.h>

namespace Ui {
class NewMailWindow;
}

class NewMailWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NewMailWindow(Mailbox* mailbox,QWidget *parent = 0);
    ~NewMailWindow();

private slots:
    void on_actionBold_triggered();

    void on_actionBold_triggered(bool checked);

    void on_actionUnderlinded_triggered(bool checked);

    void currentFontChanged(const QFont &font);

    void on_toLineEdit_textChanged(const QString &arg1);

    void on_actionSend_triggered();

private:
    Ui::NewMailWindow *ui;
    Mailbox *_mailbox;

    QFontComboBox* fontCombobox;
    DelimitedCompleter *completer;
    QSqlTableModel *adrModel;
};

#endif // NEWMAILWINDOW_H
