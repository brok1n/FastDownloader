#include "downloadtask.h"

DownloadTask::DownloadTask(QNetworkAccessManager *manager, DownloadManager *parent)
    : mReply(Q_NULLPTR)
    , mThread1(new QThread())
    , mThread2(new QThread())
    , mThread3(new QThread())
    , mThread4(new QThread())
    , mThread5(new QThread())

    , mWorker1(new DownloadWorker(1))
    , mWorker2(new DownloadWorker(2))
    , mWorker3(new DownloadWorker(3))
    , mWorker4(new DownloadWorker(4))
    , mWorker5(new DownloadWorker(5))
    , mDownloadFile(Q_NULLPTR)
    , mDownloadFileMapPtr(Q_NULLPTR)
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

    QString filename = "";
    if(contentDescription.size() > 10 && contentDescription.indexOf("=") > 0) {
        contentDescription.replace("\"", "");
        int idx = contentDescription.indexOf("=");
        if (idx >= 0 && idx < contentDescription.length()) {
            filename = contentDescription.mid(idx+1);
        }
    }
    qDebug("文件名:%s", filename.toStdString().c_str());

    if(filename.isEmpty()) {
        auto qurl = new QUrl(mUrl);
        filename = qurl->fileName();
    }

    QString downloadFullPath = mLocalPath + QDir::separator() + filename;
    QString downloadFullPathTmp = downloadFullPath + ".tmp";

    qDebug("本地文件路径:%s", downloadFullPath.toStdString().c_str());
    qDebug("本地文件临时路径:%s", downloadFullPathTmp.toStdString().c_str());

    QStringList strList = contentRange.split("/");
    if(strList.size() == 2) {
        qDebug("支持断点续传！");
        mFileSize = strList.at(1).toInt();

        mDownloadFile = new QFile(downloadFullPathTmp);
        mDownloadFile->resize(mFileSize);
        mDownloadFile->open(QIODevice::ReadWrite);

        mDownloadFileMapPtr = mDownloadFile->map(0, mFileSize);

        //支持断点续传，就开5个线程同时下载
        qDebug("支持断点续传！");
//        mWorker1->download(mUrl, mDownloadFile, 0, mFileSize / 5);
        mWorker1->download(mUrl, mDownloadFileMapPtr, 0, mFileSize / 5);

        connect(mWorker1, SIGNAL(workerFinished(int)), this, SLOT(workerFinished(int)));

    } else {
        qDebug("不支持断点续传! 文件大小:%d", mFileSize);

        mDownloadFile = new QFile(downloadFullPathTmp);
        mDownloadFile->resize(mFileSize);
        mDownloadFile->open(QIODevice::ReadWrite);

        mDownloadFileMapPtr = mDownloadFile->map(0, mFileSize);

        //不支持断点续传，就只能单个线程直接下载
//        mWorker1->download(mUrl, mDownloadFile, 0, mFileSize);
        mWorker1->download(mUrl, mDownloadFileMapPtr, 0, mFileSize);

        connect(mWorker1, SIGNAL(workerFinished(int)), this, SLOT(workerFinished(int)));

    }

    qDebug("文件大小:%ld", mFileSize);

//    this->mDownloadManager->finished(this);
//    ((DownloadManager*)parent())->finished(this);

    disconnect(mManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(probFinished(QNetworkReply*)));

    reply->abort();
    reply->deleteLater();
    reply = Q_NULLPTR;
}

void DownloadTask::workerFinished(int id)
{
    qDebug("download task finished: %d", id);

    mDownloadFile->close();

}

void DownloadTask::updateProgress(int id, qint64 recived, qint64 total)
{
    qDebug("%d:%f", (int)((double)recived / total * 1000) / 1000.0);
}

void DownloadTask::error(int id, int code, QString msg)
{
    qDebug("%d 下载异常 %d:%s", id, code, msg.toStdString().c_str());
}
