#include "audiocapture.h"

AudioCapture::AudioCapture(int _channel_layout, AVSampleFormat _format, int sample_rate_)
    :channel_layout(_channel_layout), format(_format), sample_rate(sample_rate_)
{

}

AudioCapture::~AudioCapture()
{

}

int AudioCapture::AudioCaptureInit()
{
    int ret = 0;
    //1、为packet分配空间 (packet用来读取音频的pcm数据)
    packet = av_packet_alloc();
    av_init_packet(packet);

    //2、为out_frame分配空间 (frame用来输出一帧pcm数据)
    ret = CreateFrame(1024);
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "CreateFrame error!\n");
        return ret;
    }

    //3、打开音频设备
    ret = OpenAudioDevice();
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "OpenAudioDevice error!\n");
        return ret;
    }

    return ret;
}

int AudioCapture::AudioCaptureDeinit()
{
    //1、关闭音频设备
    CloseAudioDevice();

    //2、释放out_frame
    DestoryFrame();

    //3、释放packet
    av_packet_free(&packet);

    return 0;
}

int AudioCapture::CreateFrame(int nb_samples)
{
    int ret = 0;
    out_frame = av_frame_alloc();
    out_frame->channel_layout = channel_layout;
    out_frame->format = format;
    out_frame->sample_rate = nb_samples;

    ret = av_frame_get_buffer(out_frame, 0);
    if (ret != 0){
        av_strerror(ret, error, 128);
        av_log(NULL, AV_LOG_ERROR, "open input failure.[%d][%s]\n", AVERROR(ret), error);
        return ret;
    }
    return 0;
}

void AudioCapture::DestoryFrame()
{
   av_frame_free(&out_frame);
}

int AudioCapture::OpenAudioDevice()
{
    int ret = 0;
    AVInputFormat *inputFmt = av_find_input_format(libName.c_str());
    if (!inputFmt) {
        av_log(NULL, AV_LOG_ERROR, "find lib %s failure.\n", libName.c_str());
        return -1;
    }

    ret = avformat_open_input(&fmt_ctx, AudioDeviceInfo.toUtf8().data(), inputFmt, NULL);
    if (ret != 0) {
        av_strerror(ret, error, 128);
        av_log(NULL, AV_LOG_ERROR, "open input failure.[%d][%s]\n", AVERROR(ret), error);
        return -1;
    }
    av_dump_format(fmt_ctx, 0, AudioDeviceInfo.toUtf8().data(), 0);
    return 0;
}

int AudioCapture::CloseAudioDevice()
{
    if (fmt_ctx) {
        avformat_close_input(&fmt_ctx);
    }
    return 0;
}

int AudioCapture::AudioCaptureParametersReset()
{
    read_size = -1;
    write_size = 0;
}

int AudioCapture::AudioCaptureFrame(AVFrame* &frame)
{
    //最后一片是2184, 这里就只有第一次读的时候，会进 while
    while(read_size < 0) {
        int ret = av_read_frame(fmt_ctx, packet);
        if (ret != 0) {
            return ret;
        }
        av_log(NULL, AV_LOG_DEBUG, "read packet.size = %d, read_size = %d\n", packet->size, read_size);
        //av_packet_unref(packet_);
        read_size = packet->size;
        write_size = 0;
    }

    if (read_size < 4096) { //最后一片是 2184
        memcpy(out_frame->data[0], packet->data + write_size, read_size);
        av_read_frame(fmt_ctx, packet); //不够一帧时，读取 packet 数据，继续填充 frame->data[0] ，长度是 4096 - read_size

        int remain_len = 4096 - read_size;
        memcpy(out_frame->data[0] + read_size, packet->data, remain_len);
        read_size = packet->size - remain_len; //重置新 packet 的 read_size 和 write_size
        write_size = remain_len;
    } else {
        memcpy(out_frame->data[0], packet->data + write_size, 4096);
        read_size -= 4096;
        write_size += 4096;
    }

    //frame_->data[0]的大小是参数控制的，存放的是一帧的数据量，
    //frame_->linesize[0]就是其大小，
    // pkt.size = 88200，包括了好几帧的数据，直接往data[0]里拷贝会越界.
    frame = out_frame;
    return 0;
}
