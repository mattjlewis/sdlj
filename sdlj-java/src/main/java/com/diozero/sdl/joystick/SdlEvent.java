package com.diozero.sdl.joystick;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class SdlEvent {
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

	public boolean isJoystickEvent() {
		return type > 0x600 && type < 0x700;
	}

	public JoystickEvent toJoystickEvent() {
		if (type < 0x600 || type >= 0x700) {
			return null;
		}

		JoystickEvent j_ev;

		int which = eventData.getInt();

		switch (type) {
		case 0x600: // SDL_JOYAXISMOTION
		case 0x650: // SDL_CONTROLLERAXISMOTION
			int axis = eventData.get() & 0xff;
			eventData.get();
			eventData.get();
			eventData.get();
			short value = eventData.getShort();
			j_ev = new JoystickEvent.AxisMotionEvent(timestamp, which, axis, value);
			break;
		case 0x601: // SDL_JOYBALLMOTION
			int ball = eventData.get() & 0xff;
			eventData.get();
			eventData.get();
			eventData.get();
			short xrel = eventData.getShort();
			short yrel = eventData.getShort();
			j_ev = new JoystickEvent.BallMotionEvent(timestamp, which, ball, xrel, yrel);
			break;
		case 0x602: // SDL_JOYHATMOTION
			int hat = eventData.get() & 0xff;
			int hat_value = eventData.get() & 0xff;
			/*-
			 * Zero == CENTERED
			 * SDL_HAT_LEFTUP, SDL_HAT_UP, SDL_HAT_RIGHTUP, SDL_HAT_LEFT, SDL_HAT_CENTERED, SDL_HAT_RIGHT,
			 * SDL_HAT_LEFTDOWN, SDL_HAT_DOWN, SDL_HAT_RIGHTDOWN
			 */
			j_ev = new JoystickEvent.HatMotionEvent(timestamp, which, hat, hat_value);
			break;
		case 0x603: // SDL_JOYBUTTONDOWN
		case 0x604: // SDL_JOYBUTTONUP
		case 0x651: // SDL_CONTROLLERBUTTONDOWN
		case 0x652: // SDL_CONTROLLERBUTTONUP
			int button = eventData.get() & 0xff;
			boolean pressed = (eventData.get() & 0xff) == SDL_PRESSED;
			j_ev = new JoystickEvent.ButtonEvent(timestamp, which, button, pressed);
			break;
		case 0x605: // SDL_JOYDEVICEADDED
		case 0x606: // SDL_JOYDEVICEREMOVED
		case 0x653: // SDL_CONTROLLERDEVICEADDED
		case 0x654: // SDL_CONTROLLERDEVICEREMOVED
		case 0x655: // SDL_CONTROLLERDEVICEREMAPPED
			j_ev = new JoystickEvent.DeviceEvent(timestamp, which, type);
			break;
		case 0x656: // SDL_CONTROLLERTOUCHPADDOWN
		case 0x657: // SDL_CONTROLLERTOUCHPADMOTION
		case 0x658: // SDL_CONTROLLERTOUCHPADUP
			int touchpad = eventData.get();
			int finger = eventData.get();
			float x = eventData.getFloat();
			float y = eventData.getFloat();
			float pressure = eventData.getFloat();
			j_ev = new JoystickEvent.TouchPadEvent(timestamp, which, type, touchpad, finger, x, y, pressure);
			break;
		case 0x659: // SDL_CONTROLLERSENSORUPDATE
			int sensor = eventData.getInt();
			float f1 = eventData.getFloat();
			float f2 = eventData.getFloat();
			float f3 = eventData.getFloat();
			j_ev = new JoystickEvent.SensorUpdateEvent(timestamp, which, sensor, f1, f2, f3);
			break;
		default:
			System.out.println("Unknown JoystickEvent " + type);
			j_ev = null;
		}

		return j_ev;
	}
}
