#include "downloadmanager.h"

DownloadManager* DownloadManager::mInstance = nullptr;
QMutex  DownloadManager::mMutex;

DownloadManager::~DownloadManager()
{
    mFreeTaskList->clear();
    mFreeTaskList = NULL;
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
    mFreeTaskList = new QList<DownloadTask*>();
}

void DownloadManager::downloadFile(QString url, QString downloadDir)
{
    DownloadTask *task = Q_NULLPTR;
    if(mFreeTaskList->isEmpty()) {
        task = new DownloadTask(this->mNetAccessManager, this);
    } else {
        task = mFreeTaskList->takeFirst();
    }
    mTaskList->append(task);
    task->init(url, downloadDir);
    task->start();
}

void DownloadManager::finished(DownloadTask *task)
{
    mTaskList->removeAll(task);
    mFreeTaskList->append(task);
//    mTaskList.removeOne(task);
//    mFreeTaskList.append(task);
}

qint64 DownloadManager::getFileSize(QString url)
{
//    QNetworkReply *reply = this->mNetAccessManager->get(QNetworkRequest(url));

}
