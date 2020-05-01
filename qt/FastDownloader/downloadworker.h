#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QThread>

class DownloadWorker : public QObject
{
    Q_OBJECT
public:
    ~DownloadWorker();
    explicit DownloadWorker();
    void download(QString url, uchar* mptr, long start, long end);

signals:


public slots:
    void start();

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
//    void redirected(const QUrl &url);
//    void sslErrors(const QList<QSslError> &errors);
    void encrypted();
    void error(QNetworkReply::NetworkError);
    void rfinished();
    void mfinished(QNetworkReply*);
    void metaDataChanged();

    void readyRead();


private:


private:
    QNetworkAccessManager *mManager;
    QNetworkReply *mReply;
    // 下载地址
    QString mUrl;
    // 本地文件的内存映射指针
    uchar *mMptr;
    // 下载开始位置
    long mStart;
    // 下载结束位置
    long mEnd;
    //是否下载完毕
    bool mDownloadFinished;
    //是否在等待开始下载
    bool mWaitForDownload;
};

#endif // DOWNLOADWORKER_H
