package com.diozero.sdl.joystick;

public class JoystickInfo {
	private int id;
	private String name;

	JoystickInfo(int id, String name) {
		this.id = id;
		this.name = name;
	}

	public int getId() {
		return id;
	}

	public String getName() {
		return name;
	}

	@Override
	public String toString() {
		return "JoystickInfo [id=" + id + ", name=" + name + "]";
	}
}
