#ifndef CONTROLWINDOW_H
#define CONTROLWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QModelIndex>
#include "mytool.h"
#include <QCloseEvent>
#include "myFileInfo.h"

namespace Ui {
    class ControlWindow;
}

class ControlWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ControlWindow(QWidget *parent = 0);
    ~ControlWindow();

    void connectionClosed();

    //进程管理
    int processrow;
    void insertwidget(QString newline);
    void updateprocess();
signals:
    void sendRequest(QString type,QString msga,QString msgb);
    void controlWindowClosed();
private:
    Ui::ControlWindow *ui;
    mytool *newtool;

    QPixmap pixmap;

    QList<myFileInfo> fileList;
    QString currentPath;
    void showFileInfoList();
private slots:

    void on_enterButton_clicked();
    void on_tableWidget_file_doubleClicked(QModelIndex index);

    void on_killButton_clicked();
    void on_refreshButton_clicked();
    void on_tableWidget_clicked(QModelIndex index);

    //图像显示
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

    //鼠标控制
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    //键盘控制
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    //文件获取
    void getFile(bool on);


    void closeEvent(QCloseEvent *event);
public slots:
    void stopImage(bool stop);
    void fullScreen();
    void flashProcess(bool);
    void shutdown();



    void imageReceived(QPixmap pixmapReceived);
    void dirReceived(QList<myFileInfo> list);
    void killprocess();
};

#endif // CONTROLWINDOW_H
