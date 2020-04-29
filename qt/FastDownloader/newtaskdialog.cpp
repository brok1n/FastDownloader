#include "newtaskdialog.h"
#include "ui_newtaskdialog.h"

NewTaskDialog::NewTaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTaskDialog)
{
    ui->setupUi(this);
    //不显示标题栏，亦无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
}

NewTaskDialog::~NewTaskDialog()
{
    delete ui;
}

void NewTaskDialog::mousePressEvent(QMouseEvent* event)
{
    //在标题栏上才可以移动界面
    if(event->y() < 45) {
        mMoveWindow = true;
        mPressX = event->x();
        mPressY = event->y();
    }
}

void NewTaskDialog::mouseMoveEvent(QMouseEvent *event)
{
    if(mMoveWindow) {
        int offsetX = event->x() - mPressX;
        int offsetY = event->y() - mPressY;
        this->move(this->x() + offsetX, this->y() + offsetY);
    }
}

void NewTaskDialog::mouseReleaseEvent(QMouseEvent *event)
{
    mMoveWindow = false;
}


void NewTaskDialog::on_newTaskCloseBtn_clicked()
{
    this->close();
}
