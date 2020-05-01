#include "downloadtask.h"

DownloadTask::DownloadTask(QNetworkAccessManager *manager, DownloadManager *parent)
    : mReply(Q_NULLPTR)
    , mThread1(new QThread())
    , mThread2(new QThread())
    , mThread3(new QThread())
    , mThread4(new QThread())
    , mThread5(new QThread())

    , mWorker1(new DownloadWorker())
    , mWorker2(new DownloadWorker())
    , mWorker3(new DownloadWorker())
    , mWorker4(new DownloadWorker())
    , mWorker5(new DownloadWorker())
{
    this->mManager = manager;
    this->mDownloadManager = parent;

    mWorker1->moveToThread(mThread1);
    mWorker2->moveToThread(mThread2);
    mWorker3->moveToThread(mThread3);
    mWorker4->moveToThread(mThread4);
    mWorker5->moveToThread(mThread5);

    mThread1->start();
    mThread2->start();
    mThread3->start();
    mThread4->start();
    mThread5->start();

    connect(this, &DownloadTask::startDownload, mWorker1, &DownloadWorker::start);
    connect(this, &DownloadTask::startDownload, mWorker2, &DownloadWorker::start);
    connect(this, &DownloadTask::startDownload, mWorker3, &DownloadWorker::start);
    connect(this, &DownloadTask::startDownload, mWorker4, &DownloadWorker::start);
    connect(this, &DownloadTask::startDownload, mWorker5, &DownloadWorker::start);
    emit startDownload();

    this->mDownloading = false;
    this->mStop = false;
    this->mPause = false;
    this->mRemove = false;
}

void DownloadTask::init(QString url, QString path)
{
    this->mUrl = url;
    this->mLocalPath = path;

    this->mDownloading = false;
    this->mStop = false;
    this->mPause = false;
    this->mRemove = false;

    this->mPupdateProgress = Q_NULLPTR;
    this->mPfinished = Q_NULLPTR;
    this->mPfailed = Q_NULLPTR;
}

void DownloadTask::setCallBack(void (*updateProgress)(qint64, qint64), void (*finished)(), void (*failed)(QNetworkReply::NetworkError))
{
    this->mPupdateProgress = updateProgress;
    this->mPfinished = finished;
    this->mPfailed = failed;
}


void DownloadTask::start()
{
    // 先用断点续传的方式请求5个字节 探测文件基本信息
    QNetworkRequest requests;
    requests.setUrl(this->mUrl);
    requests.setRawHeader("Range", "bytes=0-4");
    mReply = mManager->get(requests);

    connect(mManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(probFinished(QNetworkReply*)));
}

void DownloadTask::pause()
{

}

void DownloadTask::stop()
{

}

void DownloadTask::remove()
{

}

void DownloadTask::probFinished(QNetworkReply *reply)
{

    QList<QByteArray> headers = reply->rawHeaderList();
    for(int i = 0; i < headers.size(); i ++) {
        const char *key = headers[i].data();
        const char *val = reply->rawHeader(key);
        qDebug("%s: %s", key, val);
    }

    QString acceptRanges = reply->rawHeader("Accept-Ranges").trimmed();
    QString contentLength = reply->rawHeader("Content-Length").trimmed();
    QString contentRange = reply->rawHeader("Content-Range").trimmed();

    mFileSize = contentLength.toInt();

    QString contentDescription = reply->rawHeader("Content-Disposition").trimmed();
    QString filename = contentDescription.mid(QString("filename=").size());
    qDebug("文件名:%s", filename.toStdString().c_str());

//    auto qurl = new QUrl(mUrl);

    if(filename.isEmpty()) {
        auto qurl = new QUrl(mUrl);
        filename = qurl->fileName();
    }

    QString downloadFullPath = mLocalPath + QDir::separator() + filename;
    QString downloadFullPathTmp = downloadFullPath + ".tmp";

    QFile file(downloadFullPathTmp);
    file.resize(mFileSize);
    file.open(QIODevice::ReadWrite);

    uchar* ptr = file.map(0, mFileSize);

    qDebug("本地文件路径:%s", downloadFullPath.toStdString().c_str());
    qDebug("本地文件临时路径:%s", downloadFullPathTmp.toStdString().c_str());

    QStringList strList = contentRange.split("/");
    if(strList.size() == 2) {
        qDebug("支持断点续传！");
        mFileSize = strList.at(1).toInt();
        //支持断点续传，就开5个线程同时下载

        mWorker1->download(mUrl, ptr, 0, mFileSize / 5);

    } else {
        qDebug("不支持断点续传!");

        //不支持断点续传，就只能单个线程直接下载
        mWorker1->download(mUrl, ptr, 0, mFileSize);

    }

    qDebug("文件大小:%ld", mFileSize);

//    this->mDownloadManager->finished(this);
//    ((DownloadManager*)parent())->finished(this);

    disconnect(mManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(probFinished(QNetworkReply*)));

    reply->abort();
    reply->deleteLater();
    reply = Q_NULLPTR;
}
