<blockquote><font color='red' size='5'>This project is unsupported, just look at <a href='http://focus.ti.com/paramsearch/docs/parametricsearch.tsp?family=dsp&sectionId=2&tabId=1852&familyId=1300'>DaVinci</a> CPU's from TI installed into <a href='http://designsomething.org/leopardboard/default.aspx'>LeopardBoard</a>. They have better functionality, and open-source!</font></blockquote>

# Introducing: #

This is a source-code repository for crusher264 library, it based on the reverse-engineered [Crusher](Crusher.md), [CrusherCapture](CrusherCapture.md) and [CrusherHD](CrusherHD.md) H264 Encoder stick. It created to implement linux driver for Maxim's MG1264/MG3500 chips.


# crusher264 Documentation: #

  1. Hardware Designs
    * [Crusher](Crusher.md)
    * [CrusherCapture](CrusherCapture.md)
    * [CrusherHD](CrusherHD.md)
  1. [Download](Download.md)
  1. Build/Install
    * [Library and applications](Install.md)
    * [Patching ffmpeg](ffmpeg.md)
  1. Running
    * [EncoderMode applications](EncoderModeApp.md)
    * [CaptureMode applications](CaptureModeApp.md)
  1. [Troubleshooting](http://code.google.com/p/crusher264/issues/list)
  1. [FAQ](FAQ.md)



# Hardware: #

  * [Elgato Turbo.264](http://www.elgato.com/elgato/int/mainmenu/products/Accessories/Turbo264/product1.en.html) (MG1264-based) - [encoder mode](Crusher.md).
  * [Elgato Turbo.264HD](http://www.elgato.com/elgato/int/mainmenu/products/Accessories/Turbo264HD/product1.en.html) (MG3500-based) - **STILL NOT WORKS!** ([hd-encoder mode](CrusherHD.md)). Thanks Florian form [deg.net](http://deg.net) for donating this device.
  * [ADS RDX-160-EF](http://adstechnologies.com/index.php?main_page=product_info&cPath=3&products_id=13) (MG1264-based) - [encoder mode](Crusher.md).
  * [Blackmagic Design Video recorder](http://www.blackmagic-design.com/products/videorecorder/) (MG1264-based) - [capture mode](CrusherCapture.md).


[Encoder-Mode](Crusher.md) supported by Turbo.264/RDX-160-EF<br />
[Capture-Mode](CrusherCapture.md) supported by "Blackmagic Video recorder".

All what not yet done, may be seen in [TODO list](TODO.md).





# News: #

NEWS 2010-11-05:

  * Added environment variable "CRUSHER\_DEV" that tells library what device number to use.
  * Added new application cruserRTMP, that capable to publish CrusherCapture stream to RTMP servers or dump it to flv file.

NEWS 2010-16-04:

  * Now library sends USB-reset to device when it stucked (or does not reply).
  * FFmpeg patch updated (add native nv12 support to encoder).

NEWS 2010-28-03:

  * Add raw-pcm output from CrusherCapture as wave file.
  * Move qbox parser to user-space.
  * added usb reset when i/o failed.
  * updated ffmpeg-patch (added: qbox demuxer, crushercap-device, updated libcrusher264-encoder).

NEWS 2010-15-03:

  * CrusherCapture support added (AAC audio capure supported too).
  * added usb reset after encoding finished (fixed hot-starting bug).

NEWS 2009-27-10:

  * previous code fully rewritten and publically released as crusher264
  * patch for ffmpeg added

