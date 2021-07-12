package com.diozero.sdl.joystick;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import org.tinylog.Logger;

public class GameController extends Joystick {
	private long gameControllerPointer;
	private Map<Integer, Button> buttonMappings;
	private Map<Integer, Axis> axisMappings;

	public GameController(int id, String name, int instanceId, long joystickPointer, boolean haptic, int numAxes,
			int numBalls, int numButtons, int numHats, long gameControllerPointer, String mapping) {
		super(id, name, 1, true, instanceId, joystickPointer, haptic, numAxes, numBalls, numButtons, numHats);

		this.gameControllerPointer = gameControllerPointer;
		unpackMapping(mapping);
	}

	public long getGameControllerPointer() {
		return gameControllerPointer;
	}

	public Map<Integer, Button> getButtonMappings() {
		return buttonMappings;
	}

	public Button getButtonMapping(int button) {
		return buttonMappings.get(Integer.valueOf(button));
	}

	public Map<Integer, Axis> getAxisMappings() {
		return axisMappings;
	}

	public Axis getAxisMapping(int axis) {
		return axisMappings.get(Integer.valueOf(axis));
	}

	@Override
	public void close() {
		Logger.info("close()");
		// Note do NOT call Jostick.close()
		JoystickNative.closeGameController(this);
	}

	private void unpackMapping(String mappingItems) {
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

	public enum Button {
		INVALID, A, B, X, Y, BACK, GUIDE, START, LEFTSTICK, RIGHTSTICK, LEFTSHOULDER, RIGHTSHOULDER, DPUP, DPDOWN,
		DPLEFT, DPRIGHT, MISC1, PADDLE1, PADDLE2, PADDLE3, PADDLE4, TOUCHPAD, MAX;
	}

	public enum Axis {
		INVALID, LEFTX, LEFTY, RIGHTX, RIGHTY, LEFTTRIGGER, RIGHTTRIGGER, MAX;
	}
}
