#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QtNetwork/QTcpSocket>
#include <QFile>
#include <QPixmap>
#include <QtSql/QSqlTableModel>
#include <QCursor>
#include <QDir>

class ClientSocket : public QTcpSocket
{
    Q_OBJECT

public:
    ClientSocket(QObject *parent = 0);

private:
    qint64 nextBlockSize;
    QString type;

    QDataStream in;
    //接收文件//
    qint64 recvtotalBytes;  //存放总大小信息
    qint64 recvbytesReceived;  //已收到数据的大小
    qint64 recvfileNameSize;  //文件名的大小信息
    QString recvfileName;   //存放文件名
    QFile *recvlocalFile;   //本地文件
    QByteArray recvinBlock;   //数据缓冲区
    bool getpassfile;

    //发送文件//
    QFile *sendlocalFile;
    QString sendcurrentFileName;
    qint64 sendtotalBytes;//数据总大小
    qint64 sendbytesWritten;//已经发送数据大小
    qint64 sendbytesToWrite;//剩余数据大小
    qint64 sendloadSize;//每次发送数据的大小
    QByteArray sendoutBlock;  //数据缓冲区，即存放每次要发送的数据
    bool hideLocalInfo;

    //QPixmap//
    QPixmap pixmap;

    //Keybord and Mouse Control
    int ww;
    int hh;
    QCursor mouse;
    QSqlTableModel *keyid;
    void mousepress(bool LR,int x,int y);
    void mousemove(int x,int y);
    void mouserelease(bool LR,int x,int y);
    void mousedoubleclick();
    void keybord(bool PR,int key,QString text);
    int keytokey(int key);
signals:
    void sendLocalInfo(QString tip, QString msga, QString msgb);
    void messageReceived(QString msg);
    void runThread();
public slots:
    //message//
    void writeMessage(QString type, QString msg);

    //Image//
    void writeimage(QByteArray ba);

    //file//
    void loadinfile(QString filename);
    void updateProgress(qint64 numBytes);

    //getfile//
    void sendLocalDir(QDir dir);
    //read//
    void readClient();
    void receiveMessage();
    void startReceiveFile();
    void receiveFileProcess();
};

#endif // CLIENTSOCKET_H
