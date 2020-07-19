#include "datacenter.h"

DataCenter* DataCenter::mInstance = nullptr;
QMutex  DataCenter::mMutex;


DataCenter::DataCenter(QObject *parent) : QObject(parent)
  , mUrlWatcherRunning(true)
  , mWatcherTypeList(new QList<QString>())
  , mCancelUrl("")
  , mWatchedUrl("")
{
    this->initWatchTypeList();
}

void DataCenter::initWatchTypeList()
{
    this->mWatcherTypeList->append(".rar");
    this->mWatcherTypeList->append(".zip");
    this->mWatcherTypeList->append(".7z");
    this->mWatcherTypeList->append(".bz2");
    this->mWatcherTypeList->append(".bz");
    this->mWatcherTypeList->append(".gz");
    this->mWatcherTypeList->append(".xz");
    this->mWatcherTypeList->append(".tar");

    this->mWatcherTypeList->append(".dmg");

    this->mWatcherTypeList->append(".bundle");
    this->mWatcherTypeList->append(".rpm");
    this->mWatcherTypeList->append(".pkg");
    this->mWatcherTypeList->append(".deb");
    this->mWatcherTypeList->append(".sh");

    this->mWatcherTypeList->append(".iso");

    this->mWatcherTypeList->append(".whl");

    this->mWatcherTypeList->append(".exe");
    this->mWatcherTypeList->append(".msi");
    this->mWatcherTypeList->append(".vbs");
    this->mWatcherTypeList->append(".bat");

    this->mWatcherTypeList->append(".dll");
    this->mWatcherTypeList->append(".so");

    this->mWatcherTypeList->append(".txt");
    this->mWatcherTypeList->append(".xml");
    this->mWatcherTypeList->append(".json");
    this->mWatcherTypeList->append(".yml");

    this->mWatcherTypeList->append(".pdf");
    this->mWatcherTypeList->append(".doc");
    this->mWatcherTypeList->append(".docx");
    this->mWatcherTypeList->append(".xls");
    this->mWatcherTypeList->append(".xlsx");
    this->mWatcherTypeList->append(".ppt");
    this->mWatcherTypeList->append(".pptx");

    this->mWatcherTypeList->append(".c");
    this->mWatcherTypeList->append(".cs");
    this->mWatcherTypeList->append(".dart");
    this->mWatcherTypeList->append(".h");
    this->mWatcherTypeList->append(".cpp");
    this->mWatcherTypeList->append(".java");
    this->mWatcherTypeList->append(".kt");
    this->mWatcherTypeList->append(".scala");
    this->mWatcherTypeList->append(".lua");
    this->mWatcherTypeList->append(".php");
    this->mWatcherTypeList->append(".py");
    this->mWatcherTypeList->append(".pl");
    this->mWatcherTypeList->append(".go");
    this->mWatcherTypeList->append(".ts");
    this->mWatcherTypeList->append(".ino");

    this->mWatcherTypeList->append(".mp3");
    this->mWatcherTypeList->append(".ape");
    this->mWatcherTypeList->append(".flac");
    this->mWatcherTypeList->append(".lrc");

    this->mWatcherTypeList->append(".mp4");
    this->mWatcherTypeList->append(".ts");
    this->mWatcherTypeList->append(".rmvb");
    this->mWatcherTypeList->append(".flv");
    this->mWatcherTypeList->append(".mkv");

    this->mWatcherTypeList->append(".jpg");
    this->mWatcherTypeList->append(".jpeg");
    this->mWatcherTypeList->append(".bmp");
    this->mWatcherTypeList->append(".gif");
    this->mWatcherTypeList->append(".png");
    this->mWatcherTypeList->append(".ico");

    this->mWatcherTypeList->append(".jar");
    this->mWatcherTypeList->append(".apk");
    this->mWatcherTypeList->append(".ipa");

    this->mWatcherTypeList->append(".psd");

    this->mWatcherTypeList->append(".db");
    this->mWatcherTypeList->append(".mdb");
}

DataCenter::~DataCenter()
{
    this->deleteLater();
    this->mSm->detach();
    qDebug("删除");
}

DataCenter *DataCenter::GetInstance()
{
    if (mInstance == nullptr)
    {
        QMutexLocker locker(&mMutex);
        if (mInstance == nullptr)
        {
            mInstance = new DataCenter();
        }
    }
    return mInstance;
}

void DataCenter::setSharedMemory(QSharedMemory *m)
{
    this->mSm = m;
}

bool DataCenter::urlWatcherIsRunning()
{
    return this->mUrlWatcherRunning;
}

void DataCenter::stopUrlWatcher()
{
    this->mUrlWatcherRunning = false;
}

bool DataCenter::checkUrl(QString url)
{
    if(url == Q_NULLPTR or url.isEmpty()) {
        return false;
    }
    if (url == mCancelUrl || url == mWatchedUrl ) {
        return false;
    }
    QStringList sp = url.split(".");
    if(sp.size() <= 0 ) {
        return false;
    }
    QString ext = "." + sp.at(sp.size()-1);
    if((url.startsWith("http://") || url.startsWith("https://"))) {
        // 记录这个匹配的地址, 下次就不再监测这个url了，除非URL发生了变化
        mWatchedUrl = url;
        return true;
    }
    return false;
}

void DataCenter::setCancelUrl(QString url)
{
    this->mCancelUrl = url;
}

void DataCenter::setWatchedUrl(QString url)
{
    this->mWatchedUrl = url;
}
