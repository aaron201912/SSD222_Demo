#!/bin/sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/lib:/config/lib:./

chmod 777 Mp3Player

./Mp3Player test_ld.mp3
