#include "downloadworker.h"

DownloadWorker::~DownloadWorker()
{

}

DownloadWorker::DownloadWorker(int id)
    : mId(id)
    , mReply(Q_NULLPTR)
    , mDownloadIndex(0)
    , mUrl("")
    , mDownloadFile(Q_NULLPTR)
    , mMptr(Q_NULLPTR)
    , mStart(0)
    , mEnd(0)
    , mDownloadFinished(true)
    , mWaitForDownload(false)
    , mEventLoop(Q_NULLPTR)

{

}

void DownloadWorker::download(QString url, QFile *downloadFile, qint64 start, qint64 end)
{
    if( !mDownloadFinished ) {
        return;
    }
    mDownloadFinished = false;
    this->mUrl = url;
    this->mDownloadFile = downloadFile;
    this->mStart = start;
    this->mDownloadIndex = mStart;
    this->mEnd = end;
    mWaitForDownload = true;
    qDebug(QString("%d 下载Worker:%1-%2 %3 即将开始下载").arg(mId).arg(mStart).arg(mEnd).arg(mUrl).toUtf8());
}

void DownloadWorker::download(QString url, uchar *mptr, qint64 start, qint64 end)
{
    if( !mDownloadFinished ) {
        return;
    }
    mDownloadFinished = false;
    this->mUrl = url;
    this->mMptr = mptr;
    this->mStart = start;
    this->mDownloadIndex = mStart;
    this->mEnd = end;
    mWaitForDownload = true;
    qDebug(QString("%1 下载Worker:%2-%3 %4 即将开始下载").arg(mId).arg(mStart).arg(mEnd).arg(mUrl).toUtf8());
}

void DownloadWorker::start()
{
    this->mEventLoop = new QEventLoop();
    this->mManager = new QNetworkAccessManager();
    while (true) {
        if( mWaitForDownload && !mUrl.isEmpty() && mEnd > 0) {
            mWaitForDownload = false;
            QNetworkRequest requests;
            requests.setUrl(this->mUrl);
            requests.setRawHeader("Range", QString("bytes=%1-%2").arg(mStart).arg(mEnd).toUtf8());
            qDebug(requests.rawHeader("Range").toStdString().c_str());
            mReply = mManager->get(requests);
            qDebug(QString("%1 下载Worker:%2-%3 %4 开始下载...").arg(mId).arg(mStart).arg(mEnd).arg(mUrl).toUtf8());

//            connect(mManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(mfinished(QNetworkReply*)), Qt::DirectConnection);

            connect(mReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));

            connect(mReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

            connect(mReply, SIGNAL(finished()), this, SLOT(rfinished()));

            connect(mReply, SIGNAL(readyRead()), this, SLOT(readyRead()));

            mEventLoop->exec(QEventLoop::ExcludeUserInputEvents);
        }
        QThread::sleep(1);
    }

}

void DownloadWorker::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
//    qDebug("%d total:%lld, read:%d  percent:%f", mId, bytesTotal, bytesReceived, (double)bytesReceived / (double)bytesTotal);
    emit this->updateProgress(mId, bytesReceived, bytesTotal);
}

void DownloadWorker::error(QNetworkReply::NetworkError err)
{
    QString strError = mReply->errorString();
     qDebug() << "__________" + strError;
     emit this->error(mId, mReply->error(), "下载失败！");
}

void DownloadWorker::rfinished()
{
    while(mDownloadIndex < mEnd) {
        qDebug("%d  index:%lld  end:%lld", mId, mDownloadIndex, mEnd);
        QThread::sleep(2);
    }
    disconnect(mReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));

    disconnect(mReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

    disconnect(mReply, SIGNAL(finished()), this, SLOT(rfinished()));

    disconnect(mReply, SIGNAL(readyRead()), this, SLOT(readyRead()));

    mEventLoop->quit();
    mDownloadFinished = true;

    emit this->workerFinished(mId);

}


void DownloadWorker::readyRead()
{
    auto data = mReply->readAll();
//    qDebug("--------------readyRead read len:%d", data.size());
    for(int i = 0;i < data.size(); i++) {
        mMptr[mDownloadIndex++] = data.at(i);
    }
    // ----------------1------------------
}
