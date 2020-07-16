#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include "ui_downloaditemui.h"
#include "downloaditemui.h"
#include "downloadtask.h"
#include "helpdialog.h"

#include <QTranslator>
#include <QLocale>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mNewTaskDialog(Q_NULLPTR)
    , mUrlWatcherThread(Q_NULLPTR)
    , mUrlWatcher(Q_NULLPTR)
{

    ui->setupUi(this);
    //先把没实现功能的按钮隐藏
    this->ui->pauseAllBtn->setVisible(false);
    this->ui->startAllBtn->setVisible(false);


    //无标题栏 无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    //自动根据当前系统语言翻译 现在只支持 中文 英文 两种
    autoTranslate();

    //默认样式
    this->setStyle("default");
    //托盘
    this->initSystemTray();

    //设置图标
    QIcon appIcon = QIcon(":/fd/images/logo128.jpg");
    qApp->setWindowIcon(appIcon);
    setWindowIcon(appIcon);
    setIconSize(QSize(128, 128));

    DataCenter::GetInstance();
    DownloadManager::GetInstance();

    startUrlWatcher();

}

MainWindow::~MainWindow()
{
    mSysTrayIcon->deleteLater( );
    delete ui;
}

void MainWindow::autoTranslate()
{
    //多语言支持
    QLocale locale;
    QString translatorFileName = "";

    //获取当前系统默认语言
    auto lang = locale.language();
    //根据语言 选择不同翻译文件 默认中文不用翻译
    switch(lang)
    {
        case QLocale::Chinese:
            // 默认中文 不用翻译
            break;
        default:
            //其他语言 都用英文
            translatorFileName = ":/fd/FastDownloader_en_US.qm";
            break;
    }

    //  翻译 有翻译文件名 就翻译 默认是中文 翻译文件名为空 不用翻译
    if(translatorFileName.length() > 0) {
        qtTranslator.load(translatorFileName);
        qApp->installTranslator(&qtTranslator);
        ui->retranslateUi(this);
    }
}

void MainWindow::addTask(QString url, QString downloadDir)
{
    qDebug("下载任务: %s", url.toStdString().c_str());
    qDebug("保存路径: %s", downloadDir.toStdString().c_str());
    DownloadTask *task = DownloadManager::GetInstance()->downloadFile(url, downloadDir);

    QListWidgetItem *qItem = new QListWidgetItem(this->ui->downloadList);
    qItem->setBackground(QColor(222, 237, 252));
    qItem->setSizeHint(QSize(348, 60));

    DownloadItemUi *listItem = new DownloadItemUi(this->ui->downloadList);
    listItem->setStyleSheet("margin-bottom: 5px;");
    listItem->show();
    listItem->bindDownloadTask(task);
    task->bindUi(listItem);
    this->ui->downloadList->setItemWidget(qItem, listItem);


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
        DataCenter::GetInstance()->setCancelUrl(url);
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
    //如果没有创建NewTaskDialog 就创建，如果创建过了 就直接用
    if(mNewTaskDialog == Q_NULLPTR ) {
        mNewTaskDialog = new NewTaskDialog(this);
        mNewTaskDialog->setModal(false);
    }
    mNewTaskDialog->reset();
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
//            mSysTrayIcon->showMessage(QObject::trUtf8("Message Title"), QObject::trUtf8("欢迎使用此程序"), QSystemTrayIcon::Information, 1000);

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
    this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
}

void MainWindow::on_ExitAppAction()
{
    qApp->quit();
}

void MainWindow::on_lockTopCbox_stateChanged(int arg1)
{
    //根据用户选择 是否固定到屏幕最前面
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
    //初始化托盘
    mSysTrayIcon = new QSystemTrayIcon(this);
    QIcon icon = QIcon(":/fd/images/logo128.jpg");
    mSysTrayIcon->setIcon(icon);
    mSysTrayIcon->setToolTip(tr("快速下载器"));
    //给QSystemTrayIcon添加槽函数

    connect(mSysTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activitedSystemTrayIcon(QSystemTrayIcon::ActivationReason)));

    // 托盘菜单
    mTrayMenu = new QMenu(this);
    mShowWindowAction = new QAction(mTrayMenu);
    mExitAppAction = new QAction(mTrayMenu);

    mShowWindowAction->setText(tr("打开"));
    mExitAppAction->setText(tr("退出"));

    mTrayMenu->addAction(mShowWindowAction);
    mTrayMenu->addAction(mExitAppAction);

    connect(mShowWindowAction, SIGNAL(triggered()), this, SLOT(on_showMainWindowAction()));
    connect(mExitAppAction, SIGNAL(triggered()), this, SLOT(on_ExitAppAction()));

    mSysTrayIcon->setContextMenu(mTrayMenu);

    //在系统托盘显示此对象
    mSysTrayIcon->show();
}

void MainWindow::on_helpBtn_clicked()
{
    //如果没有创建NewTaskDialog 就创建，如果创建过了 就直接用
    HelpDialog *helpDialog = new HelpDialog(this);
    helpDialog->setModal(true);
    helpDialog->show();
}
