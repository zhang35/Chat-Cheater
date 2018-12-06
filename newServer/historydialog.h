#ifndef HISTORYDIALOG_H
#define HISTORYDIALOG_H

#include <QDialog>
#include <QFile>

namespace Ui {
    class HistoryDialog;
}

class HistoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HistoryDialog(QWidget *parent = 0);
    ~HistoryDialog();

    void appendMsg(QString msg);
private:
    Ui::HistoryDialog *ui;

    QFile *file;
    quint32  MagicNumber;

    void initHistoryFile();
    void readFile();

    QDataStream in;
    QDataStream out;
};

#endif // HISTORYDIALOG_H
