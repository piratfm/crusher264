# Stand-Alone Applications #

There is some applications at crusher264/sample directory, like crusher264, that able to work with encoder-sticks. It can send RAW-uncompressed YUV420p video to device and receive RAW h264 stream.

First, RAW-file have to be created, for example with FFmpeg use this command:
```
$ ffmpeg -i test.mpg -f rawvideo -an -deinterlace -s 720x576 -pix_fmt yuv420p -y test720x576.yuv
```

Where test.mpg - input file, that have to be encoded to H264. That command creates a large file, called test720x576.yuv, this file can be sent to device with crusher264 application:
```
$ sample/crusher264 -i test720x576.yuv -s 720x576 -b 1500000 -o test.h264
```

Now we have test.h264 output RAW H264 file that playable by mplayer. You can MUX it to flv/mp4/etc by FFmpeg/mencoder/VLC/gpac/mp4box.

# Patched FFmpeg #

Usage of this device would be much simpler, if FFmpeg patched by [libcrusher264](http://code.google.com/p/crusher264/wiki/ffmpeg) patch, that provides additional H264 encoder to libavcodec.

For example, here is some commands to transcode various video's to H264, that utilize hardware encoder:

```
$ ffmpeg -loglevel debug -async 1 -flags +loop -i input.ts \
> -f flv -vcodec libcrusher264 -deinterlace -g 25 -r 25 -sc_threshold 1 -b 500k -s 320x240 \
> -acodec aac -ab 64k -ac 1 -ar 44100 -y test.flv
```

Creating MPEG-TS stream:
```
$ ffmpeg -flags +loop -i test.mpg -f mpegts -vcodec libcrusher264 -an -deinterlace -g 25 -sc_threshold 1 -b 600k -s 640x480 -y test.ts
```

Transcode live-stream on-the-fly and save it to MPEG-TS file:
```
$ ffmpeg -async 1 -flags +loop -i udp://239.102.1.2:5004 -f mpegts -vcodec libcrusher264 -acodec mp2 -ab 128k -deinterlace -g 25 \
> -sc_threshold 1 -b 500k -s 320x240 -y test.ts
```