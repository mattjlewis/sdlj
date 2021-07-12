package com.diozero.sdl.joystick;

import static org.fusesource.jansi.Ansi.ansi;

import org.fusesource.jansi.AnsiConsole;
import org.tinylog.Logger;

public class JoystickTest {
	@SuppressWarnings("boxing")
	public static void main(String[] args) {
		try {
			if (JoystickNative.getNumJoysticks() < 1) {
				Logger.error("No joysticks detected.");
				return;
			}

			Logger.info("Detected these joysticks:");
			for (JoystickInfo j_info : JoystickNative.listJoysticks()) {
				Logger.info(j_info);
			}

			int joystick_id = 0;
			Logger.info("Getting Joystick({})", joystick_id);
			try (Joystick joystick = JoystickNative.getJoystickOrGameController(joystick_id)) {
				if (joystick == null) {
					Logger.error("Failed to open Joystick {}", joystick_id);
					return;
				}

				System.out.println("joystick_id: " + joystick_id + ". instance_id: " + joystick.getInstanceId());

				/*-
				Joystick (Wireless Controller) has 8 axes (X, Y, Z, Rx, Ry, Rz, Hat0X, Hat0Y)
				and 13 buttons (BtnA, BtnB, BtnX, BtnY, BtnTL, BtnTR, BtnTL2, BtnTR2, BtnSelect, BtnStart, BtnMode, BtnThumbL, BtnThumbR).
				 */
				Logger.info(
						"Joystick ({}) has {} axes, {} buttons, {} hats, and {} balls. Power level: {}. Type: {}."
								+ " Game Controller? {}. Haptic? {}",
						joystick.getName(), joystick.getNumAxes(), joystick.getNumButtons(), joystick.getNumBalls(),
						joystick.getNumHats(), joystick.getCurrentPowerLevel(), joystick.getType(),
						joystick.isGameController(), joystick.isHaptic());

				if (joystick.isGameController()) {
					@SuppressWarnings("resource")
					GameController game_controller = (GameController) joystick;

					Logger.info("GameController - button mappings: {}, axis mappings: {}",
							game_controller.getButtonMappings(), game_controller.getAxisMappings());
				}

				Logger.info("Trying low frequency rumble...");
				if (joystick.rumble(0xffff, 0, 500) < 0) {
					Logger.info("Rumble not supported");
				} else {
					Thread.sleep(500);
					Logger.info("Trying high frequency rumble...");
					joystick.rumble(0, 0xffff, 500);
					Thread.sleep(500);
				}

				/*-
				Logger.info("Trying left trigger rumble...");
				if (joystick.rumbleTriggers(Short.MAX_VALUE / 2, 0, 500) < 0) {
					Logger.info("Rumble triggers not supported");
				} else {
					Thread.sleep(500);
					Logger.info("Trying right trigger rumble...");
					joystick.rumbleTriggers(0, Short.MAX_VALUE / 2, 500);
					Thread.sleep(500);
				}
				*/

				testEvents(joystick);
				// testPoll(joystick);
			}
		} catch (InterruptedException e) {
			Logger.info(e, "Interrupted: {}", e);
		}
	}

	static void testEvents(Joystick joystick) {
		joystick.setListener(event -> processEvent(joystick, event));
		Logger.info("Processing events...");
		JoystickNative.processEvents();
	}

	static void processEvent(Joystick joystick, JoystickEvent event) {
		System.out.println(event);
		if (joystick.isGameController()) {
			GameController game_controller = (GameController) joystick;
			switch (event.getType()) {
			case AXIS_MOTION:
				System.out.println(
						"Axis: " + game_controller.getAxisMapping(((JoystickEvent.AxisMotionEvent) event).getAxis()));
				break;
			case BUTTON_PRESS:
				System.out.println(
						"Button: " + game_controller.getButtonMapping(((JoystickEvent.ButtonEvent) event).getButton()));
				break;
			default:
				// Ignore
			}
		}
	}

	static void testPoll(Joystick joystick) throws InterruptedException {
		// Attempt to initialise Jansi
		try {
			AnsiConsole.systemInstall();
		} catch (Throwable t) {
			// Ignore
			Logger.error(t, "Jansi native library not available on this platform: {}", t);
		}

		/*-
		Testing ... (interrupt to exit)
		Axes:  0:     0  1:     0  2:-32767  3:     0  4:     0  5:-32767  6:     0  7:     0
		Buttons:  0:off  1:off  2:off  3:off  4:off  5:off  6:off  7:off  8:off  9:off 10:off 11:off 12:off
		 */
		boolean first = true;
		while (true) {
			if (first) {
				first = false;
			} else {
				System.out.print(ansi().cursorUpLine(2));
			}

			System.out.print("Axes:");
			for (int i = 0; i < joystick.getNumAxes(); i++) {
				System.out.format(" %2d:%+6d", Integer.valueOf(i), Integer.valueOf(joystick.getAxisValue(i)));
			}
			System.out.println();

			System.out.print("Buttons:");
			for (int i = 0; i < joystick.getNumButtons(); i++) {
				System.out.format(" %2d:%3s", Integer.valueOf(i), joystick.getButtonValue(i) ? "On" : "Off");
			}
			System.out.println();

			Thread.sleep(100);
		}
	}
}
