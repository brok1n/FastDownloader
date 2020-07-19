#ifndef COMMON_H
#define COMMON_H

#include <QObject>


class Common : public QObject
{
    Q_OBJECT
public:
    explicit Common(QObject *parent = nullptr);

    //数据类型转换 qint64转成友好的 GB  MB  KB b
    static QString lenToTxt(qint64 len);


signals:

};

#endif // COMMON_H
