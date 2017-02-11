#include <SDL.h>
#include <SDL_joystick.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	if (SDL_WasInit(SDL_INIT_JOYSTICK) == 0) {
		if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
			printf("Unable to initialise SDL Joystick: %s\n", SDL_GetError());
			return -1;
		}
		SDL_JoystickEventState(SDL_ENABLE);
		SDL_Joystick* joystick = SDL_JoystickOpen(0);
		int num_buttons = SDL_JoystickNumButtons(joystick);
		printf("num buttons=%d\n", num_buttons);
		//while (1) {
		int x;
		for (x = 0; x < 20; x++) {
			printf("Buttons:");
			int i;
			for (i = 0; i < num_buttons; i++) {
				SDL_JoystickUpdate();
				printf(" %2d: %d", i, SDL_JoystickGetButton(joystick, i));
			}
			printf("\n");
			usleep(1000 * 1000);
		}
	}
}
