#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QIODevice>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.init(&a);
    w.show();

    QFile file("a.txt");
    file.resize(1024*1024*50);
    file.open(QIODevice::ReadWrite);

    uchar *mptr = file.map(0, 1024*1024*30);
    for(int i = 0; i < 40970; i ++) {
        *mptr++ = 8;
    }
    file.unmap(mptr);
    file.flush();
    file.close();

    return a.exec();
}
