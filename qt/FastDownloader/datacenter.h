#ifndef DATACENTER_H
#define DATACENTER_H

#include <QObject>
#include <QMutex>
#include <QSharedMemory>
class DataCenter : public QObject
{
    Q_OBJECT
private:
    explicit DataCenter(QObject *parent = nullptr);
    void initWatchTypeList();

public:
    ~DataCenter();
    static DataCenter* GetInstance();
    void setSharedMemory(QSharedMemory *);

signals:


public:
    // url watcher是否在运行
    bool urlWatcherIsRunning();
    // 停止 url watcher
    void stopUrlWatcher();
    // 监测这个字符串是否可以被触发 新建下载弹窗
    bool checkUrl(QString url);
    // 设置被取消的地址
    void setCancelUrl(QString url);
    // 设置已经触发的url
    void setWatchedUrl(QString url);


private:
    // 是否监控URL  默认监控 true
    bool mUrlWatcherRunning;
    //监控资源类型
    QList<QString> *mWatcherTypeList;
    //被取消的地址  下次再遇到这个地址 就不弹下载框了
    QString mCancelUrl;
    //已经被检测到并发送监测消息的url，之后就不监测这个url了
    QString mWatchedUrl;

    QSharedMemory *mSm;


private:
    static DataCenter *mInstance;
    static QMutex  mMutex;

};

#endif // DATACENTER_H
