#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QThread>
#include <QEventLoop>
#include <QEventLoop>
#include <QFile>
#include <QByteArray>

#define BUFFER_SIZE 4200000

class DownloadWorker : public QObject
{
    Q_OBJECT
public:
    ~DownloadWorker();
    explicit DownloadWorker(int id);
    void download(QString url, QFile *downloadFile, qint64 start, qint64 end, bool multiple);
    int  id();
    qint64 getSpeed();

signals:
    void workerFinished(int id);
    void updateProgress(int id, qint64 received, qint64 total);
    void error(int id, int code, QString msg);


public slots:
    void start();

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void error(QNetworkReply::NetworkError);
    void rfinished();
    void readyRead();


private:
    //worker id
    int mId;

    QNetworkAccessManager *mManager;
    QNetworkReply *mReply;
    qint64 mDownloadIndex;
    //是否是多线程
    bool mMultiple;
    //下载缓冲区
    QByteArray *mBuffer;
    // 下载地址
    QString mUrl;
    // 本地文件
    QFile *mDownloadFile;
    // 下载开始位置
    qint64 mStart;
    // 下载结束位置
    qint64 mEnd;
    //是否下载完毕
    bool mDownloadFinished;
    //是否在等待开始下载
    bool mWaitForDownload;

    QEventLoop *mEventLoop;
    //下载时间
    qint64 mLastTime;
    //下载数据大小
    qint64 mRecvDataLen;
    //下载速度
    qint64 mSpeed;
};

#endif // DOWNLOADWORKER_H
