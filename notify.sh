#!/bin/bash

path=$1
fname=$2


echo -e "to: bruno.keymolen@gmail.com\nsubject: *Intruder*\n"| (cat - && uuencode ${path} ${fname}) | ssmtp bruno.keymolen@gmail.com

#or
#python3 notify-gmail2.py -f $path

