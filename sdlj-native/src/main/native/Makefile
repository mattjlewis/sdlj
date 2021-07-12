# Makefile:
# libsdljoystick - Java (JNI) library wrapper of SDL Joystick for the Raspberry Pi
#

RM           := rm -f
CC           := $(CROSS_PREFIX)gcc
AR           := $(CROSS_PREFIX)ar
SIZE         := $(CROSS_PREFIX)size
STRIP        := $(CROSS_PREFIX)strip
SHLIB        := $(CC) -shared
STRIP_FLAGS  :=

LIB_NAME := sdljoystick
SRCS     := com_diozero_sdl_joystick_JoystickNative.c
LDLIBS   := -lSDL2 -lpthread

ifeq ($(OS),Windows_NT)
    uname_S := Windows
else
    uname_S := $(shell uname -s)
endif

ifeq ($(uname_S), Windows)
    OS_JAVA_INC := win32
    APP_EXT := .exe
    LIB_EXT := .dll
endif
ifeq ($(uname_S), Linux)
    OS_JAVA_INC := linux
    LIB_EXT  := .so
    STRIP_FLAGS += --strip-unneeded
endif
ifeq ($(uname_S), Darwin)
    OS_JAVA_INC := darwin
    LIB_EXT  := .dylib
    STRIP_FLAGS += -x
endif

override CFLAGS := -O3 -Wall -pthread -fPIC $(CFLAGS)

INCLUDES   := -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(OS_JAVA_INC) -I$(SDL2_BASE_DIR)/include/SDL2
LIB_PREFIX ?= lib
APP_TARGET := $(APP_NAME)$(APP_EXT)
LIB_TARGET := $(LIB_PREFIX)$(LIB_NAME)$(LIB_EXT)
LDFLAGS    ?=
OBJS       := $(SRCS:.c=.o)

all: $(LIB_TARGET) $(APP_TARGET)

$(LIB_TARGET): $(OBJS)
	@echo [LINK] $(LIB_TARGET)
	@$(SHLIB) -o $(LIB_TARGET) $(OBJS) $(LDFLAGS) $(LDLIBS)
	@$(STRIP) $(STRIP_FLAGS) $(LIB_TARGET)

$(APP_TARGET): $(OBJS)
	@echo [BUILD] $(APP_TARGET)
	@$(CC) -o $(APP_TARGET) $(OBJS) $(LDFLAGS) $(LDLIBS)

.c.o:
	@echo [COMPILE] $<
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) -f $(OBJS) $(TARGET) *~ core tags Makefile.bak

tags: $(SRCS)
	@echo [ctags]
	@ctags $(SRCS)

depend:
	makedepend -Y $(SRCS)
