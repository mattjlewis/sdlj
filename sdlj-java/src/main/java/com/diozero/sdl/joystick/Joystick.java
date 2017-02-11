package com.diozero.sdl.joystick;

import org.pmw.tinylog.Logger;

public class Joystick extends JoystickInfo {
	private int numAxes;
	private int numBalls;
	private int numButtons;
	private int numHats;
	private JoystickEventListener listener;
	private int eventTypeMask;
	
	Joystick(int id, String name, int numAxes, int numBalls, int numButtons, int numHats) {
		super(id, name);
		this.numAxes = numAxes;
		this.numBalls = numBalls;
		this.numButtons = numButtons;
		this.numHats = numHats;
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
		return JoystickNative.getAxisValue(getId(), axis);
	}
	
	public boolean getButtonValue(int button) {
		return JoystickNative.getButtonValue(getId(), button) == 1;
	}
	
	public void setListener(JoystickEventListener listener, JoystickEvent.Type ... types) {
		this.listener = listener;
		setEnabledEventTypes(types);
	}
	
	public void setEnabledEventTypes(JoystickEvent.Type ... types) {
		eventTypeMask = 0;
		for (JoystickEvent.Type type : types) {
			eventTypeMask |= type.getMask();
		}
		Logger.debug("eventTypeMask={}", Integer.valueOf(eventTypeMask));
	}
	
	public void removeListener() {
		listener = null;
	}
	
	void newEvent(JoystickEvent event) {
		Logger.debug("eventTypeMask={}, mask hit={}", Integer.valueOf(eventTypeMask),
				Integer.valueOf(eventTypeMask & event.getType().getMask()));
		if (listener != null && (eventTypeMask & event.getType().getMask()) != 0) {
			listener.accept(event);
		}
	}
	
	public void close() {
		JoystickNative.closeJoystick(this);
	}
}
