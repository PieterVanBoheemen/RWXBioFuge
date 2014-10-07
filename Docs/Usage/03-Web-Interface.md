03 Web Interface
================

The main reason for the RWXBioFuge to have a webinterface is to enable you to safely hack and tweak the machine by operating it from a distance.

Put the machine in a seperate room or a very strong container and use a regular UTP CAT5 cable to connect the RWXBioFuge to your local network.

By default the machine can be reached via http://192.168.1.210, you may change the IP address in the [source code][1].

1. Open up your browser and surf to http://192.168.1.210
2. The webinterface responds to simple POST or GET commands

Key  | Description | Possible Values
---- | ----------- | ---------------
1    | Speed (%)   | 0 to 100
2    | Time (sec)  | 0 to infinity
3    | Start       | 1 to start, 0 to do nothing
4    | Stop        | 1 to stop, 0 to continue

To tell the RWXBioFuge to spin at 20% for 2 minutes you need to surf to: http://192.168.1.210?1=20&2=120&3=1

3. Alternatively, you may use a form that allows you to send commands to the RWXBioFuge via the webinterface.
4. A frame refreshes every 2 seconds to inform you about the current status of the machine.
5. At the bottom of the page the parsed information is shown.

[1]:https://github.com/PieterVanBoheemen/RWXBioFuge/blob/master/ArduinoCode/
