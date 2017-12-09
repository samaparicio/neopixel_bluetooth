[Project documentation](https://samaparicio.github.io/bluetooth-neopixels.html)

## Concept

Do you like bright shiny lights? Have you ever wished you could have bright
shiny lights controlled by your smartphone? That is what this project was
about. Clara wanted to add some mood lighting to decorate her bedroom and we
figured it would be cool if she could change the lights to go with the desired
mood.
  
## Design


We are using an Arduino micro controller with a bluetooth module. This allows
us to drive LED-programmable lights (Neopixels) with a widely available
library, and from the Arduino environment we have some experience in, while
enjoying all the coolness of controlling from a smartphone app.

  

The folks at Adafruit made this project a lot easier and so we use their
hardware primarily. It would probably be cheaper to use a Wemo-type device
with Wifi. The ESP32 supports both Wifi and bluetooth and can be programmed in
MicroPython instead of Arduino.

  

5v power comes into a barrel jack and is fed through a capacitor to smooth it
out, to the Neopixel strip, to the Arduino, and to a logic level converter.
Out of PIN 6 comes the signal that drives the Neopixels because this Arduino
has 3.3v logic levels and Neopixels expect 5v logic levels.

  

  

## Parts

[Feather 32u4 Bluefruit LE](http://amzn.to/2kN0rSn)

[TXB108 Level shifter or similar](http://amzn.to/2BQcQIy)

[4700uF capacitor (a smaller one would also work)](http://amzn.to/2iIDxHz)

[2.1mm barrel jack](http://amzn.to/2kMXfWI)

[300 ohm resistor (or thereabouts)](http://amzn.to/2AQJvAg)

[Permaprotoboard](http://amzn.to/2iMlG2D)

[Neopixel strip(s)](http://amzn.to/2kN3l9J)

[5v power supply (I use a 10A one)](http://amzn.to/2Ab2imE)

## Learn more

[Project documentation](https://samaparicio.github.io/bluetooth-neopixels.html)
