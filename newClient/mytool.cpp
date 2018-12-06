#include "mytool.h"
#include <QAction>
#include <QTimer>

mytool::mytool(QObject *) :
    QToolBar()
{
    inittoolbar();
}

void mytool::inittoolbar()
{
    isin=false;
    this->setMovable(false);

    action_showimage = new QAction(this);
    action_showimage->setText(tr("暂停画面"));
    action_showimage->setToolTip(tr("停止画面传输"));
    action_showimage->setCheckable(true);
    action_showimage->setEnabled(true);
    action_showimage->setIcon(QIcon(":/actions/icons/pause.png"));

    action_fullscreen=new QAction(this);
    action_fullscreen->setText(tr("全屏显示"));
    action_fullscreen->setToolTip(tr("最大化界面显示"));
    action_fullscreen->setIcon(QIcon(":/actions/icons/fullscreen.png"));

    action_passfile=new QAction(this);
    action_passfile->setText(tr("文件获取"));
    action_passfile->setToolTip(tr("获取对方文件"));
    action_passfile->setCheckable(true);
    action_passfile->setEnabled(true);
    action_passfile->setIcon(QIcon(":/actions/icons/sendfile.png"));

    action_process=new QAction(this);
    action_process->setText(tr("进程管理"));
    action_process->setToolTip(tr("管理远程计算机的进程"));
    action_process->setCheckable(true);
    action_process->setEnabled(true);
    action_process->setIcon(QIcon(":/actions/icons/process.png"));

    action_shutdown=new QAction(this);
    action_shutdown->setText(tr("一键关机"));
    action_shutdown->setToolTip(tr("快捷关闭远程计算机"));
    action_shutdown->setEnabled(true);
    action_shutdown->setIcon(QIcon(":/actions/icons/shutdown.png"));

    lockbox = new QCheckBox(this);
    connect(lockbox,SIGNAL(clicked(bool)),this,SLOT(lock(bool)));
    lockbox->setIcon(QIcon(":/actions/icons/lock.png"));
    lockbox->setIconSize(QSize(25,30));

    this->addAction(action_showimage);

    this->addAction(action_passfile);
    this->addAction(action_process);
    this->addAction(action_shutdown);
    this->addSeparator();
    this->addAction(action_fullscreen);
    this->addWidget(lockbox);

    this->setIconSize(QSize(23, 30));
    this->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

void mytool::enterEvent(QEvent *)
{
    if(!lockbox->isChecked())
    {
        isin = true;
        this->setMaximumHeight(40);
        this->setMinimumHeight(40);
    }
}

void mytool::leaveEvent(QEvent *)
{
    if(!lockbox->isChecked())
    {
        isin = false;
        QTimer::singleShot(1000, this, SLOT(timeout()));
    }
}

void mytool::lock(bool islock)
{
    if(islock)
    {
        lockbox->setText(tr("解锁"));
        lockbox->setToolTip(tr("解锁工具栏和状态栏，自动隐藏"));
        lockbox->setIcon(QIcon(":/actions/icons/unlock.png"));
    }
    else
    {
        lockbox->setText(tr("锁定"));
        lockbox->setToolTip(tr("锁定工具栏和状态栏，不自动隐藏"));
        lockbox->setIcon(QIcon(":/actions/icons/lock.png"));
    }
}

void mytool::timeout()
{
    if(!isin)
    {
        this->setMaximumHeight(2);
        this->setMinimumHeight(2);
    }
}
