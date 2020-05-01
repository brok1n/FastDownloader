#include "downloadworker.h"

DownloadWorker::~DownloadWorker()
{

}


DownloadWorker::DownloadWorker()
    : mReply(Q_NULLPTR)
    , mUrl("")
    , mMptr(Q_NULLPTR)
    , mStart(0)
    , mEnd(0)
    , mDownloadFinished(true)
    , mWaitForDownload(false)
{

}

void DownloadWorker::download(QString url, uchar *mptr, long start, long end)
{
    if( !mDownloadFinished ) {
        return;
    }
    this->mUrl = url;
    this->mMptr = mptr;
    this->mStart = start;
    this->mEnd = end;
    mWaitForDownload = true;
    qDebug(QString("下载Worker:%1-%2 %3 即将开始下载").arg(mStart).arg(mEnd).arg(mUrl).toUtf8());
}

void DownloadWorker::start()
{
     this->mManager = new QNetworkAccessManager();
    while (true) {
        if( mWaitForDownload && !mUrl.isEmpty() && mEnd > 0) {
            mWaitForDownload = false;
            QNetworkRequest requests;
            requests.setUrl(this->mUrl);
            requests.setRawHeader("Range", QString("bytes=%1-%2").arg(mStart).arg(mEnd).toUtf8());
            qDebug(requests.rawHeader("Range").toStdString().c_str());
            mReply = mManager->get(requests);
            qDebug(QString("下载Worker:%1-%2 %3 开始下载...").arg(mStart).arg(mEnd).arg(mUrl).toUtf8());

//            connect(mReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));

//            connect(mReply, SIGNAL(encrypted()), this, SLOT(encrypted()));

//            connect(mReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

//            connect(mReply, SIGNAL(finished()), this, SLOT(rfinished()));
            connect(mManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(mfinished(QNetworkReply*)));

//            connect(mReply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
//            connect(mReply, SIGNAL(readyRead()), this, SLOT(readyRead()));

        }
        QThread::sleep(1);
    }



//    connect(mReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));

//    connect(mReply, SIGNAL(redirected(const QUrl &url)), this, SLOT(redirected(const QUrl &url)));

//    connect(mReply, SIGNAL(sslErrors(const QList<QSslError> &errors)), this, SLOT(sslErrors(const QList<QSslError> &errors)));

//    connect(mReply, SIGNAL(encrypted()), this, SLOT(encrypted()));

//    connect(mReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

//    connect(mReply, SIGNAL(finished()), this, SLOT(rfinished()));
//    connect(mManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(mfinished(QNetworkReply*)));

//    connect(mReply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
//    connect(mReply, SIGNAL(readyRead()), this, SLOT(readyRead()));


    //等待下载完毕才关闭线程
//    while( !mDownloadFinished ) {
//        qDebug("等待下载完毕");
//        QThread::sleep(1);
//    }

}

void DownloadWorker::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    qDebug("total:%lld, read:%lld  percent:%f", bytesTotal, bytesReceived, bytesTotal / bytesReceived);
}

//void DownloadWorker::redirected(const QUrl &url)
//{
//    qDebug("redirected:%s", url.url().toStdString().c_str());
//}

//void DownloadWorker::sslErrors(const QList<QSslError> &errors)
//{
//    qDebug() << "__________" + errors.at(0).errorString();
//}

void DownloadWorker::encrypted()
{
    qDebug("encrypted");
}

void DownloadWorker::error(QNetworkReply::NetworkError err)
{
    QString strError = mReply->errorString();
     qDebug() << "__________" + strError;
}

void DownloadWorker::rfinished()
{
    qDebug("rfinished no params");
}

void DownloadWorker::mfinished(QNetworkReply *reply)
{
    qDebug("finished QNetworkReply");
}

void DownloadWorker::metaDataChanged()
{
    qDebug("metaDataChanged");
}

void DownloadWorker::readyRead()
{
    qDebug("readyRead");
    mReply->readAll();
}
