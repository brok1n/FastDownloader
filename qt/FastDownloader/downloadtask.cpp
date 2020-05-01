#include "downloadtask.h"

DownloadTask::DownloadTask(QNetworkAccessManager *manager, DownloadManager *parent)
{
    this->mManager = manager;
    this->mDownloadManager = parent;

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
    connect(mManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(probFinished(QNetworkReply*)));

    QNetworkRequest requests;
    requests.setUrl(this->mUrl);
    requests.setRawHeader("Range", "bytes=0-4");
    mManager->get(requests);
//    QNetworkReply *reply = mManager->get(requests);
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
//    qDebug("finished");
//    QList<QByteArray> headers = reply->rawHeaderList();
//    for(int i = 0; i < headers.size(); i ++) {
//        QString key = headers.at(i);
//        QString value = reply->rawHeader(key.toStdString().c_str());
//        qDebug("%s: %s", key.toStdString().c_str(), value.toStdString().c_str());
//    }

    QString acceptRanges = reply->rawHeader("Accept-Ranges").trimmed();
    QString contentLength = reply->rawHeader("Content-Length").trimmed();
    QString contentRange = reply->rawHeader("Content-Range").trimmed();

    QStringList strList = contentRange.split("/");
    if(strList.size() == 2) {
        qDebug("支持断点续传！");
        mFileSize = strList.at(1).toInt();
    } else {
        qDebug("不支持断点续传!");
        mFileSize = contentLength.toInt();
    }


//    if( acceptRanges == "bytes") {


//    } else {
//        qDebug("不支持断点续传");
//        mFileSize = contentLength.toInt();
//    }

    qDebug("文件大小:%d", mFileSize);

    this->mDownloadManager->finished(this);
//    ((DownloadManager*)parent())->finished(this);

    reply->deleteLater();
    reply = Q_NULLPTR;
}
