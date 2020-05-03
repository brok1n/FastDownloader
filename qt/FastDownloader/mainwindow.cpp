#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include "ui_downloaditemui.h"
#include "downloaditemui.h"
#include "downloadtask.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mNewTaskDialog(Q_NULLPTR)
    , mUrlWatcherThread(Q_NULLPTR)
    , mUrlWatcher(Q_NULLPTR)
{
    ui->setupUi(this);
    //无标题栏 无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    //默认样式
    this->setStyle("default");
    //托盘
    this->initSystemTray();

    //设置图标
    QIcon appIcon = QIcon(":/fd/images/logo128.jpg");
    qApp->setWindowIcon(appIcon);
    setWindowIcon(appIcon);
    setIconSize(QSize(128, 128));

    DataCenter::GetInstance(this);

    startUrlWatcher();

    retranslateUi(this);

}

MainWindow::~MainWindow()
{
    mSysTrayIcon->deleteLater( );
    delete ui;
}

void MainWindow::retranslateUi(QMainWindow*)
{

}

void MainWindow::addTask(QString url, QString downloadDir)
{
    qDebug("下载任务: %s", url.toStdString().c_str());
    qDebug("保存路径: %s", downloadDir.toStdString().c_str());
    DownloadTask *task = DownloadManager::GetInstance()->downloadFile(url, downloadDir);

    QListWidgetItem *qItem = new QListWidgetItem(this->ui->downloadList);
    qItem->setBackground(QColor(222, 237, 252));
    qItem->setSizeHint(QSize(350, 60));

    DownloadItemUi *listItem = new DownloadItemUi(this->ui->downloadList);
    listItem->setStyleSheet("margin-bottom: 5px;");
    listItem->show();
    listItem->bindDownloadTask(task);
    task->bindUi(listItem);
    this->ui->downloadList->setItemWidget(qItem, listItem);



//    DownloadTask *task2 = DownloadManager::GetInstance()->downloadFile(url, downloadDir, false);

//    QListWidgetItem *qItem2 = new QListWidgetItem(this->ui->downloadList);
//    qItem2->setBackground(QColor(222, 237, 252));
//    qItem2->setSizeHint(QSize(350, 55));

//    DownloadItemUi *listItem2 = new DownloadItemUi(this->ui->downloadList);
//    listItem2->show();
//    listItem2->bindDownloadTask(task2);
//    task2->bindUi(listItem2);
//    this->ui->downloadList->setItemWidget(qItem2, listItem2);


    if(this->ui->emptyDownloadImage->isVisible()) {
        this->ui->emptyDownloadImage->setVisible(false);
    }

}

void MainWindow::startUrlWatcher()
{
    if(mUrlWatcherThread == Q_NULLPTR) {
        mUrlWatcherThread = new QThread();
    } else {
        mUrlWatcherThread->quit();
        mUrlWatcherThread->deleteLater();
        mUrlWatcherThread = Q_NULLPTR;
    }
    if(mUrlWatcherThread->isRunning()) {
        qDebug("已经在监控了！");
        return;
    }
    if(mUrlWatcher != Q_NULLPTR) {
        mUrlWatcher->deleteLater();
        mUrlWatcher = Q_NULLPTR;
    }
    mUrlWatcherThread = new QThread();
    mUrlWatcher = new UrlWatcher();
    mUrlWatcher->moveToThread(mUrlWatcherThread);

    connect(mUrlWatcherThread, &QThread::finished, mUrlWatcher, &QObject::deleteLater);
    connect(this, &MainWindow::startUrlWatcherSignal, mUrlWatcher, &UrlWatcher::watcher);

    connect(mUrlWatcher, &UrlWatcher::onUrl, this, &MainWindow::onWatchUrl);

    mUrlWatcherThread->start();

    emit startUrlWatcherSignal();

}


void MainWindow::cancelTask(QString url)
{
    if(this->mUrlWatcher != Q_NULLPTR) {
        DataCenter::GetInstance(this)->setCancelUrl(url);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
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

void MainWindow::mouseReleaseEvent(QMouseEvent*)
{
    mMoveWindow = false;
}


void MainWindow::on_closeBtn_clicked()
{
    this->hide();
}

void MainWindow::on_minBtn_clicked()
{
    this->showMinimized();
}

void MainWindow::on_newTaskBtn_clicked()
{
    if(mNewTaskDialog == Q_NULLPTR ) {
        mNewTaskDialog = new NewTaskDialog(this);
        mNewTaskDialog->setModal(false);
    }
    mNewTaskDialog->reset();
//    NewTaskDialog *newTaskDialog = new NewTaskDialog(this);
//    newTaskDialog->setModal(true);1
    mNewTaskDialog->show();
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
        case QSystemTrayIcon::Context:

            connect(mShowWindowAction, SIGNAL(triggered()), this, SLOT(on_showMainWindowAction()));
            connect(mExitAppAction, SIGNAL(triggered()), this, SLOT(on_ExitAppAction()));

            mSysTrayIcon->setContextMenu(mTrayMenu);

            break;
        default:
            break;
    }
}

void MainWindow::on_showMainWindowAction()
{
    this->show();
}

void MainWindow::on_ExitAppAction()
{
    qApp->quit();
}

void MainWindow::on_lockTopCbox_stateChanged(int arg1)
{
    if(arg1==2) {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    } else {
        setWindowFlags(windowFlags() ^ Qt::WindowStaysOnTopHint);
    }
    // 修改窗体属性之后，窗体被关闭了 需要重新让窗体显示出来
    this->show();
}

void MainWindow::onWatchUrl()
{
    if(mNewTaskDialog != Q_NULLPTR && mNewTaskDialog->isVisible()) {
        mNewTaskDialog->refresh();
    } else {
        this->on_newTaskBtn_clicked();
    }
}

void MainWindow::setStyle(QString styleName) {
    QFile file(":/fd/css/" + styleName + ".css");
    file.open(QFile::ReadOnly);
    QString style = QLatin1String(file.readAll());
    file.close();
    this->setStyleSheet(style);
}

void MainWindow::initSystemTray()
{
    //新建QSystemTrayIcon对象
    mSysTrayIcon = new QSystemTrayIcon(this);
    //新建托盘要显示的icon
    QIcon icon = QIcon(":/fd/images/logo128.jpg");
    //将icon设到QSystemTrayIcon对象中
    mSysTrayIcon->setIcon(icon);
    //当鼠标移动到托盘上的图标时，会显示此处设置的内容
    mSysTrayIcon->setToolTip(QString("快速下载器"));
    //给QSystemTrayIcon添加槽函数
    connect(mSysTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activitedSystemTrayIcon(QSystemTrayIcon::ActivationReason)));

    // 托盘菜单
    mTrayMenu = new QMenu(this);
    mShowWindowAction = new QAction(mTrayMenu);
    mExitAppAction = new QAction(mTrayMenu);

    mShowWindowAction->setText("打开");
    mExitAppAction->setText("退出");

    mTrayMenu->addAction(mShowWindowAction);
    mTrayMenu->addAction(mExitAppAction);

    connect(mShowWindowAction, SIGNAL(triggered()), this, SLOT(on_showMainWindowAction()));
    connect(mExitAppAction, SIGNAL(triggered()), this, SLOT(on_ExitAppAction()));

    mSysTrayIcon->setContextMenu(mTrayMenu);

    //在系统托盘显示此对象
    mSysTrayIcon->show();
}
