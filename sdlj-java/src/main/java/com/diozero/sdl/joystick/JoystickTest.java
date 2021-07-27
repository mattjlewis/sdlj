package com.diozero.sdl.joystick;

import static org.fusesource.jansi.Ansi.ansi;

import org.fusesource.jansi.AnsiConsole;
import org.tinylog.Logger;

public class JoystickTest {
	private static final long RUMBLE_DURATION = 100;
	private static int leftRumble;
	private static long leftRumbleStart;
	private static int rightRumble;
	private static long rightRumbleStart;

	@SuppressWarnings("boxing")
	public static void main(String[] args) {
		Logger.info("Compiled version: {}. Linked version: {}", JoystickNative.getCompiledVersion(),
				JoystickNative.getLinkedVersion());

		try {
			if (JoystickNative.getNumJoysticks() < 1) {
				Logger.error("No joysticks detected.");
				return;
			}

			Logger.info("Detected these joysticks:");
			for (JoystickInfo j_info : JoystickNative.listJoysticks()) {
				Logger.info(j_info);
			}

			// Find the first attached joystick
			for (int joystick_id = 0; joystick_id < JoystickNative.getNumJoysticks(); joystick_id++) {
				Logger.info("Getting Joystick({})", joystick_id);
				try (Joystick joystick = JoystickNative.getJoystickOrGameController(joystick_id)) {
					if (joystick == null) {
						Logger.error("Failed to open Joystick {}", joystick_id);
						return;
					}

					System.out.println("joystick_id: " + joystick_id + ". instance_id: " + joystick.getInstanceId());

					if (!joystick.isAttached()) {
						System.out.println("Not attached");
						continue;
					}

					if (joystick.getName().equals("Wireless Controller Motion Sensors")) {
						System.out.println("Ignoring motion sensors devices");
						continue;
					}

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

					if (JoystickNative.hasLed(joystick.getJoystickPointer())) {
						System.out.println("Has LED!");
						JoystickNative.setLed(joystick.getJoystickPointer(), 127, 127, 127);
					} else {
						System.out.println("No LED detected");
					}

					if (joystick.isGameController()) {
						@SuppressWarnings("resource")
						GameController game_controller = (GameController) joystick;

						Logger.info("GameController ({}) - button mappings: {}, axis mappings: {}",
								game_controller.getGameControllerName(), game_controller.getButtonMappings(),
								game_controller.getAxisMappings());
					}

					Logger.info("Trying low frequency rumble...");
					if (joystick.rumble(0xffff, 0, 400) < 0) {
						Logger.info("Rumble not supported");
					} else {
						Thread.sleep(500);
						Logger.info("Trying high frequency rumble...");
						joystick.rumble(0, 0xffff, 400);
						Thread.sleep(500);
						joystick.rumble(0, 0, 50);
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

					break;
				}
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
			switch (event.getCategory()) {
			case AXIS_MOTION:
				JoystickEvent.AxisMotionEvent axis_motion_event = (JoystickEvent.AxisMotionEvent) event;
				GameController.Axis axis = game_controller.getAxisMapping(axis_motion_event.getAxis());
				System.out.println("Axis: " + axis + ", Value: " + joystick.getAxisValue(axis_motion_event.getAxis()));
				if (axis == GameController.Axis.RIGHTTRIGGER
						&& ((System.currentTimeMillis() - rightRumbleStart) > RUMBLE_DURATION)
						|| rightRumbleStart == 0) {
					// Event value is -1 .. 1
					// Axis.getValue is -32768..32767
					// Rumble is Uint16 so 0..65535
					rightRumbleStart = System.currentTimeMillis();
					rightRumble = (int) ((axis_motion_event.getValue() + 1) * 32768);
					Logger.info("Setting rumble to {}", Integer.valueOf(rightRumble));
					game_controller.rumble(leftRumble, rightRumble, RUMBLE_DURATION);
				} else if (axis == GameController.Axis.LEFTTRIGGER
						&& ((System.currentTimeMillis() - leftRumbleStart) > RUMBLE_DURATION) || leftRumbleStart == 0) {
					// Event value is -1 .. 1
					// Axis.getValue is -32768..32767
					// Rumble is Uint16 so 0..65535
					leftRumbleStart = System.currentTimeMillis();
					leftRumble = (int) ((axis_motion_event.getValue() + 1) * 32768);
					Logger.info("Setting rumble to {}", Integer.valueOf(leftRumble));
					game_controller.rumble(leftRumble, rightRumble, RUMBLE_DURATION);
				}

				break;
			case BUTTON_PRESS:
				GameController.Button button = game_controller
						.getButtonMapping(((JoystickEvent.ButtonEvent) event).getButton());
				System.out.println("Button: " + button);
				if (button == GameController.Button.X) {
					Logger.info("Setting rumble to {}", Integer.valueOf(0));
					game_controller.rumble(0, 0, RUMBLE_DURATION);
				}
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
