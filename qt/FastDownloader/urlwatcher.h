#ifndef URLWATCHER_H
#define URLWATCHER_H

#include <QObject>
#include <QThread>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include <datacenter.h>

class UrlWatcher : public QObject
{
    Q_OBJECT
public:
    explicit UrlWatcher(QObject *parent = nullptr);

public slots:
    // 开始监听线程
    void watcher();

signals:
    // 当发现了URL 就触发这个信号
    void onUrl();

private:

};

#endif // URLWATCHER_H
