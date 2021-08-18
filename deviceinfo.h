#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QString>
#include <QAbstractAudioDeviceInfo>
#include <QCameraInfo>
#include <QDebug>

class DeviceInfo
{
public:
    DeviceInfo();
    ~DeviceInfo();
    QString get_audio_device_name();
    QString get_video_device_name();
private:
    //使用时需要将QString -> const char *  //[AudioDeviceInfo.toUtf8().data()]
    QString AudioDeviceInfo;
    QString VideoDeviceInfo;
};

#endif // DEVICEINFO_H
