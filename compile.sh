#!/bin/bash

echo "--> removing *.o:"
rm -f *.o test
echo "--> compile program:"
g++-4.9 -std=c++11 -W -O3 -c -I./ test.cpp
echo "--> compile tinythread:"
g++ -W -O3 -c -I./ ./tinythread.cpp
echo "--> compile easywsclient:"
g++ -c easywsclient.cpp -o easywsclient.o
echo "--> linking everything:"
g++ -o test test.o tinythread.o easywsclient.o -lpthread
