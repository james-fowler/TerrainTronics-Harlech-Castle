# TerrainTronics-Harlech-Castle
Firmware and Schematics for the Harlech Castle 8ch LED controller

https://youtu.be/fndEPuhKIHA

[![Intro Video to Harlech Castle Boards](https://img.youtube.com/vi/fndEPuhKIHA/0.jpg)](https://www.youtube.com/watch?v=fndEPuhKIHA)


I've tried to document the examples in the code.

Example Arduino INO files:

- Simple Patterns
- Wifi Simple On-Off

## Harlech Castle Theory of operation.
Harlech is essentially a pinned out version of the SM16106 IC in a format that's compatible with the Wemos D1 Mini.
It also integrates a switchable transistor that loads the 5V line with a 47Ohm resistor once in a while, to keep a USB battery pack alive.

A short video of the current PG1.0 schematic is below.

[![Intro Video to Harlech Castle Boards](https://img.youtube.com/vi/SuaxnXeibzg/0.jpg)](https://www.youtube.com/watch?v=SuaxnXeibzg)
