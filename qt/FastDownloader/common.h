#ifndef COMMON_H
#define COMMON_H

#include <QObject>

const int GB = 1024 * 1024 * 1024;//定义GB的计算常量
const int MB = 1024 * 1024;//定义MB的计算常量
const int KB = 1024;//定义KB的计算常量

class Common : public QObject
{
    Q_OBJECT
public:
    explicit Common(QObject *parent = nullptr);

    //数据类型转换 qint64转成友好的 GB  MB  KB b
    QString lenToTxt(qint64 len);


signals:

};

#endif // COMMON_H
