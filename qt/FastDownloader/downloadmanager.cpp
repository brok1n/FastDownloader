#include "downloadmanager.h"

DownloadManager* DownloadManager::mInstance = nullptr;
QMutex  DownloadManager::mMutex;

DownloadManager::~DownloadManager()
{
//    mFreeTaskList->clear();
//    mFreeTaskList = NULL;
    mTaskList->clear();
    mTaskList = NULL;
    this->mNetAccessManager->deleteLater();
    this->mNetAccessManager = Q_NULLPTR;
}

DownloadManager *DownloadManager::GetInstance(QObject *parent)
{
    if (mInstance == nullptr)
    {
        QMutexLocker locker(&mMutex);
        if (mInstance == nullptr)
        {
            mInstance = new DownloadManager(parent);
        }
    }
    return mInstance;
}

DownloadManager::DownloadManager(QObject *parent) : QObject(parent)
{
    //QNetworkAccessManager
    this->mNetAccessManager = new QNetworkAccessManager(this);
    mTaskList = new QList<DownloadTask*>();
//    mFreeTaskList = new QList<DownloadTask*>();
}

void DownloadManager::downloadFile(QString url, QString downloadDir)
{
    DownloadTask *task = Q_NULLPTR;
    for( int i = 0; i < mTaskList->size(); i ++) {
        if(mTaskList->at(i)->isFree()) {
            task = mTaskList->at(i);
            continue;
        }
    }
    if(task == Q_NULLPTR) {
        task = new DownloadTask(this->mNetAccessManager, this);
        mTaskList->append(task);
    }
//    if(mFreeTaskList->isEmpty()) {
//        task = new DownloadTask(this->mNetAccessManager, this);
//    } else {
//        task = mFreeTaskList->takeFirst();
//    }

    task->init(url, downloadDir);
    task->start();
}

void DownloadManager::finished(DownloadTask *task)
{
    qDebug("download manager finished!!!!!!");
    qDebug("download task list size:%d", mTaskList->size());
//    mTaskList->removeAll(task);
//    mFreeTaskList->append(task);

//    mTaskList->removeOne(task);
    qDebug("download task list size:%d", mTaskList->size());
//    mFreeTaskList.append(task);
}

qint64 DownloadManager::getFileSize(QString url)
{
//    QNetworkReply *reply = this->mNetAccessManager->get(QNetworkRequest(url));

}
