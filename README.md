## AI Intruder

Artificial Intelligence backed Intruder detection for home security cameras.


The goal of this project is 100% accuracy, never miss an intruder but avoid false positives in challenging environments and circumstances. 

Tested with Foscam FI9853EP outdoor IP camera's.

- leverages camera build in movement detection algorithm to trigger the Neural Network analysis, this to reduce power consumption.
- every suspicious movement is analyzed by a Neural Network which filters the false positives and only triggers when the objects of interest are actually seen (like a person, cat)


BUILD
- first, install opencv3


make dependencies<br>
make

shell 1:<br>
mkdir /tmp/ftp<br>
sudo ./ftpserver/server 21 /tmp/ftp<br>

shell 2:<br>
./nnipcamera -v /tmp/cam-00.pipe<br>


NFS Mounts
sudo apt-get install nfs-common
sudo mount 192.168.100.5:/mnt/sharedfolder /mnt/sharedfolder_client


LICENSE<br>
Please check the accompanied LICENSE file.<br><br>

2019-07-13 (C)Bruno Keymolen (bruno.keymolen@gmail.com)
