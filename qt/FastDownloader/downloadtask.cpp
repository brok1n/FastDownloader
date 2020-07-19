#include "downloadtask.h"
#include "downloaditemui.h"

#include <QSsl>

DownloadTask::DownloadTask(QNetworkAccessManager *manager, DownloadManager *parent)
    : QObject(parent)
    , mUi(Q_NULLPTR)
    , mReply(Q_NULLPTR)
    , mDownloadThreadList(new QList<QThread*>())
    , mDownloadWorkerList(new QList<DownloadWorker*>())
//    , mThread1(new QThread())
//    , mThread2(new QThread())
//    , mThread3(new QThread())
//    , mThread4(new QThread())
//    , mThread5(new QThread())

//    , mWorker1(new DownloadWorker(1))
//    , mWorker2(new DownloadWorker(2))
//    , mWorker3(new DownloadWorker(3))
//    , mWorker4(new DownloadWorker(4))
//    , mWorker5(new DownloadWorker(5))
    , mMultipleThread(true)
    , mWorkerCount(5)
    , mWorkerIdSum(0)
    , mFinishedWorkerIdSum(0)
    , mPercent(Q_NULLPTR)
    , mDownloadFullPath("")
    , mDownloadFullPathTemp("")
    , mDownloadFile(Q_NULLPTR)
    , mCompletected(false)
{
    this->mManager = manager;
    this->mDownloadManager = parent;

//    mWorker1->moveToThread(mThread1);
//    mWorker2->moveToThread(mThread2);
//    mWorker3->moveToThread(mThread3);
//    mWorker4->moveToThread(mThread4);
//    mWorker5->moveToThread(mThread5);

//    mThread1->start();
//    mThread2->start();
//    mThread3->start();
//    mThread4->start();
//    mThread5->start();

    mPercent = new int[mWorkerCount]();
    for( int i = 1; i <= mWorkerCount; i ++ ) {
        mPercent[i] = 0;
       QThread *thread = new QThread();
       DownloadWorker *worker = new DownloadWorker(i);
       worker->moveToThread(thread);
       mDownloadThreadList->append(thread);
       mDownloadWorkerList->append(worker);
       thread->start();
       connect(this, &DownloadTask::startDownload, worker, &DownloadWorker::start);
    }


//    connect(this, &DownloadTask::startDownload, mWorker1, &DownloadWorker::start);
//    connect(this, &DownloadTask::startDownload, mWorker2, &DownloadWorker::start);
//    connect(this, &DownloadTask::startDownload, mWorker3, &DownloadWorker::start);
//    connect(this, &DownloadTask::startDownload, mWorker4, &DownloadWorker::start);
//    connect(this, &DownloadTask::startDownload, mWorker5, &DownloadWorker::start);
    emit startDownload();

    this->mDownloading = false;
    this->mStop = false;
    this->mPause = false;
    this->mRemove = false;
}

void DownloadTask::init(QString url, QString path, bool multiple)
{
    this->mUrl = url;
    this->mLocalPath = path;
    this->mMultipleThread = multiple;

    this->mDownloading = false;
    this->mStop = false;
    this->mPause = false;
    this->mRemove = false;
    this->mCompletected = false;
    //完成线程id和
    this->mWorkerIdSum = 0;
    this->mFinishedWorkerIdSum = 0;

    //downloadFile
    this->mDownloadFullPath = "";
    this->mDownloadFullPathTemp = "";
    this->mDownloadFile = Q_NULLPTR;
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

    //解决SSL证书错误问题
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    requests.setSslConfiguration(config);

    requests.setUrl(this->mUrl);
    requests.setRawHeader("Range", "bytes=0-4");
    mReply = mManager->get(requests);



    connect(mManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(probFinished(QNetworkReply*)));
    connect(mReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(probError(QNetworkReply::NetworkError)));
}

