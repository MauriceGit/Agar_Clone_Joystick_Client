# 3D OpenGL client for an Agar.io like game

This implementation acts as a gui client for the current (2016) programming contest at the FH-Wedel.
The code for the programming contest is hosted here:

[https://github.com/hpatjens/Programmierwettbewerb](https://github.com/hpatjens/Programmierwettbewerb).

It is a game similar to Agar.io but purely for bots.

All game data is pulled from the server and processed to be displayed in a 3D environment, using modern OpenGL (3.3).

In addition to the mouse, an Xbox-Controller can be used to control the view on the field (camera).

# Extern dependencies

All code is statically compiled and linked.

The code makes use of some of my own and foreign libraries. These are licensed. Please read the appropriate license file before
using this code anywhere.

Library | Link | Description
 --- | --- | ---
mtVector | [https://github.com/MauriceGit/Vector_Library](https://github.com/MauriceGit/Vector_Library) | Small basic vector library.
mtQuaternions | [https://github.com/MauriceGit/Quaternion_Library](https://github.com/MauriceGit/Quaternion_Library) | A full Quaternion library with low and high level operations.
mtXboxController | [https://github.com/MauriceGit/XBox_Controller_Linux_Interface](https://github.com/MauriceGit/XBox_Controller_Linux_Interface) | A module that interacts with the usb stream of an Xbox controller for object/camera control.

# Compile and Run

I only tested and ran this simulation on a debian-based Linux OS (Ubuntu, Mint, ...). It should run on other machines as well but is not tested.

## Requirements

The following system-attributes are required for running this simulation:

- A graphics card supporting OpenGL version 3.3 (For the shaders).

- Unix-Libraries: xorg-dev and mesa-common-dev

- GLFW window library

## Running

Compiling and running is pretty straight forward.

- make

- ./agar_joystick

Pressing **'h'** while running the simulation, prints a usage to stdout. Check there for key bindings.

# A few screenshots

![Screenshot](https://github.com/MauriceGit/Agar_Clone_Joystick_Client/blob/master/Screenshots/image_01.png "Screenshot..")
![Screenshot](https://github.com/MauriceGit/Agar_Clone_Joystick_Client/blob/master/Screenshots/image_02.png "Screenshot..")
![Screenshot](https://github.com/MauriceGit/Agar_Clone_Joystick_Client/blob/master/Screenshots/image_03.png "Screenshot..")
![Screenshot](https://github.com/MauriceGit/Agar_Clone_Joystick_Client/blob/master/Screenshots/image_04.png "Screenshot..")
![Screenshot](https://github.com/MauriceGit/Agar_Clone_Joystick_Client/blob/master/Screenshots/image_05.png "Screenshot..")
![Screenshot](https://github.com/MauriceGit/Agar_Clone_Joystick_Client/blob/master/Screenshots/image_06.png "Screenshot..")
![Screenshot](https://github.com/MauriceGit/Agar_Clone_Joystick_Client/blob/master/Screenshots/image_07.png "Screenshot..")











