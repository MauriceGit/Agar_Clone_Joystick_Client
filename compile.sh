#!/bin/bash

programName="agar_hmd"
wsPrefix="easywsclient/"
threadPrefix="tinythread/"

echo "--> removing *.o, $programName .."
rm -f *.o "$programName"

echo "--> compile joystick modules .."
g++-4.9 -std=c++11 -I./ -c -o vector.o vector.c -W -O3
g++-4.9 -std=c++11 -I./ -c -o quaternions.o quaternions.c -W -O3
g++-4.9 -std=c++11 -I./ -c -o joystick.o joystick.c -W -O3
g++-4.9 -std=c++11 -I./ -c -o hmd.o hmd.c -W -O3
g++-4.9 -std=c++11 -I./ -c -o joystickCamera.o joystickCamera.c -W -O3

echo "--> compile helper modules .."
g++-4.9 -std=c++11 -I./ -c -o sphere.o sphere.c -W -O3
g++-4.9 -std=c++11 -I./ -c -o logic.o logic.c -W -O3

echo "--> compile program .."
g++-4.9 -std=c++11 -I./ -c -o agar_hmd.o agar_hmd.cpp -Wno-write-strings -W -O3

echo "--> compile tinythread .."
g++-4.9 -std=c++11 -I./ -c -o "$threadPrefix"tinythread.o "$threadPrefix"tinythread.cpp -W -O3

echo "--> compile easywsclient .."
g++-4.9 -std=c++11 -I./ -c -o "$wsPrefix"easywsclient.o "$wsPrefix"easywsclient.cpp -W -O3

echo "--> linking everything .."
g++-4.9 -L/usr/X11R6/lib64 -o "$programName" quaternions.o joystick.o hmd.o joystickCamera.o sphere.o vector.o logic.o agar_hmd.o "$threadPrefix"tinythread.o "$wsPrefix"easywsclient.o `pkg-config glfw3 --static --cflags --libs` -lGLU -lGL -lGLEW -lpthread -lboost_system -lssl -lcrypto

rm -f *.o
