Apply Neural Network Techology to outdoor Security camera video streams.


The goal of this project is to never miss an intruder but avoid false positives in challenging environments. 

Tested with Foscam FI9853EP outdoor IP camera's.

- leverages camera build in movement detection algorithm to trigger the NN, this reduces power consumption
- every suspicious movement is analyzed by a Neural Network which filters the false positives and only triggers
  when the objects of interest are actually seen (like a person, cat)


BUILD
- install opencv3 first


make dependencies<br>
make

shell 1:<br>
mkdir /tmp/ftp<br>
sudo ./ftpserver/server 21 /tmp/ftp<br>

shell 2:<br>
./nnipcamera -v /tmp/cam-00.pipe<br>



LICENSE<br>
Please check the accompanied LICENSE file.<br><br>

2019-07-13 (C)Bruno Keymolen (bruno.keymolen@gmail.com)
