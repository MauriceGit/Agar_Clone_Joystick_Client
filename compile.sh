#!/bin/bash

programName="agar_joystick"
wsPrefix="easywsclient/"
threadPrefix="tinythread/"

echo "--> removing *.o, $programName .."
rm -f *.o "$programName"

echo "--> compile joystick modules .."
g++ -std=c++11 -I./ -c -o vector.o vector.c -W -O3
g++ -std=c++11 -I./ -c -o quaternions.o quaternions.c -W -O3
g++ -std=c++11 -I./ -c -o joystick.o joystick.c -W -O3
g++ -std=c++11 -I./ -c -o hmd.o hmd.c -W -O3
g++ -std=c++11 -I./ -c -o joystickCamera.o joystickCamera.c -W -O3

echo "--> compile helper modules .."
g++ -std=c++11 -I./ -c -o sphere.o sphere.c -W -O3
g++ -std=c++11 -I./ -c -o logic.o logic.c -W -O3

echo "--> compile program .."
g++ -std=c++11 -I./ -c -o agar_hmd.o agar_hmd.cpp -Wno-write-strings -W -O3

echo "--> compile tinythread .."
g++ -std=c++11 -I./ -c -o "$threadPrefix"tinythread.o "$threadPrefix"tinythread.cpp -W -O3

echo "--> compile easywsclient .."
g++ -std=c++11 -I./ -c -o "$wsPrefix"easywsclient.o "$wsPrefix"easywsclient.cpp -W -O3

echo "--> linking everything .."
g++ -L/usr/lib/x86_64-linux-gnu/ -o "$programName" quaternions.o joystick.o hmd.o joystickCamera.o sphere.o vector.o logic.o agar_hmd.o "$threadPrefix"tinythread.o "$wsPrefix"easywsclient.o `pkg-config glfw3 --static --cflags --libs` -lGLU -lGL -lGLEW -lpthread -lssl -lcrypto

rm -f *.o
rm -f "$wsPrefix"*.o
rm -f "$threadPrefix"*.o
