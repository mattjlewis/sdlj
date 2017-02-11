package com.diozero.sdl.joystick;

public abstract class JoystickEvent {
	public static enum Type {
		DEVICE(0), AXIS_MOTION(1), BALL_MOTION(2), BUTTON_PRESS(3), HAT_MOTION(4);
		
		private int mask;
		private Type(int index) {
			this.mask = 1<<index;
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
		private boolean added;
		
		public DeviceEvent(int timestamp, int joystickId, boolean added) {
			super(Type.DEVICE, timestamp, joystickId);
			this.added = added;
		}
		
		public boolean isAdded() {
			return added;
		}

		@Override
		public String toString() {
			return "DeviceEvent [" + super.toString() + "]";
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
		 * @return range -1..1
		 */
		public float getValue() {
			return value;
		}

		@Override
		public String toString() {
			return "AxisMotionEvent [" + super.toString() + ", axis=" + axis + ", value=" + value + "]";
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
			return "BallMotionEvent [" + super.toString() + ", ball=" + ball + ", ballValue=" + ballValue + "]";
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
			return "ButtonEvent [" + super.toString() + ", button=" + button + ", pressed=" + pressed + "]";
		}
	}
	
	public static class HatMotionEvent extends JoystickEvent {
		private static final int CENTERED = 0x00;
		private static final int UP = 0x01;
		private static final int RIGHT = 0x02;
		private static final int DOWN = 0x04;
		private static final int LEFT = 0x08;
		
		public static enum HatMotionType {
			SDL_HAT_LEFTUP, SDL_HAT_UP,
			SDL_HAT_RIGHTUP, SDL_HAT_LEFT, SDL_HAT_CENTERED,
			SDL_HAT_RIGHT, SDL_HAT_LEFTDOWN, SDL_HAT_DOWN,
			SDL_HAT_RIGHTDOWN;
			
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
			return "HatMotionEvent [" + super.toString() + ", hat=" + hat + ", value=" + value + "]";
		}
	}
}
