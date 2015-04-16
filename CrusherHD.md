# Device Options: #
**still not full, or some options may not work**
  * H264 High/Main/Baseline profiles up to level 4.1.
  * Supports mpeg streams as input.
  * can encode up to 4 D1 streams at the same time.
  * can encode HD streams up to 1080i 50Hz
  * _lack of deinterlacing?_


In this mode USB-Device receiving compressed mpeg video over USB from host-pc, encode it, and send this data over USB to host-pc. Compressed video, that sent to device - is in mpeg2-pes format with only I-frames (like DV). Every even-odd bytes are swapped. Also every frame aligned with padding stream (0xbe).

# Images: #
<img src='http://crusher264.googlecode.com/svn/wiki/crusherhdtop.jpg'>
<img src='http://crusher264.googlecode.com/svn/wiki/crusherhdbottom.jpg'>

<i>i've already tried to read eeprom contents, but without success</i>

<h1>Inside device:</h1>

<ul><li>USB-controller: Cypress EZ2 (<a href='http://www.cypress.com/?mpn=CY7C68013A-56LFXC&l=1'>CY7C68013A-56LFXC</a>)<br>
</li><li>Encoder: Mobilygen/Maxim <a href='http://www.maxim-ic.com/quick_view2.cfm/qv_pk/6092'>MG3500</a>
</li><li>RAM: 2xDDR2 ProMOS <a href='http://www.promos.com.tw/website/html/english/product/V59/V59C1512164QC.pdf'>V59C1512164QCF3</a> (2x64Mb).<br>
</li><li>unknown i2c EEPROM ATMLH936 16B (seems it's analogue of 24C16WP)<br>
</li><li>99Ti 7C <i>(i2c clock generator?)</i>