#include "downloadtask.h"

DownloadTask::DownloadTask(QNetworkAccessManager *manager, DownloadManager *parent)
    : QObject(parent)
    , mReply(Q_NULLPTR)
    , mDownloadThreadList(new QList<QThread*>())
    , mDownloadWorkerList(new QList<DownloadWorker*>())
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
    , mWorkerCount(5)
    , mWorkerIdSum(0)
    , mFinishedWorkerIdSum(0)
    , mDownloadFullPath("")
    , mDownloadFullPathTemp("")
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

    for( int i = 1; i <= mWorkerCount; i ++ ) {
       mWorkerIdSum += i;
       QThread *thread = new QThread();
       DownloadWorker *worker = new DownloadWorker(i);
       worker->moveToThread(thread);
       mDownloadThreadList->append(thread);
       mDownloadWorkerList->append(worker);
       thread->start();
       connect(this, &DownloadTask::startDownload, worker, &DownloadWorker::start);
    }


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
    this->mCompletected = false;
    //完成线程id和
    this->mFinishedWorkerIdSum = 0;

    //downloadFile
    this->mDownloadFullPath = "";
    this->mDownloadFullPathTemp = "";
    this->mDownloadFile = Q_NULLPTR;
    this->mDownloadFileMapPtr = Q_NULLPTR;
    // file size
    this->mFileSize = 0;
    // download max speed
    this->mMaxSpeed = 0;
    // download min speed  default is 0 ignore 0
    this->mMinSpeed = 0;
    // download speed
    this->mSpeed = 0;

    this->mPupdateProgress = Q_NULLPTR;
    this->mPfinished = Q_NULLPTR;
    this->mPfailed = Q_NULLPTR;

    if(this->mReply != Q_NULLPTR) {
        this->mReply->deleteLater();
    }
    this->mReply = Q_NULLPTR;


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

bool DownloadTask::isFree()
{
    return this->mCompletected;
}

QString getAvaliableFilePath(QString filePath, QString nPath="", int repeatCount=0) {
    QFileInfo  fileInfo(filePath);
    if(fileInfo.exists()) {
        QString path = filePath.left(filePath.lastIndexOf(QDir::separator()));
        QString name = fileInfo.fileName();
        QString suffix = fileInfo.suffix();
        name = name.replace("." + suffix, "");
        if(!nPath.isEmpty()) {
            name = name.mid(name.lastIndexOf("(")+1);
        }
        QString newPath = path + QDir::separator() + name + "(" + QString::number(repeatCount) + ")";
        if(!suffix.isEmpty()) {
            newPath += "." + suffix;
        }
        if(QFile::exists(newPath)){
            return getAvaliableFilePath(filePath, newPath, ++repeatCount);
        } else {
            return newPath;
        }

    }
    return filePath;
}

