#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QtNetwork/QTcpSocket>
#include <QFile>
#include <QPixmap>
#include "myFileInfo.h"

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

    QPixmap pixmap;
signals:
    void sendLocalInfo(QString tip, QString msg);
    void messageReceived(QString msg);
    void imageReceived(QPixmap pixmap);
    void dirReceived(QList<myFileInfo> list);
public slots:
    void writeMessage(QString type, QString msg);
    void sendRequest(QString type,QString msga,QString msgb);
    void loadinfile(QString path);
    void updateProgress(qint64 numBytes);

    void readClient();
    void receiveMessage();
    void startReceiveFile();
    void receiveFileProcess();
    void receiveDir();

    void tcperror(QAbstractSocket::SocketError er);
    void showmessagebox(QString title,QString msg);
    bool isSendingFile();
};

#endif // CLIENTSOCKET_H
