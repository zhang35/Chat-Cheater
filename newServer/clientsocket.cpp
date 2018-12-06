#include "clientsocket.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QDesktopWidget>
#include <QApplication>
#include <QtSql/QSqlRecord>
#include <windows.h>
#include <QBuffer>
ClientSocket::ClientSocket(QObject *parent)
    : QTcpSocket(parent)
{
    type="";
    nextBlockSize = 0;

    ww=560;
    hh=320;

    //文件传送
    recvtotalBytes = 0;
    recvbytesReceived = 0;
    recvfileNameSize = 0;
    getpassfile=false;

    in.setDevice(this);         //in必须与接收设备相关联
    in.setVersion(QDataStream::Qt_4_7);

    hideLocalInfo = false;

    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));

    //keybord
    keyid = new QSqlTableModel(this);
    keyid->setTable("keybordid_qt_to_win");
    keyid->select();
}


//传送消息//
void ClientSocket::writeMessage(QString type, QString msg)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << qint64(0) << type << msg;
    out.device()->seek(0);
    out << qint64(block.size() - sizeof(qint64));
    this->write(block);
}

//传送文件//
void ClientSocket::loadinfile(QString filename)
{
    sendcurrentFileName=filename.right(filename.size() - filename.lastIndexOf('/')-1);

    //文件传送//
    sendloadSize = 4*1024;
    sendtotalBytes = 0;
    sendbytesWritten = 0;
    sendbytesToWrite = 0;

    connect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(updateProgress(qint64)));
    sendlocalFile = new QFile(filename);
    if(!sendlocalFile->open(QFile::ReadOnly))
    {
        if(!hideLocalInfo)
        {
            sendLocalInfo("fileerror","openfileError", "");
        }
        return;
    }

    if (!hideLocalInfo)
    {
        sendLocalInfo("begin",sendcurrentFileName, "");
    }

    //文件总大小
    sendtotalBytes = sendlocalFile->size();

    QString type="file";
    QDataStream sendOut(&sendoutBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_6);
    sendOut << qint64(0) << type << qint64(0) << qint64(0) << sendcurrentFileName;
    sendtotalBytes += sendoutBlock.size();
    sendOut.device()->seek(0);
    sendOut<<qint64(sendoutBlock.size() - sizeof(qint64)) << type
            << sendtotalBytes <<qint64((sendoutBlock.size() - sizeof(qint64)*2));
    sendbytesToWrite = sendtotalBytes - this->write(sendoutBlock);
    sendoutBlock.resize(0);
}

void ClientSocket::updateProgress(qint64 numBytes)//更新进度条
{
    sendbytesWritten += (int)numBytes;

    if(sendbytesToWrite > 0) //如果已经发送了数据
    {
        sendoutBlock = sendlocalFile->read(qMin(sendbytesToWrite,sendloadSize));
        sendbytesToWrite -= (int)this->write(sendoutBlock);
        sendoutBlock.resize(0);
    }
    else
    {
        disconnect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(updateProgress(qint64)));
        sendlocalFile->close(); //如果没有发送任何数据，则关闭文件
    }
    //更新进度条
    QString pm; pm.setNum(sendtotalBytes);
    QString pv; pv.setNum(sendbytesWritten);

    if (!hideLocalInfo)
    {
        sendLocalInfo("progressm",pm, "");
        sendLocalInfo("progressv",pv, "");
    }
    if(sendbytesWritten == sendtotalBytes) //发送完毕
    {
        sendtotalBytes = 0;
        sendbytesWritten = 0;
        sendbytesToWrite = 0;

        if (!hideLocalInfo)
        {
            sendLocalInfo("sendfiledone",sendcurrentFileName, "");
        }

        hideLocalInfo = false;

        disconnect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(updateProgress(qint64)));
        sendlocalFile->close();
    }
}

