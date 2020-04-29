#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr);

signals:


public:
    // download
    QString mDownloadPath;
    // download file count
    int mDownloadFileCount;
    // downloading file count
    int mDownloadingFileCount;
    // downloaded file count
    int mDownloadedFileCount;
    // current download speed
    int mCurrentDownloadSpeed;



};

#endif // DOWNLOADMANAGER_H
