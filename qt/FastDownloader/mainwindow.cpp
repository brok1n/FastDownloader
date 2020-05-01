#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    QFile file(":/fd/css/default.css");
    file.open(QFile::ReadOnly);
    QString style = QLatin1String(file.readAll());
    this->setStyleSheet(style);
    file.close();

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
    retranslateUi(this);

    DataCenter::GetInstance(this);

    startUrlWatcher();
}

MainWindow::~MainWindow()
{
    mApp = NULL;
    mSysTrayIcon->deleteLater( );
    delete ui;
}

void MainWindow::retranslateUi(QMainWindow*)
{

}

void MainWindow::init(QApplication *app)
{
    mApp = app;
    // application icon
    QIcon icon = QIcon(":/fd/images/logo128.jpg");
    setWindowIcon(icon);
    setIconSize(QSize(128, 128));
}

void MainWindow::addTask(QString url, QString downloadDir)
{
    qDebug("下载任务: %s", url.toStdString().c_str());
    qDebug("保存路径: %s", downloadDir.toStdString().c_str());
    DownloadManager::GetInstance()->downloadFile(url, downloadDir);
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
    mApp->quit();
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
