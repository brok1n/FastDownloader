#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "downloadtask.h"

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
    static DownloadManager* GetInstance(QObject *parent = nullptr);

    void downloadFile(QString url, QString path);
    void finished(DownloadTask*);

private:
    explicit DownloadManager(QObject *parent = nullptr);
    long getFileSize(QString url);

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
    QList<DownloadTask*> *mFreeTaskList;


};

#endif // DOWNLOADMANAGER_H
