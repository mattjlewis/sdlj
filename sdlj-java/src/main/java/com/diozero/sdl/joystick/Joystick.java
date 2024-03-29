package com.diozero.sdl.joystick;

import java.util.ArrayList;
import java.util.List;

import org.tinylog.Logger;

/*-
 * PS4 DualShock 4
 *
 *     11  Back-8   Start-9   Y-2
 * 13-DPAD-14              X-3   B-1
 *     12      Guide-10       A-0
 *
 * PS3 DualShock 3
 *     8
 *     11  Back-?   Start-?   Y-3
 * ??-DPAD-16              X-2   B-1
 *     12      Guide-??       A-0
 */
public class Joystick extends JoystickInfo implements AutoCloseable {
	private static List<JoystickDeviceListener> deviceListeners;
	static {
		deviceListeners = new ArrayList<>();
	}

	static void newDeviceEvent(JoystickEvent.DeviceEvent event) {
		deviceListeners.forEach(listener -> listener.accept(event));
	}

	public static void addDeviceListener(JoystickDeviceListener consumer) {
		deviceListeners.add(consumer);
	}

	public static void removeDeviceEventListener(JoystickDeviceListener consumer) {
		deviceListeners.remove(consumer);
	}

	private int instanceId;
	private byte[] guid;
	private long joystickPointer;
	private boolean haptic;
	private int numAxes;
	private int numBalls;
	private int numButtons;
	private int numHats;
	private JoystickButtonListener buttonListener;
	private JoystickAxisMotionListener axisMotionListener;
	// FIXME Fix event masking - by sub-system?
	private int eventCategoryMask = 0xffff;

	Joystick(int id, String name, int sdlTypeOrdinal, boolean gameController, int instanceId, byte[] guid,
			long joystickPointer, boolean haptic, int numAxes, int numBalls, int numButtons, int numHats) {
		super(id, name, sdlTypeOrdinal, gameController);

		this.instanceId = instanceId;
		this.guid = guid;
		this.joystickPointer = joystickPointer;
		this.haptic = haptic;
		this.numAxes = numAxes;
		this.numBalls = numBalls;
		this.numButtons = numButtons;
		this.numHats = numHats;
	}

	public int getInstanceId() {
		return instanceId;
	}

	public byte[] getGuid() {
		return guid;
	}

	public long getJoystickPointer() {
		return joystickPointer;
	}

	public boolean isHaptic() {
		return haptic;
	}

	public boolean isAttached() {
		return JoystickNative.isJoystickAttached(joystickPointer);
	}

	public int getNumAxes() {
		return numAxes;
	}

	public int getNumBalls() {
		return numBalls;
	}

	public int getNumButtons() {
		return numButtons;
	}

	public int getNumHats() {
		return numHats;
	}

	public int getAxisValue(int axis) {
		return JoystickNative.getAxisValue(joystickPointer, axis);
	}

	public boolean getButtonValue(int button) {
		return JoystickNative.getButtonValue(joystickPointer, button) == 1;
	}

	public PowerLevel getCurrentPowerLevel() {
		return PowerLevel.valueOf(JoystickNative.getCurrentPowerLevel(joystickPointer));
	}

	public int rumble(int lowFrequencyRumble, int highFrequencyRumble, long durationMs) {
		return JoystickNative.rumble(joystickPointer, lowFrequencyRumble, highFrequencyRumble, durationMs);
	}

	/*-
	public int rumbleTriggers(int leftRumble, int rightRumble, long durationMs) {
		return JoystickNative.rumbleTriggers(pointer, leftRumble, rightRumble, durationMs);
	}
	*/

	public void setButtonListener(JoystickButtonListener consumer) {
		this.buttonListener = consumer;
	}

	public void removeButtonListener() {
		buttonListener = null;
	}

	public void setAxisMotionListener(JoystickAxisMotionListener consumer) {
		this.axisMotionListener = consumer;
	}

	public void removeAxisMotionListener() {
		axisMotionListener = null;
	}

	void newEvent(JoystickEvent event) {
		Logger.debug("eventCategoryMask={}, mask hit={}", Integer.valueOf(eventCategoryMask),
				Integer.valueOf(eventCategoryMask & event.getCategory().getMask()));
		switch (event.getCategory()) {
		case BUTTON_PRESS:
			buttonListener.accept((JoystickEvent.ButtonEvent) event);
			break;
		case AXIS_MOTION:
			axisMotionListener.accept((JoystickEvent.AxisMotionEvent) event);
			break;
		default:
			Logger.warn("Category {} not yet supported in events", event.getCategory());
		}
	}

	@Override
	public void close() {
		Logger.info("close()");
		JoystickNative.closeJoystick(this);
	}

	public enum PowerLevel {
		/*-
		typedef enum
		 {
		     SDL_JOYSTICK_POWER_UNKNOWN = -1,
		     SDL_JOYSTICK_POWER_EMPTY,   <= 5%
		     SDL_JOYSTICK_POWER_LOW,     <= 20%
		     SDL_JOYSTICK_POWER_MEDIUM,  <= 70%
		     SDL_JOYSTICK_POWER_FULL,    <= 100%
		     SDL_JOYSTICK_POWER_WIRED,
		     SDL_JOYSTICK_POWER_MAX
		} SDL_JoystickPowerLevel;
		*/
		UNKNOWN(-1, -1), EMPTY(0, 5), LOW(1, 20), MEDIUM(2, 70), FULL(3, 100), WIRED(4, 100), MAX(5, 100);

		private int sdlOrdinal;
		private int level;

		PowerLevel(int sdlOrdinal, int level) {
			this.sdlOrdinal = sdlOrdinal;
			this.level = level;
		}

		int getSdlOrdinal() {
			return sdlOrdinal;
		}

		public int getLevel() {
			return level;
		}

		static PowerLevel valueOf(int sdlOrdinal) {
			switch (sdlOrdinal) {
			case 0:
				return EMPTY;
			case 1:
				return LOW;
			case 2:
				return MEDIUM;
			case 3:
				return FULL;
			case 4:
				return WIRED;
			case 5:
				return MAX;
			case -1:
			default:
				return UNKNOWN;
			}
		}
	}
}
