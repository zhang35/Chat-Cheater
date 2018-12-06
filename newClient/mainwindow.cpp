#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDateTime>
#include <QUrl>
#include <QTimer>
#include <QListWidgetItem>
MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(this->width(), this->height());   //固定大小

    status_label = new QLabel(this);
    info_label = new QLabel(this);

    ui->statusBar->addWidget(status_label);
    ui->statusBar->addWidget(info_label);

    ui->progressBar->hide();

    ui->superButton->setEnabled(false);
    ui->superButton->setToolTip(tr("请先建立连接"));

    this->setWindowTitle(tr("Chat Client"));
    info_label->setText(tr("欢迎使用！"));
    status_label->setText(tr("未连接"));

    this->setBackGround(tr(":/actions/icons/blue.png"));
    this->setWindowIcon(QIcon(":/actions/icons/windowicon.png"));

    connect(ui->ip_LineEdit, SIGNAL(textEdited(QString)), this, SLOT(checkip(QString)));
    connect(ui->action_quit, SIGNAL(triggered()), this, SLOT(close()));

    tcpSck = new ClientSocket(this);
    connect(tcpSck, SIGNAL(connected()), this, SLOT(connectionSuccess()));
    connect(tcpSck, SIGNAL(disconnected()), this, SLOT(connectionClosed()));
    connect(tcpSck, SIGNAL(sendLocalInfo(QString,QString)), this, SLOT(getLocalInfo(QString,QString)));
    connect(this, SIGNAL(writeMessage(QString,QString)), tcpSck, SLOT(writeMessage(QString,QString)));
    connect(tcpSck, SIGNAL(messageReceived(QString)), this, SLOT(showMessage(QString)));
    connect(this, SIGNAL(beginLoadFile(QString)), tcpSck, SLOT(loadinfile(QString)));

    this->setAcceptDrops(true);    //允许拖放

    historyDialog = new HistoryDialog(this);

    isLogin = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setBackGround(QString path)
{
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,QBrush(QPixmap(path).scaled(
            this->size(),
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation))); // 使用平滑的缩放方式

    this->setPalette(palette); // 至此, 已给widget加上了背景图.
}

void MainWindow::connectionSuccess()
{
    isLogin = true;

    ui->action_connect->setText(tr("断开"));
    ui->action_connect->setToolTip(tr("断开"));
    ui->action_connect->setIcon(QIcon(":/actions/icons/disconnect.png"));

    ui->superButton->setEnabled(true);
    ui->superButton->setToolTip(tr("进入超级模式"));
    status_label->setText(tr("已连接！"));
    info_label->setText("");
}

void MainWindow::connectionClosed()
{
    isLogin = false;

    ui->action_connect->setText(tr("连接"));
    ui->action_connect->setToolTip(tr("连接"));
    ui->action_connect->setIcon(QIcon(":/actions/icons/connect.png"));

    ui->superButton->setEnabled(false);
    ui->superButton->setToolTip(tr("请先建立连接"));
    status_label->setText(tr("连接已断开！"));

    info_label->clear();

    controlWindow->close();
}

void MainWindow::checkip(QString ip)
{
    if(ip.size()>0)
    {
        QChar c=ip.at(ip.size()-1);
        if( (c>='0'&&c<='9') || (c=='.') )
        {
            info_label->setText(tr(""));
        }
        else
        {
            ui->ip_LineEdit->setText(ip.remove(ip.size()-1,ip.size()));
            info_label->setText(tr("输入错误：请输入数字或."));
        }

        QString partip=ip;
        QString parttwo;
        if(ip.indexOf('.')>0) parttwo=partip.remove(0,partip.lastIndexOf('.')+1);
        else parttwo=partip;

        if(parttwo.toInt()<=255);
        else
        {
            info_label->setText(tr("输入错误:每分段值域为0-255！"));
            ui->ip_LineEdit->setText(ip.remove(ip.size()-1,ip.size()));
        }

        if((ip.lastIndexOf('.')>0&&(ip.size()-ip.lastIndexOf('.'))==4)||(ip.lastIndexOf('.')<0&&ip.size()==3))
        {
            int pointnum=0;
            for(int i=0;i<ip.size();i++)
            {
                if(ip.at(i)=='.')pointnum+=1;
            }
            if(pointnum==3);
            else ui->ip_LineEdit->setText(ip.append("."));
        }
    }
}

void MainWindow::on_sendButton_clicked()
{
    QString msg = ui->textEdit->toPlainText();

    if (msg == "")
    {
        info_label->setText("消息不能为空");
        return;
    }

    emit writeMessage("msg", msg);
    ui->textEdit->clear();

    QDateTime time=QDateTime::currentDateTime();
    QString timeStr=time.toString("h:m:s ap");

    QListWidgetItem *item = new QListWidgetItem(timeStr + "\n" + msg);
    item->setTextColor(QColor(0, 0, 255));
    ui->listWidget->addItem(item);

    timeStr=time.toString("yyyy-MM-dd h:m:s ap");
    historyDialog->appendMsg(timeStr + " from local\n" + msg);

}

