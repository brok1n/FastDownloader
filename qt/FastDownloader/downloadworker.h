#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QThread>
#include <QEventLoop>
#include <QEventLoop>
#include <QFile>

class DownloadWorker : public QObject
{
    Q_OBJECT
public:
    ~DownloadWorker();
    explicit DownloadWorker(int id);
    void download(QString url, QFile *downloadFile, qint64 start, qint64 end);
    void download(QString url, uchar *mptr, qint64 start, qint64 end);
    int  id();

signals:
    void workerFinished(int id);
    void updateProgress(int id, qint64 received, qint64 total);
    void error(int id, int code, QString msg);


public slots:
    void start();

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
//    void redirected(const QUrl &url);
//    void sslErrors(const QList<QSslError> &errors);
//    void encrypted();
    void error(QNetworkReply::NetworkError);
    void rfinished();
//    void mfinished(QNetworkReply*);
    void readyRead();


private:
    //worker id
    int mId;

    QNetworkAccessManager *mManager;
    QNetworkReply *mReply;
    qint64 mDownloadIndex;
    // 下载地址
    QString mUrl;
    // 本地文件
    QFile *mDownloadFile;
    //内存映射文件指针
    uchar *mMptr;
    // 下载开始位置
    qint64 mStart;
    // 下载结束位置
    qint64 mEnd;
    //是否下载完毕
    bool mDownloadFinished;
    //是否在等待开始下载
    bool mWaitForDownload;

    QEventLoop *mEventLoop;
};

#endif // DOWNLOADWORKER_H
