package com.diozero.sdl.joystick;

public class JoystickInfo {
	public enum Type {
		/*-
		typedef enum
		 {
		     SDL_JOYSTICK_TYPE_UNKNOWN,
		     SDL_JOYSTICK_TYPE_GAMECONTROLLER,
		     SDL_JOYSTICK_TYPE_WHEEL,
		     SDL_JOYSTICK_TYPE_ARCADE_STICK,
		     SDL_JOYSTICK_TYPE_FLIGHT_STICK,
		     SDL_JOYSTICK_TYPE_DANCE_PAD,
		     SDL_JOYSTICK_TYPE_GUITAR,
		     SDL_JOYSTICK_TYPE_DRUM_KIT,
		     SDL_JOYSTICK_TYPE_ARCADE_PAD,
		     SDL_JOYSTICK_TYPE_THROTTLE
		} SDL_JoystickType;
		*/
		UNKNOWN, GAMECONTROLLER, WHEEL, ARCADE_STICK, FLIGHT_STICK, DANCE_PAD, GUITAR, DRUM_KIT, ARCADE_PAD, THROTTLE;

		static Type valueOf(int sdlOrdinal) {
			if (sdlOrdinal < 0 || sdlOrdinal >= values().length) {
				return UNKNOWN;
			}
			return values()[sdlOrdinal];
		}
	}

	private int id;
	private String name;
	private Type type;
	private boolean gameController;

	JoystickInfo(int id, String name, int sdlTypeOrdinal, boolean gameController) {
		this.id = id;
		this.name = name;
		this.type = Type.valueOf(sdlTypeOrdinal);
		this.gameController = gameController;
	}

	public int getId() {
		return id;
	}

	public String getName() {
		return name;
	}

	public Type getType() {
		return type;
	}

	public boolean isGameController() {
		return gameController;
	}

	@Override
	public String toString() {
		return "JoystickInfo [id=" + id + ", name=" + name + ", type=" + type + ", gameController=" + gameController
				+ "]";
	}
}
