#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include "clientsocket.h"
#include "controlwindow.h"
#include "historydialog.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel *info_label;
    QLabel *status_label;
    ClientSocket *tcpSck;
    HistoryDialog *historyDialog;

    bool isLogin;
    ControlWindow *controlWindow;
    void aboutApp();
    void setBackGround(QString path);

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void openFile(QString path);



    QString dragFileName;
signals:
    void writeMessage(QString type, QString msg);
    void beginLoadFile(QString file);
private slots:
    void on_action_super_triggered();
    void on_action_history_triggered();
    void on_clearButton_clicked();
    void on_superButton_clicked();
    void on_action_connect_triggered();
    void on_action_sendFile_triggered();
    void on_action_about_triggered();
    void on_sendButton_clicked();
    void checkip(QString);
    void connectionSuccess();
    void connectionClosed();
    void showMessage(QString);

    void GotoSuperMode();

    void dropfile();
    void getLocalInfo(QString tip, QString msg);

    void controlWindowClosed();
    void closeEvent(QCloseEvent *event);

};

#endif // MAINWINDOW_H
