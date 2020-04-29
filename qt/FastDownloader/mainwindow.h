#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void retranslateUi(QMainWindow *MainWindow);
    void init(QApplication* app);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);


private slots:
    void on_closeBtn_clicked();
    void on_minBtn_clicked();
    void on_newTaskBtn_clicked();
    void on_activitedSystemTrayIcon(QSystemTrayIcon::ActivationReason reason);
    void on_showMainWindowAction();
    void on_ExitAppAction();
    void on_lockTopCbox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    // move window
    bool mMoveWindow;
    //press x,y
    int mPressX, mPressY;
    // 托盘
    QSystemTrayIcon* mSysTrayIcon;
    // 托盘菜单
    QMenu * mTrayMenu;
    // 显示主界面菜单项
    QAction* mShowWindowAction;
    // 退出程序菜单项
    QAction* mExitAppAction;
    //对QApplication引用
    QApplication* mApp;

};
#endif // MAINWINDOW_H
