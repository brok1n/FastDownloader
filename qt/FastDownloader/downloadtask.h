#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H

#include "downloadmanager.h"

#include <QNetworkReply>
#include <QObject>
#include <QNetworkRequest>
#include <QClipboard>
#include <QMimeData>
#include <QThread>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#include <downloadworker.h>
class DownloadManager;

class DownloadTask : public QObject
{
    Q_OBJECT
public:
    explicit DownloadTask(QNetworkAccessManager *manager, DownloadManager *parent = nullptr);
    void init( QString url, QString path);
    void setCallBack(void (*updateProgress)(qint64, qint64), void (*finished)(), void (*failed)(QNetworkReply::NetworkError));
    void start();
    void pause();
    void stop();
    void remove();

    //这个task是否是空闲的
    bool isFree();



signals:
    void startDownload();



private slots:
    //探测完毕
    void probFinished(QNetworkReply *reply);

    void workerFinished(int id);
    void updateProgress(int id, qint64 recived, qint64 total);
    void error(int id, int code, QString msg);
//    void worker2Finished();
//    void worker3Finished();
//    void worker4Finished();
//    void worker5Finished();


private:
    //QNetworkAccessManager
    QNetworkAccessManager *mManager;
    //QNetworkReply
    QNetworkReply *mReply;
    // 下载线程列表
    QList<QThread*> *mDownloadThreadList;
    QList<DownloadWorker*> *mDownloadWorkerList;
    QThread *mThread1;
    QThread *mThread2;
    QThread *mThread3;
    QThread *mThread4;
    QThread *mThread5;
    //workerl列表
    DownloadWorker *mWorker1;
    DownloadWorker *mWorker2;
    DownloadWorker *mWorker3;
    DownloadWorker *mWorker4;
    DownloadWorker *mWorker5;

    // 线程数
    int mWorkerCount;
    //完成线程id和
    int mWorkerIdSum;
    int mFinishedWorkerIdSum;

    //downloadFile
    QString mDownloadFullPath;
    QString mDownloadFullPathTemp;
    QFile *mDownloadFile;
    uchar *mDownloadFileMapPtr;
    // is downloading
    bool mDownloading;
    // is pause
    bool mPause;
    // is stoped
    bool mStop;
    // is removed
    bool mRemove;
    // is completected
    bool mCompletected;
    // url
    QString mUrl;
    // local full path
    QString mLocalPath;
    // file size
    qint64 mFileSize;
    // download max speed
    long mMaxSpeed;
    // download min speed  default is 0 ignore 0
    long mMinSpeed;
    // download speed
    long mSpeed;

private:
    DownloadManager *mDownloadManager;

    // update progress function (qint64 bytesReceived, qint64 bytesTotal)
    void (*mPupdateProgress)(qint64, qint64);
//    typeof (void (qint64, qint64)) *mPupdateProgress;
    // finished function
    void (*mPfinished)();
//    typeof (void ()) *mPfinished;
    // failed function
    void (*mPfailed)(QNetworkReply::NetworkError);
//    typeof (void (QNetworkReply::NetworkError)) *mPfailed;

};

#endif // DOWNLOADTASK_H