void DownloadTask::probFinished(QNetworkReply *)
{

    QList<QByteArray> headers = mReply->rawHeaderList();
    for(int i = 0; i < headers.size(); i ++) {
        const char *key = headers[i].data();
        const char *val = mReply->rawHeader(key);
        qDebug("%s: %s", key, val);
    }

    QString acceptRanges = mReply->rawHeader("Accept-Ranges").trimmed();
    QString contentLength = mReply->rawHeader("Content-Length").trimmed();
    QString contentRange = mReply->rawHeader("Content-Range").trimmed();

    mFileSize = contentLength.toInt();

    QString contentDescription = mReply->rawHeader("Content-Disposition").trimmed();

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

    mDownloadFullPath = mLocalPath + QDir::separator() + filename;
    mDownloadFullPath = getAvaliableFilePath(mDownloadFullPath);
    mDownloadFullPathTemp = mDownloadFullPath + ".tmp";

    qDebug("本地文件路径:%s", mDownloadFullPath.toStdString().c_str());
    qDebug("本地文件临时路径:%s", mDownloadFullPathTemp.toStdString().c_str());

    QStringList strList = contentRange.split("/");
    if(strList.size() == 2) {
        qDebug("支持断点续传！");
        mFileSize = strList.at(1).toDouble();
        qDebug("文件大小:%lld", mFileSize);

        mDownloadFile = new QFile(mDownloadFullPathTemp);
        mDownloadFile->resize(mFileSize);
        mDownloadFile->open(QIODevice::ReadWrite);

        mDownloadFileMapPtr = mDownloadFile->map(0, mFileSize);

        mWorkerCount = mDownloadWorkerList->size();
        qint64 start = 0;
        qint64 end = 0;
        qint64 block = mFileSize / mWorkerCount;
        qint64 blockMod = mFileSize % mWorkerCount;
        for(int i = 0; i < mWorkerCount; i ++) {
            DownloadWorker *worker = mDownloadWorkerList->at(i);
            start = i * block;
            end = start + block;
            if(i > 0) {
                start += 1;
            }
            //最后一个 要把多余的加上
            if(i == mWorkerCount - 1) {
                end += blockMod;
            }
            worker->download(mUrl, mDownloadFileMapPtr, start, end);
            connect(worker, SIGNAL(updateProgress(int, qint64, qint64)), this, SLOT(updateProgress(int, qint64, qint64)));
            connect(worker, SIGNAL(error(int, int, QString)), this, SLOT(error(int, int, QString)));
            connect(worker, SIGNAL(workerFinished(int)), this, SLOT(workerFinished(int)));
        }
    } else {
        qDebug("不支持断点续传! 文件大小:%lld", mFileSize);

        mDownloadFile = new QFile(mDownloadFullPathTemp);
        mDownloadFile->resize(mFileSize);
        mDownloadFile->open(QIODevice::ReadWrite);

        mDownloadFileMapPtr = mDownloadFile->map(0, mFileSize);

        //不支持断点续传，就只能单个线程直接下载
        DownloadWorker *worker = mDownloadWorkerList->at(0);
        worker->download(mUrl, mDownloadFileMapPtr, 0, mFileSize);
        connect(worker, SIGNAL(updateProgress(int, qint64, qint64)), this, SLOT(updateProgress(int, qint64, qint64)));
        connect(worker, SIGNAL(error(int, int, QString)), this, SLOT(error(int, int, QString)));
        connect(worker, SIGNAL(workerFinished(int)), this, SLOT(workerFinished(int)));
    }

    qDebug("文件大小:%lld", mFileSize);

    disconnect(mManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(probFinished(QNetworkReply*)));

    mReply->abort();
    mReply->deleteLater();
    mReply = Q_NULLPTR;
}

void DownloadTask::workerFinished(int id)
{
    qDebug("download task finished: %d", id);
    mFinishedWorkerIdSum += id;
    if(mFinishedWorkerIdSum >= mWorkerIdSum) {
        mDownloadFile->flush();
        mDownloadFile->unmap(mDownloadFileMapPtr);
        mDownloadFile->close();

        //为了确保重命名成功，先检查一下重命名后的文件是否存在，如果存在，就按下载文件重名自动在文件名后加(重复文件数)
         mDownloadFullPath = getAvaliableFilePath(mDownloadFullPath);

        mDownloadFile->rename(mDownloadFullPath);
        mDownloadFile->close();

        for(int i = 0; i < mWorkerCount; i ++) {
            DownloadWorker *worker = mDownloadWorkerList->at(i);
            disconnect(worker, SIGNAL(updateProgress(int, qint64, qint64)), this, SLOT(updateProgress(int, qint64, qint64)));
            disconnect(worker, SIGNAL(error(int, int, QString)), this, SLOT(error(int, int, QString)));
            disconnect(worker, SIGNAL(workerFinished(int)), this, SLOT(workerFinished(int)));
        }

        qDebug("文件下载完毕:%s", mDownloadFullPath.toStdString().c_str());

        mCompletected = true;

//        QFile file(mDownloadFullPathTemp);
//        file.rename(mDownloadFullPath);
//        file.close();

//        this->mDownloadManager->finished(this);
        //    ((DownloadManager*)parent())->finished(this);
    }
}

void DownloadTask::updateProgress(int id, qint64 recived, qint64 total)
{
    qDebug("%d - :%d", id, (int)(recived / (double)total * 100));
}

void DownloadTask::error(int id, int code, QString msg)
{
    qDebug("%d 下载异常 %d:%s", id, code, msg.toStdString().c_str());
}
