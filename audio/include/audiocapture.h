#ifndef AUDIOCAPTURE_H
#define AUDIOCAPTURE_H

#include "deviceinfo.h"
#include "ffmpeg_media.h"

/*
** @brief AudioCapture 音频采集类，需要提供采集的设备名和底层库(windows: dshow ; mac: avfoundation)
** use method:
**  first call AudioCaptureInit() init Audio param and open device;
**  scond call AudioCaptureParametersReset() set audio capture parameters;
**  third call AudioCaptureFrame to get a frame pcm data.
*/
class AudioCapture : public DeviceInfo
{
public:
    AudioCapture(int _channel_layout = AV_CH_LAYOUT_STEREO, AVSampleFormat _format = AV_SAMPLE_FMT_S16, int sample_rate_= 44100);
    ~AudioCapture();
    int AudioCaptureInit();
    int AudioCaptureDeinit();

    int AudioCaptureParametersReset();
    int AudioCaptureFrame(AVFrame* &frame);

private:
    int CreateFrame(int nb_samples);
    void DestoryFrame();
    int OpenAudioDevice();
    int CloseAudioDevice();

private:
    int channel_layout;
    AVSampleFormat format;
    int sample_rate;
    AVFrame *out_frame;
    AVPacket *packet;
    AVFormatContext* fmt_ctx;
    int read_size;
    int write_size;
    std::string libName {"dshow"}; //windows下使用的dshow采集,mac下使用avfundation
private:
    char error[1024] {0}; //用于错误打印
};

#endif // AUDIOCAPTURE_H
