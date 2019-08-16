## AI Intruder

Artificial Intelligence backed Intruder detection for home security cameras.
   
<img src="doc/alarm.img.1.jpg" width="500">
    
  
The goal of this project is near 100% accuracy, never miss an intruder but avoid false positives in challenging environments and circumstances. 
  
Tested with Foscam FI9853EP outdoor IP camera's.
  
- leverages camera build in movement detection algorithm to trigger the Neural Network analysis, this to reduce power consumption.
- every suspicious movement is analyzed by a Neural Network (yolov3) which filters the false positives and only triggers when the objects of interest are actually seen (like a person, cat)
  

#Dependencies
- make dependencies

  
#BUILD
- install opencv4
- make
 

#Run  
./aiintruder -c /conf/aiintruder.cfg<br>
  
 
#Email notifications  
see: notify.sh  
 - sudo apt install ssmtp


#LICENSE<br>
Please check the accompanied LICENSE file.<br><br>

#Raspberry PI 3B+
Aiintruder is tested on on a raspberry PI 3B+

| Algorithm   | Memory  | Decode JPEG |
|-------------|---------|-------------| 
|Yolo v3      | ~560 MB | ~12 Sec     | 
|Yolo v3 tiny | ~160 MB | ~2 Sec      | 

#Todo
- integrate vdecoder an make it build-optional, like on Raspberry PI only Jpegs are used to save resources.  



2019 (C)Bruno Keymolen (bruno.keymolen@gmail.com)
