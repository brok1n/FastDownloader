#ifndef NEWTASKDIALOG_H
#define NEWTASKDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <mainwindow.h>

namespace Ui {
class NewTaskDialog;
}

class NewTaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewTaskDialog(QWidget *parent = nullptr);
    ~NewTaskDialog();
    //重置界面内容
    void reset();
    //刷新
    void refresh();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

    //事件过滤器 用来监听本窗口被激活 当窗口被激活的时候。看一下用户是否复制了一个下载地址，
    //如果是,并且本窗口下载地址是空的，就吧这个下载地址自动填到本窗口的下载地址里
    bool eventFilter(QObject *o, QEvent *e);

private slots:
    // 当 取消按钮被点击
    void on_newTaskCloseBtn_clicked();
    // 当 下载按钮被点击
    void on_newTaskDownloadBtn_clicked();
    // 选择下载文件夹按钮被点击
    void on_newTaskSelectDirBtn_clicked();

private:
    Ui::NewTaskDialog *ui;
    // move window
    bool mMoveWindow;
    //press x,y
    int mPressX, mPressY;
};

#endif // NEWTASKDIALOG_H
