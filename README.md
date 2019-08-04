# Humidifier
This is a device to control the humidity in a humidor for storing cigars.
## The idea
I enjoy smoking a cigar from time to time. However, cigars need to be stored at a controlled humidity level until it is time to enjoy them. So I bought a cheap humidor with a passive element in it for just that. A passive element works be releasing humidity until a certain relative humidity is reached but can also absorb excess humidity. In my case, this did not work properly: the foam would never reach the desired percentage of relative humidity. So I set out to solve this issue.
## Setting forth requirements
Being the engineer I am, I set forth the following requirements:
* The relative humidity should be actively controlled.
* The device should fit inside the humidor.
* If the humidity cannot be controlled for whatever reason, the user shall notice.
* No batteries to worry about.
## The solution
The device I built uses a humidity sensor, a fan and a piece of foam soaked in distilled water. If the sensor notices that the humidity drops below a certain threshold, the fan is switched on. It blows air over the wet foam, thus humidifying the air and raising the humidity inside the humidor. After the upper humidity threshold is reached, the fan is switched off. An integrated speaker alerts the user if the humidity cannot be raised after a certain time. The humidifier fits inside the humidor and is powered by a 5V USB charger, plugged into an outlet near the humidor.
## What is in the repository?
I documented my work here and I think it should be pretty self-explanatory. Here is an overview:
### BOM
The bill of materials lists all the material used for building the humidifer. For the fan, just use any generic 30mm 5V fan. I got mine of aliexpress.
### Enclosure
The enclosure I designed holds the fan, PCB and the piece of 50mmx10mm foam I used to store the distilled water. It consists of the following parts:
* The cover clips into the enclosure and has a funnel to soak the foam without taking the cover off.
* The enclosure which has stubs to mount the fan, the PCB and the foam.
* Standoffs I needed to lift the PCB after I realised that it was sitting too low.
### Images
Some images of the humidifier.
### PCB
Contains the Gerber and NG Drill files you need if you want to build this humidifer.
### Schematics
The schematic is stored here. If you open it in Adobe Reader, you can even click the parts to reveal more information.
### Software
The Arduino software is stored here. You will need to load the Arduino bootloader onto the ATiny85 before using it. Check [SpenceKonde's ATTinyCore](https://github.com/SpenceKonde/ATTinyCore) on how to do this.



<img src="images/inuse.jpg" width="600">
