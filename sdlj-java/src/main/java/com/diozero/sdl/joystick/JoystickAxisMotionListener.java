package com.diozero.sdl.joystick;

@FunctionalInterface
public interface JoystickAxisMotionListener {
	void accept(JoystickEvent.AxisMotionEvent event);
}
