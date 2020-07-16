#include "mainwindow.h"

#include <QApplication>
#include <QSharedMemory>
#include "datacenter.h"
#include "config.h"

int main(int argc, char *argv[])
{
    QSharedMemory *sharedMemory = new QSharedMemory(SHARED_MEM_KEY);
    sharedMemory->attach();
    if( sharedMemory->size() > 0)
    {
        qDebug("存在相同的共享内存");
        sharedMemory->detach();
        return 0;
    }
    sharedMemory->create(SHARED_MEM_LEN);
    DataCenter::GetInstance()->setSharedMemory(sharedMemory);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    int status = a.exec();
    sharedMemory->detach();
    return status;
}
