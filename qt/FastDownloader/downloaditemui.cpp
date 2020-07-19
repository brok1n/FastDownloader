#include "downloaditemui.h"
#include "ui_downloaditemui.h"
#include "downloadtask.h"
#include "common.h"

DownloadItemUi::DownloadItemUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadItemUi),
    mDownloadTask(Q_NULLPTR),
    mProgressBarList(new QList<QProgressBar*>())
{
    ui->setupUi(this);
    mProgressBarList->append(ui->progressBar1);
    mProgressBarList->append(ui->progressBar2);
    mProgressBarList->append(ui->progressBar3);
    mProgressBarList->append(ui->progressBar4);
    mProgressBarList->append(ui->progressBar5);
}

DownloadItemUi::~DownloadItemUi()
{
    delete ui;
}

void DownloadItemUi::bindDownloadTask(DownloadTask *task)
{
    this->mDownloadTask = task;
}

void DownloadItemUi::onParseName(QString name)
{
    qDebug("收到名字:%s", name.toStdString().c_str());
    this->ui->fileNameLabel->setText(name);
}

void DownloadItemUi::onContentLength(qint64 len)
{
    this->ui->contentLenLabel->setText(Common::lenToTxt(len));
}

void DownloadItemUi::onDownloadFailed(QString msg)
{
    if(msg.isEmpty())
    {
        this->ui->statusLabel->setText("下载失败");
    }
    else
    {
        this->ui->statusLabel->setText(msg.insert(0, "下载失败:"));
    }
}

void DownloadItemUi::onSingleDownload()
{
    this->ui->progressBar1->resize(330, 16);
    this->ui->statusLabel->setText("开始下载");
}

void DownloadItemUi::onMultipleDownload()
{
    this->ui->progressBar1->resize(66, 16);
    this->ui->statusLabel->setText("开始下载");
}

void DownloadItemUi::onUpdateProgress(int *progressList, int len)
{
    int count = 0;
    qint64 speed = 0;
    for(int i = 0; i < len; i ++) {
        mProgressBarList->at(i)->setValue(progressList[i]);
        mProgressBarList->at(i)->update();
        count += progressList[i];
         speed += mDownloadTask->getDownloadWorker(i)->getSpeed();
    }
    int progress = int(count / (len * 100.0) * 100);
    this->ui->statusLabel->setText(QString("%1%").arg(progress));

    this->ui->speedLabel->setText(QString("%1/s").arg(Common::lenToTxt(speed)));

}

void DownloadItemUi::downloadCompletected()
{
    this->ui->statusLabel->setText("已完成");
}
