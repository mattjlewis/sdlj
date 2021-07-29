package com.diozero.sdl.joystick;

@FunctionalInterface
public interface JoystickButtonListener {
	void accept(JoystickEvent.ButtonEvent event);
}
