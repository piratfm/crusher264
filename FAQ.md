### Q: How to use this library without root-priveleges? ###

Udev rules have to be updated. Add to /etc/udev/rules.d directory file 50-udev-default.rules that contain this strings:

```
# libusb device nodes
SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", NAME="bus/usb/$env{BUSNUM}/$env{DEVNUM}", MODE="0666"
```