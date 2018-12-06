#ifndef MYFILEINFO_H
#define MYFILEINFO_H


#include <QString>

struct myFileInfo
{
    QString fileName;
    QString fileSize;
    QString fileDate;
    QString fileAbsolutePath;
    bool isDir;
};



#endif // MYFILEINFO_H
