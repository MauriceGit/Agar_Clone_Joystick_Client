
TARGET = agar_joystick

SRCS_C = mtVector.c mtQuaternions.c joystick.c hmd.c joystickCamera.c sphere.c logic.c
SRCS_CPP = agar_hmd.cpp tinythread/tinythread.cpp easywsclient/easywsclient.cpp

OBJS_C = $(SRCS_C:%.c=%.o)
OBJS_CPP = $(SRCS_CPP:%.cpp=%.o)

CC = g++
LD = g++

CC_FLAGS = -std=c++11 -I./ -c -o
CC_PARAMS = -W -O3 -Wno-write-strings
LD_FLAGS = -L/usr/lib/x86_64-linux-gnu/
LD_LIBS  = `pkg-config glfw3 --static --cflags --libs` -lGLU -lGL -lGLEW -lpthread -lssl -lcrypto

.PHONY: all

all: $(TARGET)

$(TARGET): start $(OBJS_C) $(OBJS_CPP) finish
	@echo -n "linking  : $(@) <-- $(OBJS_C) $(OBJS_CPP)"
	@$(LD) $(LD_FLAGS) -o $(TARGET) $(OBJS_C) $(OBJS_CPP) $(LD_LIBS)
	@echo " ... done."

$(OBJS_C): %:
	@echo -n "$(@:%.o=%.c) "
	@$(CC) $(CC_FLAGS) $(@) $(@:%.o=%.c) $(CC_PARAMS)

$(OBJS_CPP): %:
	@echo -n "$(@:%.o=%.cpp) "
	@$(CC) $(CC_FLAGS) $(@) $(@:%.o=%.cpp) $(CC_PARAMS)

start:
	@echo -n "compiling: "

finish:
	@echo "... done."

clean:
	@echo -n "cleaning ... "
	@rm -f $(OBJS_C)
	@rm -f $(OBJS_CPP)
	@echo "done"

