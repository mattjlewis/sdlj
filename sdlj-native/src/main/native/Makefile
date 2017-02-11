# Makefile:
#	libsdljoystick - Java (JNI) library wrapper of SDL Joystick for the Raspberry Pi
#

APPTARGET=sdljoysticktest
LIBTARGET=libsdljoystick.so

CC	         = gcc
INCLUDE	     = -I. \
               -I/$(JAVA_HOME)/include                         \
               -I/$(JAVA_HOME)/include/linux                   \
               -I/usr/local/include -I/usr/local/include/linux -I/usr/include/SDL2

CFLAGS      = -O3 -Wall -pthread -fPIC $(INCLUDE)
LDFLAGS =

LIBS         = -lSDL2 -lrt -lpthread

# Should not alter anything below this line
###############################################################################

SRC	=   com_diozero_sdl_joystick_JoystickNative.c sdljoysticktest.c

OBJ =	$(SRC:.c=.o)

all:	$(LIBTARGET) $(APPTARGET)
#all:	$(LIBTARGET)

$(LIBTARGET):	$(OBJ)
	@echo [LINK] $(LIBTARGET)
	@$(CC) $(OBJ) -shared -o $(LIBTARGET) $(LIBS)

$(APPTARGET): $(OBJ)
	@echo [BUILD] $(APPTARGET)
	@$(CC) -o $(APPTARGET) $(OBJ) $(LDFALGS) $(LIBS)

.c.o:
	@echo [COMPILE] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) *~ core tags Makefile.bak

tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

depend:
	makedepend -Y $(SRC)
