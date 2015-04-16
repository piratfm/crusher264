# Device options: #
  * H264 Baseline profile (I & P frames only).
  * Bitrate, framerate, width, height regulation.
  * Gop size set.
  * RC buffer size set.
  * Deblocking filter flag (+alpha & beta values).
  * Flag to create new gop on scene change.
  * Real-time (or faster) conversion up to 640x480.
  * Max image dimensions: 768x576.

In this mode USB-Device receiving video over USB from host-pc, encode it, and send this data over USB to host-pc.

**Note:** Real-time encoding possible on framesize less than 640x480. Smaller frame size, gives faster encoding.

# Image: #
<img src='http://crusher264.googlecode.com/svn/wiki/crusher.jpg'>


<h1>Inside device:</h1>

<ul><li>USB-controller: Cypress EZ2 (<a href='http://www.cypress.com/?mpn=CY7C68013A-56LFXC&l=1'>CY7C68013A-56LFXC</a>)<br>
</li><li>Encoder: Mobilygen <a href='http://www.mobilygen.com/default.asp?contentID=20'>MG1264</a>
</li><li>RAM: EtronTech (<a href='http://www.alldatasheet.com/datasheet-pdf/pdf/208058/ETRON/EM639165TS-7G.html'>EM639165TS-7G</a>), or similar.