#ifndef DOWNLOADITEMUI_H
#define DOWNLOADITEMUI_H

#include <QtWidgets/QProgressBar>
#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class DownloadItemUi;
}
QT_END_NAMESPACE

class DownloadTask;
class DownloadItemUi : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadItemUi(QWidget *parent = nullptr);
    ~DownloadItemUi();
    void bindDownloadTask(DownloadTask* task);

public slots:
    void onParseName(QString name);
    void onContentLength(qint64 len);
    void onDownloadFailed(QString msg);
    void onSingleDownload();
    void onMultipleDownload();
    void onUpdateProgress(int *, int);
    void downloadCompletected();

private:
    Ui::DownloadItemUi *ui;
    DownloadTask *mDownloadTask;
    QList<QProgressBar*> *mProgressBarList;
};

#endif // DOWNLOADITEMUI_H
