QT       += core gui  multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    audio/src/audiocapture.cpp \
    audio/src/audiodecode.cpp \
    audio/src/audioencode.cpp \
    audio/src/audioresample.cpp \
    common/src/avdeque.cpp \
    deviceinfo.cpp \
    main.cpp \
    qtplayermaster.cpp \
    video/src/videocapture.cpp \
    video/src/videodecode.cpp \
    video/src/videoencode.cpp


HEADERS += \
    audio/include/audiocapture.h \
    audio/include/audiodecode.h \
    audio/include/audioencode.h \
    audio/include/audioresample.h \
    common/include/avdeque.h \
    deviceinfo.h \
    ffmpeg_media.h \
    qtplayermaster.h \
    video/include/videocapture.h \
    video/include/videodecode.h \
    video/include/videoencode.h


FORMS += \
    qtplayermaster.ui

######ffmpeg库相关####################################
INCLUDEPATH+=C:/qtplayer/ffmpeg-win64/include
INCLUDEPATH+=audio/include
INCLUDEPATH+=video/include
INCLUDEPATH+=common/include
LIBS+=C:/qtplayer/ffmpeg-win64/lib/libavcodec.dll.a\
      C:/qtplayer/ffmpeg-win64/lib/libavdevice.dll.a\
      C:/qtplayer/ffmpeg-win64/lib/libavfilter.dll.a\
      C:/qtplayer/ffmpeg-win64/lib/libavformat.dll.a\
      C:/qtplayer/ffmpeg-win64/lib/libavutil.dll.a\
      C:/qtplayer/ffmpeg-win64/lib/libswresample.dll.a\
      C:/qtplayer/ffmpeg-win64/lib/libswscale.dll.a\
      C:/qtplayer/ffmpeg-win64/lib/libpostproc.dll.a
######################################################

################sdl库相关##############################
INCLUDEPATH+=C:/qtplayer/SDL2-2.0.14/x86_64-w64-mingw32/include
LIBS+=C:/qtplayer/SDL2-2.0.14/x86_64-w64-mingw32/lib/libSDL2.dll.a \
      C:/qtplayer/SDL2-2.0.14/x86_64-w64-mingw32/lib/libSDL2main.a
######################################################

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
