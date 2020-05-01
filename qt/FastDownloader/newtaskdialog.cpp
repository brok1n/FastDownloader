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
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    // if main window fixed on top
//    int hasTopHint = parent->windowFlags() & Qt::WindowStaysOnTopHint;
//    if(hasTopHint > 1) {
//        qDebug("main window fixed on top");
//        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
//    }

    //default download path
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    ui->newTaskSaveDirEdit->setText(defaultPath);

    // url edit auto request focus
    ui->newTaskAddrEdit->setFocus();

    // 如果剪切板里有url，用户可能希望在新建下载任务窗口弹出的时候 自动吧剪切板里的url地址填入下载地址输入框中
//    QClipboard *clipboard = QApplication::clipboard();
//    if(clipboard->mimeData()->hasText()) {
//        QString text = clipboard->text();
//        if(text.contains("://") > 0) {
//            ui->newTaskAddrEdit->setText(clipboard->text());
//        }
//    }

    //为这个窗口安装过滤器
    QWidget::installEventFilter(this);


}

NewTaskDialog::~NewTaskDialog()
{
    delete ui;
}

void NewTaskDialog::mousePressEvent(QMouseEvent *event)
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

bool NewTaskDialog::eventFilter(QObject *watched, QEvent *event)
{
    if( watched == this )
    {
       //窗口激活
       if(QEvent::WindowActivate == event->type() && this->ui->newTaskAddrEdit->text().trimmed().size() == 0)
       {
           // 如果剪切板里有url，用户可能希望在新建下载任务窗口弹出的时候 自动吧剪切板里的url地址填入下载地址输入框中
           QClipboard *clipboard = QApplication::clipboard();
           if(clipboard->mimeData()->hasText()) {
               QString text = clipboard->text();
               if(text.contains("://") > 0) {
                   ui->newTaskAddrEdit->setText(clipboard->text());
               }
           }
           return true ;
       }
    }
    return false ;
}

void NewTaskDialog::reset()
{
    this->ui->newTaskAddrEdit->setText("");
    //    this->ui->newTaskSaveDirEdit->setText("");
}

void NewTaskDialog::refresh()
{
    QClipboard *clipboard = QApplication::clipboard();
    if(clipboard->mimeData()->hasText()) {
        QString text = clipboard->text();
        if(text.contains("://") > 0) {
            ui->newTaskAddrEdit->setText(clipboard->text());
        }
    }
}

void NewTaskDialog::mouseReleaseEvent(QMouseEvent *event)
{
    mMoveWindow = false;
}


void NewTaskDialog::on_newTaskCloseBtn_clicked()
{
    ((MainWindow*)parent())->cancelTask( ui->newTaskAddrEdit->text() );
    this->close();
}

void NewTaskDialog::on_newTaskDownloadBtn_clicked()
{
    QString url = ui->newTaskAddrEdit->text();
    QString downloadDir = ui->newTaskSaveDirEdit->text();

    ((MainWindow*)parent())->addTask(url, downloadDir);

    this->close();

}
