#include "common.h"
#include "downloadworker.h"

DownloadWorker::~DownloadWorker()
{

}

DownloadWorker::DownloadWorker(int id)
    : mId(id)
    , mReply(Q_NULLPTR)
    , mDownloadIndex(0)
    , mMultiple(true)
    , mBuffer(new QByteArray())
    , mUrl("")
    , mDownloadFile(Q_NULLPTR)
    , mStart(0)
    , mEnd(0)
    , mDownloadFinished(true)
    , mWaitForDownload(false)
    , mEventLoop(Q_NULLPTR)
    , mLastTime(0)
    , mRecvDataLen(0)
    , mSpeed(0)
{

}

void DownloadWorker::download(QString url, QFile *downloadFile, qint64 start, qint64 end, bool multiple)
{
    qDebug("download file");
    if( !mDownloadFinished ) {
        return;
    }
    if(downloadFile) {
        qDebug() << "download file 文件对象正常!";
    }
    mBuffer->clear();
    mDownloadFinished = false;
    this->mUrl = url;
    this->mDownloadFile = downloadFile;
    this->mStart = start;
    this->mDownloadIndex = start;
    this->mEnd = end;
    mWaitForDownload = true;
    qDebug(QString("%1 下载Worker:%2-%3 %4 即将开始下载").arg(mId).arg(mStart).arg(mEnd).arg(mUrl).toUtf8());
}

int DownloadWorker::id()
{
    return this->mId;
}

qint64 DownloadWorker::getSpeed()
{
    return mSpeed;
}

void DownloadWorker::start()
{
    this->mEventLoop = new QEventLoop();
    this->mManager = new QNetworkAccessManager();
    while (true) {
        if( mWaitForDownload && !mUrl.isEmpty() && mEnd > 0) {
            mWaitForDownload = false;
            QNetworkRequest requests;

            //解决SSL证书错误问题
            QSslConfiguration config = QSslConfiguration::defaultConfiguration();
            config.setProtocol(QSsl::TlsV1_2);
            config.setPeerVerifyMode(QSslSocket::VerifyNone);
            requests.setSslConfiguration(config);

            requests.setUrl(this->mUrl);
            requests.setRawHeader("Range", QString("bytes=%1-%2").arg(mStart).arg(mEnd).toUtf8());
            qDebug(requests.rawHeader("Range").toStdString().c_str());
            mReply = mManager->get(requests);
            qDebug(QString("%1 下载Worker:%2-%3 %4 开始下载...").arg(mId).arg(mStart).arg(mEnd).arg(mUrl).toUtf8());

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
    QThread::msleep(200);
    uint tmpLen = mBuffer->size();
    uchar *fmp = mDownloadFile->map(mDownloadIndex, tmpLen);
    if(fmp) {
       memcpy(fmp, mBuffer->data(), tmpLen);
       mDownloadFile->unmap(fmp);
    } else {
        qDebug("多线程下载, mmap错误!!!!!!!!!!!!!!");
    }
    mDownloadIndex += tmpLen;
    mBuffer->clear();
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

    mSpeed = 0;

    emit this->workerFinished(mId);
}


void DownloadWorker::readyRead()
{
    if(mLastTime == 0)
    {
        mLastTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }

    auto data = mReply->readAll();
    uint tmpLen = data.size();

    mRecvDataLen += tmpLen;

    qint64 offset = QDateTime::currentDateTime().toMSecsSinceEpoch() - mLastTime;
    if(offset >= 1000)
    {
        mSpeed = mRecvDataLen / (float)(offset / 1000);
        qDebug() << "speed:" << Common::lenToTxt(mSpeed) << "b/s";
        mLastTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        mRecvDataLen = 0;
    }

    if(mMultiple) {
        mBuffer->append(data);
        tmpLen = mBuffer->size();
        if(tmpLen > BUFFER_SIZE) {
            uchar *fmp = mDownloadFile->map(mDownloadIndex, tmpLen);
            if(fmp) {
               memcpy(fmp, mBuffer->data(), tmpLen);
               mDownloadFile->unmap(fmp);
            } else {
                qDebug("多线程下载, mmap错误!!!!!!!!!!!!!!");
            }
            mDownloadIndex += tmpLen;
            mBuffer->clear();
        }

    } else {
        //单线程下载，可以不用mmap 直接seek到指定下载位置写入数据
        mDownloadFile->seek(mDownloadIndex);
        mDownloadFile->write(data);
        mDownloadFile->flush();
        mDownloadIndex += tmpLen;
    }
}
