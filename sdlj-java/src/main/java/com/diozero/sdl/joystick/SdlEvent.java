package com.diozero.sdl.joystick;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import org.tinylog.Logger;

public class SdlEvent implements SdlEventTypes {
	private static final int SDL_RELEASED = 0;
	private static final int SDL_PRESSED = 1;

	private int type;
	private int timestamp;
	// Every event is a 56-byte long union struct
	private ByteBuffer eventData;

	public SdlEvent(byte[] padding) {
		this.eventData = ByteBuffer.wrap(padding);
		eventData.order(ByteOrder.nativeOrder());

		type = eventData.getInt();
		timestamp = eventData.getInt();
	}

	public int getType() {
		return type;
	}

	public int getTimestamp() {
		return type;
	}

	public boolean isJoystickOrGameControllerEvent() {
		return type >= SDL_JOYSTICK_EVENT_START && type < SDL_GAMECONTROLLER_EVENT_END;
	}

	public JoystickEvent toJoystickOrGameControllerEvent() {
		if (type < SDL_JOYSTICK_EVENT_START || type >= SDL_GAMECONTROLLER_EVENT_END) {
			return null;
		}

		JoystickEvent j_ev;

		int which = eventData.getInt();

		switch (type) {
		case SDL_JOYAXISMOTION:
		case SDL_CONTROLLERAXISMOTION:
			int axis = eventData.get() & 0xff;
			eventData.get();
			eventData.get();
			eventData.get();
			short value = eventData.getShort();
			j_ev = new JoystickEvent.AxisMotionEvent(timestamp, which, type, axis, value);
			break;
		case SDL_JOYBALLMOTION:
			int ball = eventData.get() & 0xff;
			eventData.get();
			eventData.get();
			eventData.get();
			short xrel = eventData.getShort();
			short yrel = eventData.getShort();
			j_ev = new JoystickEvent.BallMotionEvent(timestamp, which, type, ball, xrel, yrel);
			break;
		case SDL_JOYHATMOTION:
			int hat = eventData.get() & 0xff;
			int hat_value = eventData.get() & 0xff;
			/*-
			 * Zero == CENTERED
			 * SDL_HAT_LEFTUP, SDL_HAT_UP, SDL_HAT_RIGHTUP, SDL_HAT_LEFT, SDL_HAT_CENTERED, SDL_HAT_RIGHT,
			 * SDL_HAT_LEFTDOWN, SDL_HAT_DOWN, SDL_HAT_RIGHTDOWN
			 */
			j_ev = new JoystickEvent.HatMotionEvent(timestamp, which, type, hat, hat_value);
			break;
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
			int button = eventData.get() & 0xff;
			boolean pressed = (eventData.get() & 0xff) == SDL_PRESSED;
			j_ev = new JoystickEvent.ButtonEvent(timestamp, which, type, button, pressed);
			break;
		case SDL_JOYDEVICEADDED:
		case SDL_JOYDEVICEREMOVED:
		case SDL_CONTROLLERDEVICEADDED:
		case SDL_CONTROLLERDEVICEREMOVED:
		case SDL_CONTROLLERDEVICEREMAPPED:
			j_ev = new JoystickEvent.DeviceEvent(timestamp, which, type);
			break;
		case SDL_CONTROLLERTOUCHPADDOWN:
		case SDL_CONTROLLERTOUCHPADMOTION:
		case SDL_CONTROLLERTOUCHPADUP:
			int touchpad = eventData.get();
			int finger = eventData.get();
			float x = eventData.getFloat();
			float y = eventData.getFloat();
			float pressure = eventData.getFloat();
			j_ev = new JoystickEvent.TouchPadEvent(timestamp, which, type, touchpad, finger, x, y, pressure);
			break;
		case SDL_CONTROLLERSENSORUPDATE:
			int sensor = eventData.getInt();
			float f1 = eventData.getFloat();
			float f2 = eventData.getFloat();
			float f3 = eventData.getFloat();
			j_ev = new JoystickEvent.SensorUpdateEvent(timestamp, which, type, sensor, f1, f2, f3);
			break;
		default:
			Logger.warn("Unknown JoystickEvent {}", Integer.valueOf(type));
			j_ev = null;
		}

		return j_ev;
	}
}
