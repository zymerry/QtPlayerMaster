#ifndef FFMPEG_MEDIA_H
#define FFMPEG_MEDIA_H

extern "C"
{
#include <pthread.h>
#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavformat/version.h>
#include <libavutil/time.h>
#include <libavutil/mathematics.h>
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include <SDL2/SDL.h>
}

#endif // FFMPEG_MEDIA_H
