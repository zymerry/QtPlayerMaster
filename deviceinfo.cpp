#include "deviceinfo.h"

DeviceInfo::DeviceInfo()
    :AudioDeviceInfo("audio="), VideoDeviceInfo("video=")
{
    /* 获取麦克风的设备名 */ //输入音频
    const auto deviceInfos = QAudioDeviceInfo::availableDevices(QAudio::AudioInput); //QAudio::AudioOutput
    QAudioDeviceInfo deviceInfo;
    for (const QAudioDeviceInfo &deviceInfo_tmp : deviceInfos) {
        qDebug() << "Device name: " << deviceInfo_tmp.deviceName();
        deviceInfo = deviceInfo_tmp;
    }

    AudioDeviceInfo += deviceInfo.deviceName();

    /* 获取camer的设备名称 */
    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : cameras){
        qDebug() << cameraInfo.deviceName();
        qDebug() << cameraInfo.description();
        VideoDeviceInfo += cameraInfo.description();
    }
}

DeviceInfo::~DeviceInfo()
{
    //do nothing
}

QString DeviceInfo::get_audio_device_name()
{
    return AudioDeviceInfo;
}

QString DeviceInfo::get_video_device_name()
{
    return VideoDeviceInfo;
}
