#!/bin/sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/lib:/config/lib:./lib/6.4.0

chmod 777 ./bin/6.4.0/Mp3Player

./bin/6.4.0/Mp3Player test_ld.mp3
