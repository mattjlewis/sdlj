package com.diozero.sdl.joystick;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import org.tinylog.Logger;

public class GameController extends Joystick {
	private long gameControllerPointer;
	private String gameControllerName;
	private Map<Integer, Button> buttonMappings;
	private Map<Integer, Axis> axisMappings;
	private Type type;

	public GameController(int id, String name, int instanceId, byte[] guid, long joystickPointer, boolean haptic,
			int numAxes, int numBalls, int numButtons, int numHats, long gameControllerPointer,
			String gameControllerName, String mapping, int type) {
		super(id, name, 1, true, instanceId, guid, joystickPointer, haptic, numAxes, numBalls, numButtons, numHats);

		this.gameControllerPointer = gameControllerPointer;
		this.gameControllerName = gameControllerName;
		unpackMapping(mapping);
		this.type = Type.valueOf(type);
	}

	public long getGameControllerPointer() {
		return gameControllerPointer;
	}

	public String getGameControllerName() {
		return gameControllerName;
	}

	/**
	 * Get the mappings from joystick button ids (as returned by a Joystick
	 * ButtonEvent) to Button enum value. Do not use this if
	 * ButtonEvent.isGameController() returns true.
	 *
	 * @return the mappings from unmapped Joystick button id to GameController
	 *         Button value
	 */
	public Map<Integer, Button> getJoystickButtonMappings() {
		return buttonMappings;
	}

	/**
	 * Convert an unmapped joystick button id to a mapped Button enum. Do not use
	 * this if ButtonEvent.isGameController() returns true.
	 *
	 * @param joystickButton the button id returned from a Joystick Button Event
	 * @return the mapped GameController Button value or INVALID if not found
	 */
	public Button getJoystickButtonMapping(int joystickButton) {
		Button mapped_button = buttonMappings.get(Integer.valueOf(joystickButton));
		if (mapped_button == null) {
			mapped_button = Button.INVALID;
		}
		return mapped_button;
	}

	/**
	 * Get the mappings from joystick axis ids (as returned by a Joystick
	 * AxisMotionEvent) to Axis enum value. Do not use this if
	 * AxisMotionEvent.isGameController() returns true.
	 *
	 * @return the mappings from unmapped Joystick axis id to GameController Axis
	 *         value
	 */
	public Map<Integer, Axis> getJoystickAxisMappings() {
		return axisMappings;
	}

	/**
	 * Convert an unmapped joystick axis id to a mapped Axis enum. Do not use this
	 * if AxisMotionEvent.isGameController() returns true.
	 *
	 * @param joystickAxis the axis id returned from a Joystick AxisMotion Event
	 * @return the mapped Axis or INVALID if not found
	 */
	public Axis getJoystickAxisMapping(int joystickAxis) {
		Axis mapped_axis = axisMappings.get(Integer.valueOf(joystickAxis));
		if (mapped_axis == null) {
			mapped_axis = Axis.INVALID;
		}
		return mapped_axis;
	}

	public int getBindForAxis(int axis) {
		return JoystickNative.getGameControllerBindForAxis(gameControllerPointer, axis);
	}

	public Type getGameControllerType() {
		return type;
	}

	@Override
	public boolean isAttached() {
		return JoystickNative.isGameControllerAttached(gameControllerPointer);
	}

	@Override
	public void close() {
		Logger.info("close()");
		// Note do NOT call Jostick.close()
		JoystickNative.closeGameController(this);
	}

	private void unpackMapping(String mappingItems) {
		System.out.println("processing mapping: " + mappingItems);

		Map<Integer, Button> button_mappings = new HashMap<>();
		Map<Integer, Axis> axis_mappings = new HashMap<>();

		/*-
		 * Example:
		 * 030000004c050000c405000000016800,*,a:b0,b:b1,back:b4,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b5,
		 * leftshoulder:b9,leftstick:b7,lefttrigger:a4,leftx:a0,lefty:a1,rightshoulder:b10,rightstick:b8,
		 * righttrigger:a5,rightx:a2,righty:a3,start:b6,x:b2,y:b3,touchpad:b15,
		 */
		String[] mappings = mappingItems.split(",");
		for (int i = 2; i < mappings.length; i++) {
			String[] mapping_item = mappings[i].split(":");
			String to = mapping_item[0];
			String from = mapping_item[1];
			try {
				if (from.charAt(0) == 'b') {
					int num = Integer.parseInt(from.substring(1));
					String value = to.toUpperCase();
					Button button = Button.valueOf(value);
					button_mappings.put(Integer.valueOf(num), button);
				} else if (from.charAt(0) == 'a') {
					int num = Integer.parseInt(from.substring(1));
					String value = to.toUpperCase();
					Axis axis = Axis.valueOf(value);
					axis_mappings.put(Integer.valueOf(num), axis);
				}
			} catch (IllegalArgumentException e) {
				Logger.warn("Enum value not found for {} processing item {}", to.toUpperCase(), from);
			}
		}

		buttonMappings = Collections.unmodifiableMap(button_mappings);
		axisMappings = Collections.unmodifiableMap(axis_mappings);
	}

	public enum Type {
		// Note starts at 0
		UNKNOWN, XBOX360, XBOXONE, PS3, PS4, NINTENDO_SWITCH_PRO, VIRTUAL, PS5, AMAZON_LUNA, GOOGLE_STADIA;

		public static Type valueOf(int sdlType) {
			if (sdlType < 0 || sdlType >= values().length) {
				return UNKNOWN;
			}

			return values()[sdlType];
		}
	}

	public enum Button {
		// Note starts at -1
		INVALID, A, B, X, Y, BACK, GUIDE, START, LEFTSTICK, RIGHTSTICK, LEFTSHOULDER, RIGHTSHOULDER, LEFTTRIGGER,
		RIGHTTRIGGER, DPUP, DPDOWN, DPLEFT, DPRIGHT, MISC1, PADDLE1, PADDLE2, PADDLE3, PADDLE4, TOUCHPAD, MAX;

		public static Button valueOf(int sdlValue) {
			// Note starts at -1
			int ordinal = sdlValue + 1;
			if (ordinal < 0 || ordinal >= values().length) {
				return INVALID;
			}

			return values()[ordinal];
		}
	}

	public enum Axis {
		// Note starts at -1
		INVALID, LEFTX, LEFTY, RIGHTX, RIGHTY, LEFTTRIGGER, RIGHTTRIGGER, MAX;

		public static Axis valueOf(int sdlValue) {
			// Note starts at -1
			int ordinal = sdlValue + 1;
			if (ordinal < 0 || ordinal >= values().length) {
				return INVALID;
			}

			return values()[ordinal];
		}
	}
}
