#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QString>
#include <QAbstractAudioDeviceInfo>
#include <QCameraInfo>
#include <QDebug>
#include <string>

/*
** @brief DeviceInfo 设备信息类
** func: Get capture audio and camera device name.
*/
class DeviceInfo
{
public:
    DeviceInfo();
    ~DeviceInfo();
    QString get_audio_device_name();
    QString get_video_device_name();

protected:
    //使用时需要将QString -> const char *  //[AudioDeviceInfo.toUtf8().data()]
    QString AudioDeviceInfo;
    QString VideoDeviceInfo;
};

#endif // DEVICEINFO_H
