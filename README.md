Apply Neural Network Techology to outdoor Security camera video streams.


The goal of this project is to never miss an intruder but avoid false positives in challenging environments. 

Tested with Foscam FI9853EP outdoor IP camera's.

- leverages camera build in movement detection algorithm to trigger the NN, this reduces power consumption
- every suspicious movement is analyzed by a Neural Network which filters the false positives and only triggers
  when the objects of interest are actually seen (like a person, cat)


BUILD
- install opencv3 first


make dependencies
make

shell 1:
mkdir /tmp/ftp
sudo ./ftpserver/server 21 /tmp/ftp

shell 2:
./nnipcamera -v /tmp/cam-00.pipe



LICENSE
Please check the accompanied LICENSE file.

2019-07-13 (C)Bruno Keymolen (bruno.keymolen@gmail.com)
