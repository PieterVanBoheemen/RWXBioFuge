02 Wiring
=========

The second step is to wire all the components. Take a close look at the [Fritzing arduino shield wiring schematic][1] before connecting all the components.

1. Prepare molex connector
2. Connect brushless motor to molex connector and ATX power supply
3. Connect power switch to ATX power supply
4. Connect Electronic Speed Controller to brushless motor
5. Wire everything to the arduino shield, overlayed with the "RWXBioFuge Shield port indicator"
  1. Connect Electronic Speed Controller
  2. Connect Infra-red RPM sensor 
  3. Connect Lid lever switch
  4. Connect Electromagnet
  5. Connect the Start push button
  6. Connect the Stop push button
  7. Connect the Short push button
  6. Connect the Time rotary encoder
  7. Connect the Speed rotary encoder
  7. Connect I2C LCD
  8. Connect power supply 


> **Tip:** Use tie wraps to bundle the wires.

Continue to the following step [03 Programming][0].

[0]:https://github.com/PieterVanBoheemen/RWXBioFuge/blob/master/Docs/Assembly/03-Programming.md
[1]:https://github.com/PieterVanBoheemen/RWXBioFuge/blob/master/Device/Shield%20v1.0.fzz
