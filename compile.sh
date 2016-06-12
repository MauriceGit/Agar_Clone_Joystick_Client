#!/bin/bash

programName="agar"

echo "--> removing *.o:"
rm -f *.o "$programName"
echo "compile vector:"
g++ -I./ -c -o vector.o vector.c
g++ -I./ -c -o sphere.o sphere.c
echo "compile logic:"
g++ -I./ -c -o logic.o logic.c
echo "--> compile program:"
g++-4.9 -std=c++11 -Wno-write-strings -W -O3 -c -I./ test.cpp
echo "--> compile tinythread:"
g++ -W -O3 -c -I./ ./tinythread.cpp
echo "--> compile easywsclient:"
g++ -c easywsclient.cpp -o easywsclient.o
echo "--> linking everything:"
g++ -L/usr/X11R6/lib64 -o "$programName" sphere.o vector.o logic.o test.o tinythread.o easywsclient.o `pkg-config glfw3 --static --cflags --libs` -lGLU -lGL -lGLEW -lpthread -lboost_system -lssl -lcrypto
rm -f *.o
