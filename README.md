# ESP8266Remotes
This repository has a couple of arduino files that can each be loaded onto an ESP8266 to control home media devices that use IR or RC signals. Each file creates a webserver with endpoints that can be called to control the devices. It also uses Fauxmo to emulate a Wemo device so that it can be controlled by Amazon Echo to turn on and off the device.  

### SimpleExampleIR.ino
This file contains a simple implementation of an IR-controlled device. It uses the IRremoteESP8266 library to send IR signals to control a device. 

### SimpleExampleRC.ino
This file contains a simple implementation of an RC-controlled device. It uses the RCSwitch library to send RC signals to control a device.

### Fireplace-Screen.ino
This file contains two devices: an IR-controlled fireplace and a RC-controlled projector screen.

### Projector-Onkyo.ino
This file contains two devices: an IR-controlled projector and an IR-controlled Onkyo receiver. 