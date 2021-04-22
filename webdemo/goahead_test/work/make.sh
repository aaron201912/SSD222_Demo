#!/bin/sh

rm -f webServer
arm-linux-gnueabihf-g++ -o webServer *.cpp -static -L. -lgoahead
#g++ -std=c++11 -o webServer *.cpp ./configJob/*.cpp -static -L. -lgoahead -lpthread
