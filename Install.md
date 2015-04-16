# Pre-Requests #

You need libusb headers.
Also if you want to compile RTMP-publishing application, You need librtmp from [rtmpdump](http://rtmpdump.mplayerhq.hu/).

# Building #

Before making executables from sources, You have to download firmwares archive, that avaliable from here: [firmware.tar.gz](http://tipok.org.ua/downloads/hardware/crusher264/firmware/firmware.tar.gz)

```
$ tar -xvzf  crusher264-1.1.1.tar.gz
$ cd crusher264-1.1.1
$ wget http://tipok.org.ua/downloads/hardware/crusher264/firmware/firmware.tar.gz
$ tar -xvzf firmware.tar.gz
$ ./configure
$ make
```

# Installing #

```
$ sudo make install
```