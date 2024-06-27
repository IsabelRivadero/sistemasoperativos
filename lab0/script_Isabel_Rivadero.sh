#!/bin/bash
# -*- ENCODING: UTF-8 -*-

#problema 1
cat /proc/cpuinfo | grep "model name" 

#problema 2
cat /proc/cpuinfo | grep "cpu cores" | wc -l

#problema 3
wget -O ISABEL_in_wonderland.txt https://www.gutenberg.org/files/11/11-0.txt && sed -i 's/Alice/Isabel/g' ISABEL_in_wonderland.txt && sed -i 's/ALICE/ISABEL/g' ISABEL_in_wonderland.txt | rm ISABEL_in_wonderland.txt 

#problema 4
sort -nr -k 5 weather_cordoba.in | head -n 1 | awk '{print $1,$2,$3}'
sort -nr -k 5 weather_cordoba.in | tail -n 1 | awk '{print $1,$2,$3}'

#problema 5
sort -n -k 3 atpplayers.in

#problema 6
awk '{print $0, $7 - $8}' superliga.in | sort -nr -k 2 -k9 | awk '{print $1,$2,$3,$4,$5,$6,$7,$8}'

#problema 7
ip addr | grep -C1 "link/ether"

#problema 8
for f in *; do  mv "$f" "${f%_es.str}.str"; done


