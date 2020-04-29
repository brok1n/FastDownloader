#include "newtaskdialog.h"
#include "ui_newtaskdialog.h"

#include <QDir>
#include <QStandardPaths>

NewTaskDialog::NewTaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTaskDialog)
{
    ui->setupUi(this);
    //no titlebar no border
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    // if main window fixed on top
    int hasTopHint = parent->windowFlags() & Qt::WindowStaysOnTopHint;
    if(hasTopHint > 1) {
        qDebug("main window fixed on top");
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    }

    //default download path
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    qDebug("download path:%s", defaultPath.toStdString().c_str());
    ui->newTaskSaveDirEdit->setText(defaultPath);

}

NewTaskDialog::~NewTaskDialog()
{
    delete ui;
}

void NewTaskDialog::mousePressEvent(QMouseEvent* event)
{
    //in titlebar
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

void NewTaskDialog::on_newTaskDownloadBtn_clicked()
{
    QString url = ui->newTaskAddrEdit->text();
    QString path = ui->newTaskSaveDirEdit->text();

    qDebug("download url:%s", url.toStdString().c_str());
    qDebug("download path:%s", path.toStdString().c_str());

}
