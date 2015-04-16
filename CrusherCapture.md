# Device options: #
  * H264 Baseline profile (I & P frames only).
  * Bitrate, framerate, width, height regulation.
  * Due lack of deinterlacing, image width can be 288px max (with proper aspect ratio settings), that's what "blackmagic design" guys calls full 720x576.
  * AAC Audio capture with fixed samplerate 48kHz (may be other, when set proper clock?).

In this mode USB-Device receiving analogue audio-video, convert it to digital and encode, after that it send encoded data over USB to host-pc.

# Image: #

### Reference Design: ###
<img src='http://crusher264.googlecode.com/svn/wiki/crushercapture.jpg'>

<h3>Blackmagic Video Recorder Stick:</h3>
<img src='http://crusher264.googlecode.com/svn/wiki/blackmagic.jpg'>


<h1>Inside device:</h1>

<i>as in Crusher design, but some adc's and eeprom added</i>

<ul><li>USB-controller: Cypress EZ2 (<a href='http://www.cypress.com/?mpn=CY7C68013A-56LFXC&l=1'>CY7C68013A-56LFXC</a>)<br>
</li><li>Encoder: Mobilygen <a href='http://www.mobilygen.com/default.asp?contentID=20'>MG1264</a>
</li><li>RAM: EtronTech (<a href='http://www.alldatasheet.com/datasheet-pdf/pdf/208058/ETRON/EM639165TS-7G.html'>EM639165TS-7G</a>), or similar.<br>
</li><li>Video ADC: <a href='http://www.analog.com/en/analog-to-digital-converters/video-decoders/ADV7180/products/product.html'>ADV7180</a>
</li><li>Audio ADC: <a href='http://www.cirrus.com/en/products/pro/detail/P1067.html'>CS4265</a>
</li><li>i2c EEPROM <a href='.md'>24LC02</a>.