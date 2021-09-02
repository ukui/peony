#ifndef COMMON_H
#define COMMON_H
typedef struct TabletApp
{
    long int iTime;
    quint32 serialNumber; //排序序号
    QString desktopName;  //desktop文件名
    QString appName;      //应用名称
    QString appIcon;      //应用图标路径
    QString execCommand;  //应用的执行路径

}TABLETAPP;
Q_DECLARE_METATYPE(TABLETAPP)
#endif // COMMON_H
