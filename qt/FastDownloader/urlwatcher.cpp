#include "urlwatcher.h"

UrlWatcher::UrlWatcher(QObject *parent) : QObject(parent)
{
}

void UrlWatcher::watcher()
{
    DataCenter *dc = DataCenter::GetInstance();
    while (dc->urlWatcherIsRunning()) {
        QThread::msleep(600);
        QClipboard *clipboard = QApplication::clipboard();
        if(clipboard->mimeData()->hasText()) {
            QString text = clipboard->text().trimmed();
            if(dc->checkUrl(text)) {
                // 发现了URL
                emit onUrl();
            }
        }

    }
}
