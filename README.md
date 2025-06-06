# Measure-platform AKA  Prusa spare parts hacks
The code below is the first implemenation based on the spare prusa Parts  
Based on the spares of an old MK2 printer  
Using the Mini Rambo (the one with plug in power connectors)  
Using the X axis with end stop  
Start button connected to pin Z_MIN  

To upload code to the motherboards you need to add the board definition to the board manager.  
  
Add this url to the board manager url's in settings: https://raw.githubusercontent.com/ultimachine/ArduinoAddons/master/package_ultimachine_index.json  
Select board Rambo and COM port.   
And upload.

The code has the main Ino file to edit and some configuration files behind it that can be chosen from based on the exact hardware you are using.
