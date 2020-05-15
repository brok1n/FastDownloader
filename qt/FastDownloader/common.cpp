#include "common.h"
#include <math.h>
#include <QLocale>
Common::Common(QObject *parent) : QObject(parent)
{

}

QString Common::lenToTxt(qint64 len)
{
    QLocale locale;
    QString valueText = locale.formattedDataSize(len);
    return valueText;

//    if (len / GB >= 1)//如果当前Byte的值大于等于1GB
//        return (Math.Round(len / (float)GB, 2)).ToString() + "GB";//将其转换成GB
//    else if (len / MB >= 1)//如果当前Byte的值大于等于1MB
//        return (Math.Round(len / (float)MB, 2)).ToString() + "MB";//将其转换成MB
//    else if (len / KB >= 1)//如果当前Byte的值大于等于1KB
//        return (Math.Round(len / (float)KB, 2)).ToString() + "KB";//将其转换成KGB
//    else
//        return len.ToString() + "Byte";//显示Byte值
}
