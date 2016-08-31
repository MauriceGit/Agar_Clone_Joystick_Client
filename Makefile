
TARGET = agar_joystick

SRCS_C = vector quaternions joystick hmd joystickCamera sphere logic
SRCS_CPP = agar_hmd tinythread/tinythread easywsclient/easywsclient

OBJS_C = $(SRCS_C:%=%.o)
OBJS_CPP = $(SRCS_CPP:%=%.o)

CC = g++
LD = g++

CC_FLAGS = -std=c++11 -I./ -c -o
LD_FLAGS = -L/usr/lib/x86_64-linux-gnu/
LD_LIBS  = `pkg-config glfw3 --static --cflags --libs` -lGLU -lGL -lGLEW -lpthread -lssl -lcrypto

.PHONY: all

all: $(TARGET)

$(TARGET): clean compile link

clean:
	@echo -n "cleaning ... "
	@rm -f $(TARGET)
	@rm -f $(OBJS_C)
	@rm -f $(OBJS_CPP)
	@echo "done"

compile:
	@echo -n "compiling source files ... "
	@$(foreach SRC, $(SRCS_C),   ( $(CC) $(CC_FLAGS) $(SRC).o $(SRC).c   -W -O3 -Wno-write-strings );)
	@$(foreach SRC, $(SRCS_CPP), ( $(CC) $(CC_FLAGS) $(SRC).o $(SRC).cpp -W -O3 -Wno-write-strings );)
	@echo "done"

link:
	@echo -n "linking object files ... "
	@$(LD) $(LD_FLAGS) -o $(TARGET) $(OBJS_C) $(OBJS_CPP) $(LD_LIBS)
	@echo "done"
