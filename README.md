# WORK IN PROGRESS

## AI Intruder

Artificial Intelligence backed Intruder detection for home security cameras.
   
<img src="doc/alarm.img.1.jpg" width="500">
    
  
The goal of this project is near 100% accuracy, never miss an intruder but avoid false positives in challenging environments and circumstances. 
  
Tested with Foscam FI9853EP outdoor IP camera's.
  
- leverages camera build in movement detection algorithm to trigger the Neural Network analysis, this to reduce power consumption.
- every suspicious movement is analyzed by a Neural Network which filters the false positives and only triggers when the objects of interest are actually seen (like a person, cat)
  
  
  
BUILD
- install opencv4
  

make dependencies<br>
make
  
  
./aiintruder -c /conf/aiintruder.cfg<br>
  
 
  
notify.sh  
  sudo apt install ssmtp


LICENSE<br>
Please check the accompanied LICENSE file.<br><br>
  
<img src="doc/alarm.img.i2.jpg" width="300">
  

2019-07-13 (C)Bruno Keymolen (bruno.keymolen@gmail.com)