//图像传送
void ClientSocket::writeimage(QByteArray ba)
{
    QDataStream out(&ba, QIODevice::ReadWrite);
    out.setVersion(QDataStream::Qt_4_7);
    this->write(ba);
}

void ClientSocket::readClient()
{
    if (!getpassfile)
    {
        //forever循环很有必要，直到有足够的信息到来时才能保证正常接收
        forever
        {

            if (nextBlockSize == 0)
            {
                if (bytesAvailable() < sizeof(qint64))
                    break;
                in >> nextBlockSize;
            }

            if (bytesAvailable() < nextBlockSize)
            {
                break;
            }

            in >> type;

            if (type == "msg")
            {
                receiveMessage();
                break;
            }

            if(type=="file")//接收文件
            {
                startReceiveFile();
                break;
            }

            //request
            QString msga, msgb;
            in >> msga >> msgb;         //注意：发过来的即便是空，也要完整接收！
            if(type=="super" || type=="shutdown" || type=="update" || type=="killprocess" || type=="winsize" || type=="unsuper")
            {     
                sendLocalInfo(type, msga, msgb);
            }

            if(type=="winsize")
            {
                ww = msga.toInt();
                hh = msgb.toInt();
            }

            if(type=="Lmousepress"||type=="Rmousepress")
            {
                if(type=="Lmousepress") mousepress(true,msga.toInt(),msgb.toInt());
                else mousepress(false,msga.toInt(),msgb.toInt());
            }
            if(type=="mousemov")
            {
                mousemove(msga.toInt(),msgb.toInt());
            }
            if(type=="Lmouserelease"||type=="Rmouserelease")
            {
                if(type=="Lmouserelease") mouserelease(true,msga.toInt(),msgb.toInt());
                else mouserelease(false,msga.toInt(),msgb.toInt());
            }
            if(type=="doubleclick")
            {
                mousedoubleclick();
            }
            if(type=="keypress"||type=="keyrelease")
            {
                if(type=="keypress") keybord(true,msga.toInt(),msgb);
                else keybord(false,msga.toInt(),msgb);
            }            
            if(type=="getdir")//获取目录
            {
                QDir dir(QDir::home());
                sendLocalInfo("unsuper", "", "");
                sendLocalDir(dir);
            }

            if(type=="cd")
            {
                QDir dir;
                if (dir.exists(msga))
                {
                    dir.setPath(msga);
                    if (msgb != "" )
                    {
                        dir.cd(msgb);
                    }
                    sendLocalDir(dir);
                }

            }
            if(type=="getfile")
            {
                if (QFile::exists(msga))
                {
                    hideLocalInfo = true;
                    loadinfile(msga);
                }
            }
        }
        nextBlockSize = 0;
    }

    else
    {
        receiveFileProcess();
    }
}

//消息接收
void ClientSocket::receiveMessage()
{
    QString msg;
    in >> msg;
    emit messageReceived(msg);
}

//文件接收
void ClientSocket::startReceiveFile()
{
    getpassfile=true;
    if(recvbytesReceived <= sizeof(qint64)*2)
    {
        if((bytesAvailable() >= sizeof(qint64)*2)&&(recvfileNameSize == 0))
        {
            in >> recvtotalBytes >> recvfileNameSize;
            recvbytesReceived += sizeof(qint64) * 2;

            in >> recvfileName;

            if(QFile::exists(recvfileName))
            {
                recvfileName="["+QDateTime::currentDateTime().toString("yyyyMMddhhmmss")+"] "+recvfileName;
            }

            recvbytesReceived += recvfileNameSize;
            recvlocalFile = new QFile(recvfileName);
            if(!recvlocalFile->open(QFile::WriteOnly))
            {
                sendLocalInfo("fileerror","openfileError", "");
                return;
            }
            sendLocalInfo("begin",recvfileName, "");
        }
        else return;
    }
    if(recvbytesReceived < recvtotalBytes)
    {
        recvbytesReceived += bytesAvailable();
        recvinBlock = readAll();
        recvlocalFile->write(recvinBlock);
        recvinBlock.resize(0);
    }
    //更新进度条
    QString pm; pm.setNum(recvtotalBytes);
    QString pv; pv.setNum(recvbytesReceived);
    sendLocalInfo("progressm",pm, "");
    sendLocalInfo("progressv",pv, "");

    if(recvbytesReceived == recvtotalBytes) //接收数据完成
    {
        recvlocalFile->close();
        sendLocalInfo("receivefiledone",recvfileName, "");
        recvtotalBytes = 0;
        recvbytesReceived = 0;
        recvfileNameSize = 0;
        getpassfile=false;
    }
}

