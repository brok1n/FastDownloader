#include "downloaditemui.h"
#include "ui_downloaditemui.h"
#include "downloadtask.h"

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

void DownloadItemUi::onSingleDownload()
{
    this->ui->progressBar1->resize(330, 23);
}

void DownloadItemUi::onMultipleDownload()
{
    this->ui->progressBar1->resize(66, 23);
}

void DownloadItemUi::onUpdateProgress(int *progressList, int len)
{
    for(int i = 0; i < len; i ++) {
        mProgressBarList->at(i)->setValue(progressList[i]);
    }
}
