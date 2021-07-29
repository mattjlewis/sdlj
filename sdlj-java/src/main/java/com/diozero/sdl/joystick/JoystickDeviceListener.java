package com.diozero.sdl.joystick;

@FunctionalInterface
public interface JoystickDeviceListener {
	void accept(JoystickEvent.DeviceEvent event);
}
