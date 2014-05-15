#!/usr/bin/python

from Adafruit_BBIO.SPI import SPI

spi = SPI(0,0)

spi.lsbfirst=False
spi.msh=1000000

spi.cshigh=False
spi.xfer2([0x36])
spi.cshigh=True

for reg in range(0x0,0x3d+1,1):
 spi.cshigh=False
 send = [ reg+0xc0 , 0x0 ]
 recv = spi.xfer2(send)
 spi.cshigh=True
 print reg,recv

patable=9*[0]
patable[0]=0x3e+0xc0

spi.cshigh=False
dd = spi.xfer2(patable)
spi.cshigh=True
print 0x3e,dd

rxfifo=65*[0]
rxfifo[0]=0x3f+0xc0

spi.cshigh=False
dd = spi.xfer2(rxfifo)
spi.cshigh=True
print 0x3f,dd









