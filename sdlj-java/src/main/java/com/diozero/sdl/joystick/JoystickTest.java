package com.diozero.sdl.joystick;

import org.tinylog.Logger;

public class JoystickTest {
	@SuppressWarnings("boxing")
	public static void main(String[] args) {
		try {
			if (JoystickNative.getNumJoysticks() < 1) {
				Logger.error("No joysticks detected.");
				return;
			}

			for (JoystickInfo j_info : JoystickNative.listJoysticks()) {
				Logger.info(j_info);
			}

			int joystick_id = 0;
			Logger.info("Getting Joystick({})", joystick_id);
			try (Joystick joystick = JoystickNative.getJoystick(joystick_id)) {
				if (joystick == null) {
					Logger.error("Failed to open Joystick {}", joystick_id);
					return;
				}

				Logger.info("Opened Joystick '{}', axes={}, balls={}, buttons={}, hats={}", joystick.getName(),
						joystick.getNumAxes(), joystick.getNumBalls(), joystick.getNumButtons(), joystick.getNumHats());

				testPoll(joystick);
			}
		} catch (InterruptedException e) {
			Logger.info(e, "Interrupted: {}", e);
		}
	}

	private static void testEvents(Joystick joystick) {
		joystick.setListener(event -> System.out.println(event), JoystickEvent.Type.AXIS_MOTION,
				JoystickEvent.Type.BUTTON_PRESS);
		Logger.info("Processing events...");
		JoystickNative.processEvents();
	}

	private static void testPoll(Joystick joystick) throws InterruptedException {
		while (true) {
			System.out.print("Axes values:");
			for (int i = 0; i < joystick.getNumAxes(); i++) {
				System.out.format(" %2d:%+5d", Integer.valueOf(i), Integer.valueOf(joystick.getAxisValue(i)));
			}
			System.out.println();

			System.out.print("Buttons:");
			for (int i = 0; i < joystick.getNumButtons(); i++) {
				System.out.format(" %2d:%3s", Integer.valueOf(i), (joystick.getButtonValue(i) ? "On" : "Off"));
			}
			System.out.println();

			Thread.sleep(100);
		}
	}
}
