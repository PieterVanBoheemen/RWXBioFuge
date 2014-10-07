RWXBioFuge Arduino Code
=======================

Input:
- Time rotary encoder
- RPM rotary encoder
- Start button
- Short run button
- Lid detector
- Infrared RPM sensor

Output:
- Pulses for Electronic Speed Controller
- Serial USB
- Ethernet
- I2C 16x2 LCD display

TO DO:
- Electromagnet lid lock
- Temperature control for cooled centrifuge
- Servo lid opener
- RGD LED strip for eye candy
- GForce or RPM selector instead of % power
- Rotor balance sensor

Dependencies:
- Webduino https://github.com/sirleech/Webduino
- AdaEncoder https://code.google.com/p/adaencoder/
- ooPinChangeInt https://code.google.com/p/oopinchangeint/
- LiquidCrystal_I2 http://hmario.home.xs4all.nl/arduino/LiquidCrystal_I2C/

Credits:
This project has been greatly inspired by the work of:
- Cathal Garvey - Dremelfuge
- Karlin Yeh - OpenFuge
- Drevious - Microfuge 8 Place Rotor
