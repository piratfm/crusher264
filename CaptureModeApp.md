# Stand-Alone Applications #

### crusher264 ###
Here is sample to capture from CrusherCapture (Blackmagic Video Recorder):

Capture from component-video input, with 352x288 image size and bitrate 300Kbps, also 1-channel audio captured with 48 kHz and bitrate 64Kbps to file, named "test.h264.aac":
```
$ sample/crusher264 -i 2 -s 352x288 -b 300000 -a 64000 -c 1 -o test.h264
```

Capture from composite-video input, with 640x480 image size and bitrate 1500Kbps, also 2-channels audio captured with 48 kHz and bitrate 128Kbps to "qbox" file, that playable by patched ffmpeg:
```
$ sample/crusher264 -i 0 -s 640x480 -b 1500000 -a 128000 -c 2 -f qbox -o test.qbox
```

### crusherRTMP ###
**Publishing works only if librtmp with headers installed.**
Publishing CrusherCapture stream to rtmp-server and saving audio-video to dump.flv. Stream parameters set to: composite-video input, with 320x240 image size and bitrate 300Kbps, also 2-channel audio captured with 48 kHz and bitrate 64Kbps.
```
$ sample/crusherRTMP -i 0 -s 320x240 -b 300000 -a 64000 -f dump.flv rtmp://example.com/live/flv:livestream.flv
```

# Patched FFmpeg #

This samples for patched ffmpeg. Additional parameters, such as bitrates, audio-codec, device-number given by "-i" parameter:
```
$ ffmpeg -flags +global_header -v 10 -loglevel debug -ac 2 -s 640x480 -f crushercap \
> -i devnum=0:bitrate=1800000:ab=128000:acodec=aac \
> -f mp4  -vcodec copy -acodec copy -y test.mp4
```

With smappler bitrate and image-dimensions:
```
$ ffmpeg -flags +global_header -v 10 -loglevel debug -ac 1 -s 320x240 -f crushercap \
> -i devnum=0:bitrate=300000:ab=64000:acodec=aac \
> -f flv -vcodec copy -acodec copy -y test.flv
```