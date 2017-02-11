package com.diozero.sdl.joystick;

import java.util.function.Consumer;

@FunctionalInterface
public interface JoystickEventListener extends Consumer<JoystickEvent> {
}
