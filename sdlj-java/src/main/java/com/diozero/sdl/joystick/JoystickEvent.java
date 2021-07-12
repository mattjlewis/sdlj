package com.diozero.sdl.joystick;

public abstract class JoystickEvent {
	public enum Type {
		DEVICE(0), AXIS_MOTION(1), BALL_MOTION(2), BUTTON_PRESS(3), HAT_MOTION(4), TOUCH_PAD(5), SENSOR(6), UNKNOWN(7);

		private int mask;

		Type(int index) {
			this.mask = 1 << index;
		}

		public int getMask() {
			return this.mask;
		}
	}

	private Type type;
	private int timestamp;
	private int joystickId;

	public JoystickEvent(Type type, int timestamp, int joystickId) {
		this.type = type;
		this.timestamp = timestamp;
		this.joystickId = joystickId;
	}

	public Type getType() {
		return type;
	}

	public int getTimestamp() {
		return timestamp;
	}

	public int getJoystickId() {
		return joystickId;
	}

	public static class DeviceEvent extends JoystickEvent {
		private EventType eventType;

		public DeviceEvent(int timestamp, int joystickId, int eventType) {
			super(Type.DEVICE, timestamp, joystickId);

			this.eventType = EventType.valueOf(eventType);
		}

		public EventType getEventType() {
			return eventType;
		}

		@Override
		public String toString() {
			return "DeviceEvent [eventType=" + eventType + ", getType()=" + getType() + ", getTimestamp()="
					+ getTimestamp() + ", getJoystickId()=" + getJoystickId() + "]";
		}

		private enum EventType {
			JOYSTICK_ADDED, JOYSTICK_REMOVED, GAME_CONTROLLER_ADDED, GAME_CONTROLLER_REMOVED, GAME_CONTROLLER_REMAPPED,
			UNKNOWN;

			static EventType valueOf(int eventType) {
				switch (eventType) {
				case 0x605:
					return JOYSTICK_ADDED;
				case 0x606:
					return JOYSTICK_REMOVED;
				case 0x653:
					return GAME_CONTROLLER_ADDED;
				case 0x654:
					return GAME_CONTROLLER_REMOVED;
				case 0x655:
					return GAME_CONTROLLER_REMAPPED;
				default:
					return UNKNOWN;
				}
			}
		}
	}

	public static class ButtonEvent extends JoystickEvent {
		private int button;
		private boolean pressed;

		public ButtonEvent(int timestamp, int joystickId, int button, boolean pressed) {
			super(Type.BUTTON_PRESS, timestamp, joystickId);
			this.button = button;
			this.pressed = pressed;
		}

		public int getButton() {
			return button;
		}

		public boolean isPressed() {
			return pressed;
		}

		@Override
		public String toString() {
			return "ButtonEvent [button=" + button + ", pressed=" + pressed + ", getType()=" + getType()
					+ ", getTimestamp()=" + getTimestamp() + ", getJoystickId()=" + getJoystickId() + "]";
		}
	}

	public static class AxisMotionEvent extends JoystickEvent {
		// Axis motion range -32768 to 32767
		public static final float AXIS_RANGE = (float) Math.pow(2, 15);

		private int axis;
		private float value;

		public AxisMotionEvent(int timestamp, int joystickId, int axis, int value) {
			super(Type.AXIS_MOTION, timestamp, joystickId);
			this.axis = axis;
			// Scale to -1..1
			this.value = value / AXIS_RANGE;
		}

		public int getAxis() {
			return axis;
		}

		/**
		 * Get the axis motion value
		 *
		 * @return range -1..1
		 */
		public float getValue() {
			return value;
		}

		@Override
		public String toString() {
			return "AxisMotionEvent [axis=" + axis + ", value=" + value + ", getType()=" + getType()
					+ ", getTimestamp()=" + getTimestamp() + ", getJoystickId()=" + getJoystickId() + "]";
		}
	}

	public static class HatMotionEvent extends JoystickEvent {
		private static final int CENTERED = 0x00;
		private static final int UP = 0x01;
		private static final int RIGHT = 0x02;
		private static final int DOWN = 0x04;
		private static final int LEFT = 0x08;

		public enum HatMotionType {
			SDL_HAT_LEFTUP, SDL_HAT_UP, SDL_HAT_RIGHTUP, SDL_HAT_LEFT, SDL_HAT_CENTERED, SDL_HAT_RIGHT,
			SDL_HAT_LEFTDOWN, SDL_HAT_DOWN, SDL_HAT_RIGHTDOWN;

