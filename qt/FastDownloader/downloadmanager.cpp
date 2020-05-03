#include "downloadmanager.h"
#include "downloadtask.h"

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

    this->deleteLater();
}

DownloadManager *DownloadManager::GetInstance()
{
    if (mInstance == nullptr)
    {
        QMutexLocker locker(&mMutex);
        if (mInstance == nullptr)
        {
            mInstance = new DownloadManager();
        }
    }
    return mInstance;
}

DownloadManager::DownloadManager(QObject *parent) : QObject(parent)
{
    //QNetworkAccessManager
    this->mNetAccessManager = new QNetworkAccessManager(this);
    mTaskList = new QList<DownloadTask*>();
    this->mDownloadPath =  QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
}

DownloadTask* DownloadManager::downloadFile(QString url, QString downloadDir, bool multiple)
{
    // save last download path
    this->mDownloadPath = downloadDir;

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
    task->init(url, downloadDir, multiple);
    task->start();
    return task;
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

QString DownloadManager::getDownloadPath()
{
    return this->mDownloadPath;
}

qint64 DownloadManager::getFileSize(QString url)
{
//    QNetworkReply *reply = this->mNetAccessManager->get(QNetworkRequest(url));

}
