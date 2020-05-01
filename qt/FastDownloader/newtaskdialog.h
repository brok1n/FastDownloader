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
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    //事件过滤器 用来监听本窗口被激活 当窗口被激活的时候。看一下用户是否复制了一个下载地址，
    //如果是,并且本窗口下载地址是空的，就吧这个下载地址自动填到本窗口的下载地址里
    bool eventFilter(QObject *o, QEvent *e);

private slots:
    void on_newTaskCloseBtn_clicked();

    void on_newTaskDownloadBtn_clicked();

private:
    Ui::NewTaskDialog *ui;
    // move window
    bool mMoveWindow;
    //press x,y
    int mPressX, mPressY;
};

#endif // NEWTASKDIALOG_H
