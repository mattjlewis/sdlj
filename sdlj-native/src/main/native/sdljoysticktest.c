#include <stdio.h>
#include <stdlib.h>
#include "com_diozero_sdl_joystick_JoystickNative.h"

int main(int argc, char* argv[]) {
	if (Java_com_diozero_sdl_joystick_JoystickNative_initialise(NULL, NULL) < 0) {
		exit(1);
	}

	Java_com_diozero_sdl_joystick_JoystickNative_listJoysticksNative(NULL, NULL);
	Java_com_diozero_sdl_joystick_JoystickNative_terminate(NULL, NULL);

	return 0;
}
