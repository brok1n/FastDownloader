#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <downloadmanager.h>
#include <newtaskdialog.h>
#include <QThread>
#include <urlwatcher.h>
#include <QListWidget>
#include <QTranslator>

class NewTaskDialog;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //自动语言翻译
    void autoTranslate();
    //添加一个下载任务
    void addTask(QString url, QString downloadDir);

    //开始URL监听
    void startUrlWatcher();

    //用户取消了一个下载任务 就把这个下载任务标记为被取消的 之后再检测到这个这个任务就不弹出下载框了
    void cancelTask(QString url);

signals:
    //开启URL监听信号
    void startUrlWatcherSignal();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);


private slots:
    //关闭按钮被点击
    void on_closeBtn_clicked();
    //最小化按钮被点击
    void on_minBtn_clicked();

    //新建下载任务按钮被点击
    void on_newTaskBtn_clicked();
    //托盘图标被激活
    void on_activitedSystemTrayIcon(QSystemTrayIcon::ActivationReason reason);

    //显示主窗口托盘动作
    void on_showMainWindowAction();
    //退出应用托盘动作
    void on_ExitAppAction();

    //是否置顶复选框被触发
    void on_lockTopCbox_stateChanged(int arg1);

    // 当 url监控线程发现了有效的url 触发
    void onWatchUrl();
    //修改样式
    void setStyle(QString styleName);
    //初始化托盘
    void initSystemTray();

    void on_helpBtn_clicked();

private:
    Ui::MainWindow *ui;
    QTranslator qtTranslator;
    // move window
    bool mMoveWindow;
    //press x,y
    int mPressX, mPressY;
    // 托盘
    QSystemTrayIcon *mSysTrayIcon;
    // 托盘菜单
    QMenu *mTrayMenu;
    // 显示主界面菜单项
    QAction *mShowWindowAction;
    // 退出程序菜单项
    QAction *mExitAppAction;
    //新建下载弹框
    NewTaskDialog *mNewTaskDialog;
    //剪切板URL监控线程
    QThread *mUrlWatcherThread;
    //剪切板URL监控worker
    UrlWatcher *mUrlWatcher;

};
#endif // MAINWINDOW_H
