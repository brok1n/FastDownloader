#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QStandardPaths>
#include <QList>
#include <QMutex>

class DownloadTask;
class DownloadManager : public QObject
{
    Q_OBJECT
public:
    ~DownloadManager();
    static DownloadManager* GetInstance();
    //下载一个文件
    DownloadTask* downloadFile(QString url, QString downloadDir, bool multiple=true);
    //下载完毕
    void finished(DownloadTask*);
    //默认下载路径
    QString getDownloadPath();

private:
    explicit DownloadManager(QObject *parent = nullptr);
    //探测文件，获取文件大小
    qint64 getFileSize(QString url);

signals:


public:
    // download file count
    int mDownloadFileCount;
    // downloading file count
    int mDownloadingFileCount;
    // downloaded file count
    int mDownloadedFileCount;
    // current download speed
    int mCurrentDownloadSpeed;


private:
    static DownloadManager *mInstance;
    static QMutex  mMutex;

    // network manager
    QNetworkAccessManager *mNetAccessManager;
    // task list
    QList<DownloadTask*> *mTaskList;
//    QList<DownloadTask*> *mFreeTaskList;
    // download path  dir
    QString mDownloadPath;

};

#endif // DOWNLOADMANAGER_H
