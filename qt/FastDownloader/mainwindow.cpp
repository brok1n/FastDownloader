#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //不显示标题栏，亦无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //设置背景（全透明）
//    setAttribute(Qt::WA_TranslucentBackground);
    QFile file(":/fd/css/default.css");
    file.open(QFile::ReadOnly);
    QString style = QLatin1String(file.readAll());
    this->setStyleSheet(style);
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    //在标题栏上才可以移动界面
    if(event->y() < 45) {
        moveWindow = true;
        pressX = event->x();
        pressY = event->y();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(moveWindow) {
        int offsetX = event->x() - pressX;
        int offsetY = event->y() - pressY;
        this->move(this->x() + offsetX, this->y() + offsetY);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    moveWindow = false;
}


void MainWindow::on_closeBtn_clicked()
{
    QApplication::exit(1);
}

void MainWindow::on_minBtn_clicked()
{

}

void MainWindow::on_newTaskBtn_clicked()
{
    qDebug("新建按钮被点击");
}