			public static HatMotionType valueOf(int value) {
				HatMotionType type;
				switch (value) {
				case UP:
					type = SDL_HAT_UP;
					break;
				case LEFT | UP:
					type = SDL_HAT_LEFTUP;
					break;
				case RIGHT | UP:
					type = SDL_HAT_RIGHTUP;
					break;
				case LEFT:
					type = SDL_HAT_LEFT;
					break;
				case CENTERED:
					type = SDL_HAT_CENTERED;
					break;
				case RIGHT:
					type = SDL_HAT_RIGHT;
					break;
				case LEFT | DOWN:
					type = SDL_HAT_LEFTDOWN;
					break;
				case DOWN:
					type = SDL_HAT_DOWN;
					break;
				case RIGHT | DOWN:
					type = SDL_HAT_RIGHTDOWN;
					break;
				default:
					throw new IllegalArgumentException("Invalid Hat Motion Type " + value);
				}

				return type;
			}
		}

		private int hat;
		private HatMotionType value;

		public HatMotionEvent(int timestamp, int joystickId, int hat, int value) {
			super(Type.HAT_MOTION, timestamp, joystickId);
			this.hat = hat;
			this.value = HatMotionType.valueOf(value);
		}

		public int getHat() {
			return hat;
		}

		public HatMotionType getValue() {
			return value;
		}

		@Override
		public String toString() {
			return "HatMotionEvent [hat=" + hat + ", value=" + value + ", getType()=" + getType() + ", getTimestamp()="
					+ getTimestamp() + ", getJoystickId()=" + getJoystickId() + "]";
		}
	}

	public static class BallMotionEvent extends JoystickEvent {
		private int ball;
		private Ball ballValue;

		public BallMotionEvent(int timestamp, int joystickId, int ball, int dx, int dy) {
			super(Type.BALL_MOTION, timestamp, joystickId);
			this.ball = ball;
			this.ballValue = new Ball(dx, dy);
		}

		public int getBall() {
			return ball;
		}

		public Ball getBallValue() {
			return ballValue;
		}

		@Override
		public String toString() {
			return "BallMotionEvent [ball=" + ball + ", ballValue=" + ballValue + ", getType()=" + getType()
					+ ", getTimestamp()=" + getTimestamp() + ", getJoystickId()=" + getJoystickId() + "]";
		}
	}

	public static class TouchPadEvent extends JoystickEvent {
		private EventType eventType;
		private int touchPad;
		private int finger;
		private float x;
		private float y;
		private float pressure;

		public TouchPadEvent(int timestamp, int joystickId, int eventType, int touchPad, int finger, float x, float y,
				float pressure) {
			super(Type.TOUCH_PAD, timestamp, joystickId);

			this.eventType = EventType.valueOf(eventType);
			this.touchPad = touchPad;
			this.finger = finger;
			this.x = x;
			this.y = y;
			this.pressure = pressure;
		}

		public int getTouchPad() {
			return touchPad;
		}

		public int getFinger() {
			return finger;
		}

		public float getX() {
			return x;
		}

		public float getY() {
			return y;
		}

		public float getPressure() {
			return pressure;
		}

		@Override
		public String toString() {
			return "TouchPadEvent [eventType=" + eventType + ", touchPad=" + touchPad + ", finger=" + finger + ", x="
					+ x + ", y=" + y + ", pressure=" + pressure + ", getType()=" + getType() + ", getTimestamp()="
					+ getTimestamp() + ", getJoystickId()=" + getJoystickId() + "]";
		}

		public enum EventType {
			DOWN, MOTION, UP, UNKNOWN;

			static EventType valueOf(int eventType) {
				switch (eventType) {
				case 0x656:
					return DOWN;
				case 0x657:
					return MOTION;
				case 0x658:
					return UP;
				default:
					return UNKNOWN;
				}
			}
		}
	}

	public static class SensorUpdateEvent extends JoystickEvent {
		private int sensor;
		private float f1;
		private float f2;
		private float f3;

		public SensorUpdateEvent(int timestamp, int joystickId, int sensor, float f1, float f2, float f3) {
			super(Type.SENSOR, timestamp, joystickId);

			this.sensor = sensor;
			this.f1 = f1;
			this.f2 = f2;
			this.f3 = f3;
		}

		public int getSensor() {
			return sensor;
		}

		public float getF1() {
			return f1;
		}

		public float getF2() {
			return f2;
		}

		public float getF3() {
			return f3;
		}

		@Override
		public String toString() {
			return "SensorUpdateEvent [sensor=" + sensor + ", f1=" + f1 + ", f2=" + f2 + ", f3=" + f3 + ", getType()="
					+ getType() + ", getTimestamp()=" + getTimestamp() + ", getJoystickId()=" + getJoystickId() + "]";
		}
	}
}
