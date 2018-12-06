#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProcess>
#include <QDragEnterEvent>
#include <QDropEvent>
#include "pipeserver.h"
#include "clientsocket.h"
#include "runthread.h"
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
    PipeServer *tcpSv;
    ClientSocket *tcpSck; 
    HistoryDialog *historyDialog;

    QProcess *newpro;
    runthread *thread;

    void runServer();
    void aboutApp();
    void showIP();
    void setBackGround(QString path);

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void openFile(QString path);

    QString dragFileName;
signals:
    void writeMessage(QString type, QString msg);
    void beginLoadFile(QString file);
private slots:
    void on_action_IP_triggered();
    void on_action_history_triggered();
    void on_clearButton_clicked();
    void on_action_about_triggered();
    void on_action_sendFile_triggered();
    void on_sendButton_clicked();
    void connectionSuccess();
    void connectionClosed();
    void getLocalInfo(QString tip, QString msga, QString msgb);

    void showMessage(QString);

    void closeEvent(QCloseEvent *event);

    void runprocess(QString type, QString msg);
    void readprocessmsg();

    void dropfile();
};

#endif // MAINWINDOW_H
