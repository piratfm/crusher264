# Introduction #

**This patch not updating so far as FFmpeg, so if it fails, try older versions of ffmpeg (for example 16 Apr 2010)**

First You have to install libcrusher264 before compiling FFmpeg.

This patch adding next features to FFmpeg:
  * crushercap to libavdevice as input-device for CrusherCapture.
  * libcrusher264 to libavcodec as H264 encoder for [Crusher](Crusher.md).
  * QBOX demuxer to libavformat as demuxer for streams, produced by MG1264/MG3500



# Patching #

After downloading FFmpeg:

```
$ git clone git://git.ffmpeg.org/ffmpeg/
$ cd ffmpeg
$ git clone git://git.ffmpeg.org/libswscale/
```

You have to download patch: [libcrusher264-16042010.ffmpeg-git.patch](http://tipok.org.ua/downloads/hardware/crusher264/patch/libcrusher264-16042010.ffmpeg-git.patch)

FFmpeg patching:

```
$ patch -p1 < libcrusher264-16042010.ffmpeg-git.patch
```

Then configure/build/install it with libcrusher264 enabled:

```
$ ./configure --enable-gpl --enable-libcrusher264
$ make
$ sudo make install
```