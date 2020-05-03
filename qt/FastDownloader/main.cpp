#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QIODevice>
int main(int argc, char *argv[])
{

//    long long len = (long long)1024 * 1024 * 1024 * 2;  // 2Gb

//    QFile f("b.txt");
//    f.resize(len);
//    f.open(QIODevice::ReadWrite);
//    f.seek(len-1);
//    f.write("a");
//    f.flush();
//    f.close();

//    QFile file("b.txt");
//    file.open(QIODevice::ReadWrite);

//    uchar *mptr = file.map(0, len);

//    mptr[len-1] = 0;  // this line  error  SIGSEGV

//    file.flush();
//    file.close();


    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
