# Nextion display Arduino Library

This is (yet another) Arduino Library for use with Itead Nextion display (https://nextion.tech/).
It offers display and component objects manipulation, setting text, number, attributes, etc.
I tried to keep memory use as low as possible, so should be usable even on low RAM basic Arduino boards.

The library offers one class, nxtLcd, to manipulate various aspect of display.

Support all the widgets, including trasparent data mode for wave widgets (but not yet for eeprom).

All test were made on a [NX4024K032](https://nextion.tech/datasheets/nx4024k032/) display using an Arduino Nano (clone) and an
STM32 board as platform.

Documentation is on source files (fair incomplete, not so much comments)

# Useful Resources

- Nextion complete instruction list: https://nextion.tech/instruction-set/

- Nextion editor guide: https://nextion.tech/editor_guide/

- Nextion Product Datasheets: https://nextion.tech/datasheets/

# Bugs

Probably a lot.
In nxtLcd::addWaveBytes(), if the size of data is grater than buffer, after the 2° run of data chunks there is a 
glitch in the data, not discovered yet why.


# TODO

(...and problably never will be done)

- Professional display support
- Eeprom support
- Touch drawing support


(c) Alessandro Guarguaglini - ilguargua@gmail.com - 2019
