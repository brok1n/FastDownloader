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

//    int mapLen = 1024;

//    uchar *mptr = file.map(len-mapLen, mapLen);

//    for(int i = 0; i < mapLen; i ++) {
//        mptr[i] = i;  // this line  error  SIGSEGV
//    }

//    uchar *mptr2 = file.map(len-mapLen * 2, mapLen);

//    for(int i = 0; i < mapLen; i ++) {
//        mptr2[i] = i;  // this line  error  SIGSEGV
//    }


//    uchar *mptr3 = file.map(len-mapLen * 3, mapLen);

//    for(int i = 0; i < mapLen; i ++) {
//        mptr3[i] = i;  // this line  error  SIGSEGV
//    }

//    file.unmap(mptr);
//    file.unmap(mptr2);
//    file.unmap(mptr3);

//    file.flush();
//    file.close();


    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