void MainWindow::showMessage(QString msg)
{
    QDateTime time=QDateTime::currentDateTime();
    QString timeStr=time.toString("h:m:s ap");

    QListWidgetItem *item = new QListWidgetItem(timeStr + "\n" + msg);
    item->setTextColor(QColor(255, 0, 0));
    ui->listWidget->addItem(item);

    timeStr = time.toString("yyyy-MM-dd h:m:s ap");
    historyDialog->appendMsg(timeStr + " from server\n" + msg);

}

void MainWindow::getLocalInfo(QString tip, QString msg)
{
    if(tip=="begin")
    {
        ui->progressBar->show();
        info_label->setText(tr("正在接收") + msg);
        //ui->progressBar->setFormat("文件："+msg+" 已接收文件%p%");
    }
    if(tip=="progressm")
    {
        ui->progressBar->setMaximum(msg.toInt());
    }
    if(tip=="progressv")
    {
        ui->progressBar->setValue(msg.toInt());
    }

    if(tip=="sendfiledone")
    {
        info_label->setText("传送成功：" + msg);
        ui->progressBar->hide();
    }

    if(tip=="receivefiledone")
    {
        info_label->setText("接收成功：" + msg);
        ui->progressBar->hide();
    }

    if(tip=="fileerror")
    {
        info_label->setText(tr("文件打开失败"));
    }
    if(tip=="img")
    {
        this->setWindowTitle(this->windowTitle()+msg);
    }
    if(tip=="dataerror")
    {
        info_label->setText(tr("接收错误"));
    }
    if(tip=="tcperror")
    {
        status_label->setText(tr("欢迎使用"));
    }
    if(tip=="update")
    {
        controlWindow->insertwidget(msg);
    }
}

//passfile //
void MainWindow::openFile(QString path)
{

    QString sentname = path.right(path.size() - path.lastIndexOf('/')-1);
    switch( QMessageBox::information(this,tr("文件传送提示")
                                     ,tr("你确定传送文件：%1 ?").arg(sentname),tr("确定"),tr("取消"),0,1))
    {
    case 0:
        emit beginLoadFile(path);
        break;
    case 1:
        return;
    }
}

void MainWindow::on_action_sendFile_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("选择传送文件："));
    if(fileName.isEmpty())
        return;
    openFile(fileName);
}

void MainWindow::on_action_connect_triggered()
{
    if (isLogin)
    {
        tcpSck->disconnectFromHost();
    }
    else
    {
        QString ip  = ui->ip_LineEdit->text();
        tcpSck->connectToHost(ip, 3535);
        status_label->setText(tr("正在连接……"));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    switch( QMessageBox::information(this,tr("提示"),tr("你确定要退出么?"),tr("确定"),tr("取消"),0,1))
    {
    case 0:{
            event->accept();
            break;
        }
    case 1:event->ignore();//取消
        break;
    }
}

//Super Mode
void MainWindow::GotoSuperMode()
{
    controlWindow = new ControlWindow(this);
    connect(tcpSck, SIGNAL(imageReceived(QPixmap)), controlWindow, SLOT(imageReceived(QPixmap)));
    connect(tcpSck, SIGNAL(dirReceived(QList<myFileInfo>)), controlWindow, SLOT(dirReceived(QList<myFileInfo>)));
    connect(controlWindow, SIGNAL(sendRequest(QString,QString,QString)),
            tcpSck, SLOT(sendRequest(QString,QString,QString)));
    connect(controlWindow, SIGNAL(controlWindowClosed()), this, SLOT(controlWindowClosed()));

    controlWindow->show();
    controlWindow->stopImage(false);
    ui->action_sendFile->setEnabled(false);
    ui->superButton->setEnabled(false);
}

void MainWindow::on_superButton_clicked()
{
    GotoSuperMode();
}


void MainWindow::on_action_super_triggered()
{
    GotoSuperMode();
}

void MainWindow::controlWindowClosed()
{
    ui->action_sendFile->setEnabled(true);
    ui->superButton->setEnabled(true);
}

void MainWindow::aboutApp()
{
    switch( QMessageBox::information( this,tr("关于程序")
                                      ,tr("本程序为聊天工具的客户端，用于连接服务端。"
                                          "\n编自：张家齐，张文涛，亢鹏，for软件设计大赛。"),tr("确定"),0))
    {
    case 0:break;
    }
}

void MainWindow::on_action_about_triggered()
{
    aboutApp();
}


void MainWindow::on_clearButton_clicked()
{
    ui->listWidget->clear();
    info_label->clear();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{

    if(event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();

}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls=event->mimeData()->urls();
    if(urls.isEmpty())
        return;
    dragFileName=urls.first().toLocalFile();
    if(dragFileName.isEmpty())
        return;


    QTimer::singleShot(10,this,SLOT(dropfile()));    //必要的，原因不明
}

void MainWindow::dropfile()
{
    openFile(dragFileName);
}

void MainWindow::on_action_history_triggered()
{
    historyDialog->show();
    historyDialog->setGeometry(this->x()+this->width(), this->y()+30, this->width(), this->height());

}

