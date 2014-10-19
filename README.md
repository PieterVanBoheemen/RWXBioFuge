RWXBioFuge
==========

Centrifugation is a powerful method for isolation of compounds such as DNA, proteins, oil or membrane vesicles from a complex mixture. The RWXBioFuge was designed to bring this powerful tool into the hands of labtechnicans, scientists, hackers, makers, diybio-ers and those who cannot afford to procure A-label equipment in low-resource settings. 

It may also be used as an educational project to teach some fundamental principles of physics, chemistry, biology and electronics hands-on. Apart from an assembly guide, this documentation also includes [7 instructions for demonstration experiments and science classes][6].

For assembly workshops or any questions, remarks or suggestions please contact pietervanboheemen@gmail.com

This repository contains the code, source files and instructions for operating the RWXBioFuge Open Source microcentrifuge.

- [ArduinoCode][1]: source code for the Arduino Ethernet
- [Rotor][2]: OpenSCAD source code for the rotor, preview at [YouMagine][5]
- [Device][3]: Bill of Materials, 3D model, 2D CAD files and Fritzing wiring
- [Docs][4]: assembly and tutorial documents

![Render](Device/RWXBioFuge%20preview%20v1.0.png?raw=true)
![Pictures](RWXBioFuge%10collage%20v0.1.png?raw=true)

> **WARNING**: Fast spinning rotors are dangerous! Make sure you read the [safety instructions][7] before use.

> **LICENSE**: All work in this project is licensed under the [Creative Commons Attribution-ShareAlike 4.0 International License][8]. In other words: you are free to use everything in this repository as long as you attribute Pieter van Boheemen and share derivative work under the same conditions.

###Release notes and features of V1.0
The V1.0 release of the RWXBioFuge contains the following features:
- Completely Open Source and designed in freely available software, such as Arduino IDE, Sketchup, OpenSCAD, InkScape and Fritzing 
- Constructed out of solely off-the-shelf or digitally fabricated parts
- High speed centrifuge > 10,000 RPM, depending on your own settings and confidence
- Configurable speed in percentages of full power
- Configurable time in convenient steps, including infinite
- Separate Start and Stop buttons
- Current RPM, Gforce and remaining time is displayed during operation
- A button for Short immediate operation
- Open lid detector
- Web interface to set, start and stop the machine, including status during operation

###Release notes of V0.1
V0.1 is the first development release of RWXBioFuge. It is still in experimental phase with plenty of bugs. All V0.1 source files will be replaced when V1.0 is ready.

###To Do
The following upgrades are planned for the next version:
- Step by step graphical assembly guide in Instructables style
- Optimization of Power Supply, ESC and brushless motor setup to save costs
- Reduced size of the casing by better placement of components
- Single rotary encoder interface to save costs
- Magnetic lid lock to increase safety
- Accelerometer for the detection of unbalanced rotor through vibrations to increase safety
- Transparent window in lid
- Numbered tube holes in rotor
- Improved graphical Web User Interface 

Credits: This project has been greatly inspired by the work of:

    Cathal Garvey - Dremelfuge
    Karlin Yeh - OpenFuge
    Drevious - Microfuge 8 Place Rotor
    Hackteria - Hacked Hard Disk centrifuge


[1]:https://github.com/PieterVanBoheemen/RWXBioFuge/blob/master/ArduinoCode/
[2]:https://github.com/PieterVanBoheemen/RWXBioFuge/blob/master/Rotor/
[3]:https://github.com/PieterVanBoheemen/RWXBioFuge/blob/master/Device/
[4]:https://github.com/PieterVanBoheemen/RWXBioFuge/blob/master/Docs/
[5]:https://www.youmagine.com/designs/microcentrifuge-20-place-rotor
[6]:https://github.com/PieterVanBoheemen/RWXBioFuge/blob/master/Docs/Tutorials/
[7]:https://github.com/PieterVanBoheemen/RWXBioFuge/blob/master/Docs/Usage/
[8]:http://creativecommons.org/licenses/by-sa/4.0/
