#ifndef NEWTASKDIALOG_H
#define NEWTASKDIALOG_H

#include <QDialog>
#include <QMouseEvent>

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