void DownloadTask::probError(QNetworkReply::NetworkError err) {
//    disconnect(mManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(probFinished(QNetworkReply*)));
    qDebug("资源访问异常！ %d", err);
//    emit onDownloadFailed("资源访问异常");
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

void DownloadTask::bindUi(DownloadItemUi *ui)
{
    mUi = ui;
    connect(this, SIGNAL(onParseName(QString)), ui, SLOT(onParseName(QString)));
    connect(this, SIGNAL(onContentLength(qint64)), ui, SLOT(onContentLength(qint64)));
    connect(this, SIGNAL(onDownloadFailed(QString)), ui, SLOT(onDownloadFailed(QString)));
    connect(this, SIGNAL(onSingleDownload()), ui, SLOT(onSingleDownload()));
    connect(this, SIGNAL(onMultipleDownload()), ui, SLOT(onMultipleDownload()));
    connect(this, SIGNAL(onUpdateProgress(int*, int)), ui, SLOT(onUpdateProgress(int*, int)));
    connect(this, SIGNAL(downloadCompletected()), ui, SLOT(downloadCompletected()));
}

bool DownloadTask::isFree()
{
    return this->mCompletected;
}

DownloadWorker *DownloadTask::getDownloadWorker(int i)
{
    return mDownloadWorkerList->at(i);
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

void DownloadTask::probFinished(QNetworkReply *repl)
{
    qDebug("prob code:%d", repl->error());

    QList<QByteArray> headers = mReply->rawHeaderList();
    for(int i = 0; i < headers.size(); i ++) {
        const char *key = headers[i].data();
        const char *val = mReply->rawHeader(key);
        qDebug("%s: %s", key, val);
    }

    QString acceptRanges = mReply->rawHeader("Accept-Ranges").trimmed();
    QString contentLength = mReply->rawHeader("Content-Length").trimmed();
    QString contentRange = mReply->rawHeader("Content-Range").trimmed();



    QString contentDescription = mReply->rawHeader("Content-Disposition").trimmed();

    QString filename = "";
    if(contentDescription.size() > 10 && contentDescription.indexOf("=") > 0)
    {
        contentDescription.replace("\"", "");
        int idx = contentDescription.indexOf("=");
        if (idx >= 0 && idx < contentDescription.length())
        {
            filename = contentDescription.mid(idx+1);
        }
    }
    qDebug("文件名:%s", filename.toStdString().c_str());

    if(filename.isEmpty())
    {
        auto qurl = new QUrl(mUrl);
        filename = qurl->fileName();
    }

    // 解析到名字 就把名字显示在界面上
    qDebug("设置名字");
    emit onParseName(filename);

    mDownloadFullPath = mLocalPath + QDir::separator() + filename;
    mDownloadFullPath = getAvaliableFilePath(mDownloadFullPath);
    mDownloadFullPathTemp = mDownloadFullPath + ".tmp";

    qDebug("本地文件路径:%s", mDownloadFullPath.toStdString().c_str());
    qDebug("本地文件临时路径:%s", mDownloadFullPathTemp.toStdString().c_str());

    QStringList strList = contentRange.split("/");
    if(strList.size() == 2)
    {
        mFileSize = strList.at(1).toDouble();
    }
    else
    {
        mFileSize = contentLength.toInt();
    }

    emit onContentLength(mFileSize);

    if(repl->error() == 0 )
    {
        if(mMultipleThread && strList.size() == 2)
        {
            qDebug("多线程下载文件大小:%lld", mFileSize);

            //通知 多线程下载
            emit onMultipleDownload();

            mDownloadFile = new QFile(mDownloadFullPathTemp);
            mDownloadFile->resize(mFileSize);
            mDownloadFile->open(QIODevice::ReadWrite);
            mDownloadFile->flush();
            mDownloadFile->close();
            mDownloadFile->deleteLater();
            mDownloadFile = Q_NULLPTR;

            mDownloadFile = new QFile(mDownloadFullPathTemp);
            mDownloadFile->open(QIODevice::ReadWrite);

            mWorkerCount = mDownloadWorkerList->size();
            qint64 start = 0;
            qint64 end = 0;
            qint64 block = mFileSize / mWorkerCount;
            qint64 blockMod = mFileSize % mWorkerCount;
            for(int i = 0; i < mWorkerCount; i ++)
            {
                DownloadWorker *worker = mDownloadWorkerList->at(i);
                this->mWorkerIdSum += worker->id();
                start = i * block;
                end = start + block;
                if(i > 0)
                {
                    start += 1;
                }
                //最后一个 要把多余的加上
                if(i == mWorkerCount - 1)
                {
                    end += blockMod;
                }
                worker->download(mUrl, mDownloadFile, start, end, true);
                connect(worker, SIGNAL(updateProgress(int, qint64, qint64)), this, SLOT(updateProgress(int, qint64, qint64)));
                connect(worker, SIGNAL(error(int, int, QString)), this, SLOT(error(int, int, QString)));
                connect(worker, SIGNAL(workerFinished(int)), this, SLOT(workerFinished(int)));
            }
        }
        else
        {
            qDebug("单线程下载! 文件大小:%lld", mFileSize);

            //通知 单线程下载
            emit onSingleDownload();

            mDownloadFile = new QFile(mDownloadFullPathTemp);
            mDownloadFile->resize(mFileSize);
            mDownloadFile->open(QIODevice::ReadWrite);
            mDownloadFile->flush();
            mDownloadFile->close();

            mDownloadFile = new QFile(mDownloadFullPathTemp);
            mDownloadFile->open(QIODevice::ReadWrite);

            //不支持断点续传 或者控制单线程下载，就只能单个线程直接下载
            DownloadWorker *worker = mDownloadWorkerList->at(0);
            this->mWorkerIdSum = worker->id();
            worker->download(mUrl, mDownloadFile, 0, mFileSize, false);
            connect(worker, SIGNAL(updateProgress(int, qint64, qint64)), this, SLOT(updateProgress(int, qint64, qint64)));
            connect(worker, SIGNAL(error(int, int, QString)), this, SLOT(error(int, int, QString)));
            connect(worker, SIGNAL(workerFinished(int)), this, SLOT(workerFinished(int)));
        }
    }
    else
    {
        emit onDownloadFailed("");
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
        mDownloadFile->close();
        mDownloadFile->deleteLater();
        mDownloadFile = Q_NULLPTR;

        //为了确保重命名成功，先检查一下重命名后的文件是否存在，如果存在，就按下载文件重名自动在文件名后加(重复文件数)
         mDownloadFullPath = getAvaliableFilePath(mDownloadFullPath);

        QFile file(mDownloadFullPathTemp);
        file.rename(mDownloadFullPath);
        file.close();

        for(int i = 0; i < mWorkerCount; i ++) {
            DownloadWorker *worker = mDownloadWorkerList->at(i);
            disconnect(worker, SIGNAL(updateProgress(int, qint64, qint64)), this, SLOT(updateProgress(int, qint64, qint64)));
            disconnect(worker, SIGNAL(error(int, int, QString)), this, SLOT(error(int, int, QString)));
            disconnect(worker, SIGNAL(workerFinished(int)), this, SLOT(workerFinished(int)));
        }

        qDebug("文件下载完毕:%s", mDownloadFullPath.toStdString().c_str());

        mCompletected = true;

        //发送下载完毕的信号
        emit downloadCompletected();

        //释放信号连接
        disconnect(this, SIGNAL(onParseName(QString)), mUi, SLOT(onParseName(QString)));
        disconnect(this, SIGNAL(onContentLength(qint64)), mUi, SLOT(onContentLength(qint64)));
        disconnect(this, SIGNAL(onDownloadFailed(QString)), mUi, SLOT(onDownloadFailed(QString)));
        disconnect(this, SIGNAL(onSingleDownload()), mUi, SLOT(onSingleDownload()));
        disconnect(this, SIGNAL(onMultipleDownload()), mUi, SLOT(onMultipleDownload()) );
        disconnect(this, SIGNAL(onUpdateProgress(int*, int)), mUi, SLOT(onUpdateProgress(int*, int)));
        disconnect(this, SIGNAL(downloadCompletected()), mUi, SLOT(downloadCompletected()));

        for(int i = 0; i < mWorkerCount; i++) {
            mPercent[i] = 0;
        }

    }
}

void DownloadTask::updateProgress(int id, qint64 recived, qint64 total)
{
    int percent = (int)(recived / (double)total * 100);
    qDebug("%d - :%d", id, percent);
    mPercent[id-1] = percent;
    emit onUpdateProgress(mPercent, mWorkerCount);

}

void DownloadTask::error(int id, int code, QString msg)
{
    qDebug("%d 下载异常 %d:%s", id, code, msg.toStdString().c_str());
}
