#include "runthread.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QBuffer>

runthread::runthread()
{
    stoprun=false;
    type="img";
    winw=560;
    winh=320;

    //调用QApplication::desktop()是不可少的，否则run函数里的QApplication::desktop()会引发崩溃，原因不明。
    int xx=QApplication::desktop()->width()/2;
    int yy=QApplication::desktop()->height()/2;
    xx = yy = 0;
}

void runthread::stop()
{
    stoprun=true;
}

void runthread::run()
{
    stoprun = false;
    while (!stoprun)
    {
        newpixmap = QPixmap::grabWindow(QApplication::desktop()->winId()).scaled(winw,winh,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        if(oldpixmap.toImage()!=newpixmap.toImage())
        {
            oldpixmap=newpixmap;

            QByteArray bytearray;
            QBuffer buffer(&bytearray);
            buffer.open(QIODevice::WriteOnly);

            QDataStream out(&buffer);
            out <<  qint64(0) << type << newpixmap;
            out.device()->seek(0);
            out << qint64(bytearray.size() - sizeof(qint64));

            emit sendimage(bytearray);
        }
    }
}