void ClientSocket::receiveFileProcess()
{
    if(recvbytesReceived < recvtotalBytes)
    {
        recvbytesReceived += bytesAvailable();
        recvinBlock = readAll();
        recvlocalFile->write(recvinBlock);
        recvinBlock.resize(0);
    }
    //更新进度条
    QString pm; pm.setNum(recvtotalBytes);
    QString pv; pv.setNum(recvbytesReceived);
    sendLocalInfo("progressm",pm, "");
    sendLocalInfo("progressv",pv, "");

    if(recvbytesReceived == recvtotalBytes)
    {
        recvlocalFile->close();
        sendLocalInfo("receivefiledone",recvfileName, "");
        recvtotalBytes = 0;
        recvbytesReceived = 0;
        recvfileNameSize = 0;
        getpassfile=false;
    }

}


//Mouse
void ClientSocket::mousepress(bool LR, int x, int y)
{
#ifdef Q_WS_WIN        //声明要使用windows API
    if(LR) mouse_event (MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0 );
    else mouse_event (MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0 );
#endif
}

void ClientSocket::mousemove(int x, int y)
{
    mouse.setPos(QApplication::desktop()->width()*x/ww,QApplication::desktop()->height()*y/hh);
}

void ClientSocket::mouserelease(bool LR, int x, int y)
{
#ifdef Q_WS_WIN        //声明要使用windows API
    if(LR) mouse_event (MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );
    else mouse_event (MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0 );
#endif
}

void ClientSocket::mousedoubleclick()
{
#ifdef Q_WS_WIN        //声明要使用windows API
    mouse_event (MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );
    mouse_event (MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );
#endif                 //有ifdef，就必须有endif
}

void ClientSocket::keybord(bool PR, int key, QString text)
{
#ifdef Q_WS_WIN        //声明要使用windows API
    if(PR) keybd_event(keytokey(key),0,0,0);
    else keybd_event(keytokey(key),0,KEYEVENTF_KEYUP,0);
#endif
}

int ClientSocket::keytokey(int key)
{
    int returnkey=key;
    for(int i=0;i<keyid->rowCount();i++)
    {
        if(keyid->record(i).value("qtid").toInt()==key)
        {
            returnkey=keyid->record(i).value("winid").toInt();
            break;
        }
    }
    return returnkey;
}

void ClientSocket::sendLocalDir(QDir dir)
{
    QStringList str;
    str << "*";
    QFileInfoList list = dir.entryInfoList(str, QDir::AllEntries | QDir::Hidden | QDir::NoDot, QDir::DirsFirst);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);

    out << qint64(0) << tr("dir") << list.count();

    QFileInfo info;
    QString fileSize;
    QString fileDate;
    bool isDir;
    for (int i=0; i<list.count(); i++)
    {
        info = list.at(i);
        fileSize.setNum(info.size());
        fileDate = info.lastModified().toString("yyyy/MM/dd h:m:s ap");

        isDir = info.isDir();

        out << info.fileName() << fileSize << fileDate << info.absolutePath() << isDir;
    }

    out.device()->seek(0);
    out << qint64(block.size() - sizeof(qint64));
    this->write(block);
}
