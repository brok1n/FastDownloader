#include "mainwindow.h"
#include "newtaskdialog.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //不显示标题栏，亦无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //设置背景（全透明）
//    setAttribute(Qt::WA_TranslucentBackground);
    QFile file(":/fd/css/default.css");
    file.open(QFile::ReadOnly);
    QString style = QLatin1String(file.readAll());
    this->setStyleSheet(style);

    //新建QSystemTrayIcon对象
    mSysTrayIcon = new QSystemTrayIcon(this);
    //新建托盘要显示的icon
    QIcon icon = QIcon(":/fd/images/logo128.jpg");
    //将icon设到QSystemTrayIcon对象中
    mSysTrayIcon->setIcon(icon);
    //当鼠标移动到托盘上的图标时，会显示此处设置的内容
    mSysTrayIcon->setToolTip(QObject::trUtf8("快速下载器"));
    //给QSystemTrayIcon添加槽函数
    connect(mSysTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activitedSystemTrayIcon(QSystemTrayIcon::ActivationReason)));

    //在系统托盘显示此对象
    mSysTrayIcon->show();

}



MainWindow::~MainWindow()
{
    mSysTrayIcon->deleteLater( );
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    //在标题栏上才可以移动界面
    if(event->y() < 45) {
        mMoveWindow = true;
        mPressX = event->x();
        mPressY = event->y();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(mMoveWindow) {
        int offsetX = event->x() - mPressX;
        int offsetY = event->y() - mPressY;
        this->move(this->x() + offsetX, this->y() + offsetY);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    mMoveWindow = false;
}


void MainWindow::on_closeBtn_clicked()
{
    this->hide();
}

void MainWindow::on_minBtn_clicked()
{

}

void MainWindow::on_newTaskBtn_clicked()
{
    qDebug("新建按钮被点击");
    NewTaskDialog* newTaskDialog = new NewTaskDialog(this);
    newTaskDialog->setModal(true);
    newTaskDialog->show();
}

void MainWindow::on_activitedSystemTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
        case QSystemTrayIcon::Trigger:
            //单击托盘图标
            //显示消息球，1s后自动消失
            //第一个参数是标题
            //第二个参数是消息内容
            //第三个参数图标
            //第四个参数是超时毫秒数
//            mSysTrayIcon->showMessage(QObject::trUtf8("Message Title"),
//                                      QObject::trUtf8("欢迎使用此程序"),
//                                      QSystemTrayIcon::Information,
//                                      1000);

            break;
        case QSystemTrayIcon::DoubleClick:
            //双击托盘图标
          this->show();
            break;
        default:
            break;
    }
}
