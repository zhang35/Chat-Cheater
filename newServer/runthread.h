#ifndef RUNTHREAD_H
#define RUNTHREAD_H

#include <QThread>
#include <QPixmap>
class runthread : public QThread
{
    Q_OBJECT
public:
    runthread();
    void run();
    void stop();
    volatile bool stoprun;
    volatile int winw;
    volatile int winh;
    QString type;

signals:
    void sendimage(QByteArray ba);

private:
    QPixmap newpixmap;
    QPixmap oldpixmap;

private slots:
};
#endif // RUNTHREAD_H
