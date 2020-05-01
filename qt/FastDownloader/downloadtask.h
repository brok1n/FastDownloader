#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H

#include "downloadmanager.h"

#include <QNetworkReply>
#include <QObject>
#include <QNetworkRequest>
#include <QClipboard>
#include <QMimeData>

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




signals:


private slots:
    void probFinished(QNetworkReply *reply);


public:
    //QNetworkAccessManager
    QNetworkAccessManager *mManager;
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
    long mFileSize;
    // split count
    int mSplitCount;
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
