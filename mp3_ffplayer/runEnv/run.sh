#!/bin/sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/lib:/config/lib:./

chmod 777 mp3Player
./mp3Player start.mp3
./mp3Player done.mp3
./mp3Player fail.mp3
./mp3Player Q001.mp3
./mp3Player start.mp3 10
./mp3Player done.mp3 0
./mp3Player fail.mp3 5
./mp3Player Q001.mp3 -20
