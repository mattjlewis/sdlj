package com.diozero.sdl.joystick;

public class JoystickTest implements JoystickEventListener {
	public static void main(String[] args) {
		new JoystickTest().test();
	}
	
	@SuppressWarnings("static-method")
	private void test() {
		for (JoystickInfo j_info : JoystickNative.listJoysticks()) {
			System.out.println(j_info);
		}
		
		if (JoystickNative.getNumJoysticks() > 0) {
			int id = 0;
			System.out.println("Getting Joystick(" + id + ")");
			Joystick j = JoystickNative.getJoystick(id);
			if (j == null) {
				System.out.println("Failed to open Joystick " + id);
			} else {
				System.out.println("Opened Joystick '" + j.getName()
					+ "', axes=" + j.getNumAxes() + ", balls=" + j.getNumBalls()
					+ ", buttons=" + j.getNumButtons() + ", hats=" + j.getNumHats());
				
				//j.setListener(this, JoystickEvent.Type.AXIS_MOTION, JoystickEvent.Type.BUTTON_PRESS);
				//System.out.println("Processing events...");
				//JoystickNative.processEvents();
				while (true) {
					/*
					System.out.print("Axes:");
					for (int i=0; i<j.getNumAxes(); i++) {
						System.out.format(" %2d:%6d", i, j.getAxisValue(i));
					}
					System.out.println();*/
					System.out.print("Buttons:");
					for (int i=0; i<j.getNumButtons(); i++) {
						System.out.format(" %2d:%3s", Integer.valueOf(i), (j.getButtonValue(i) ? "on" : "off"));
					}
					System.out.println();
				}
				//j.close();
			}			
		}
	}

	@Override
	public void accept(JoystickEvent event) {
		System.out.println(event);
	}
}
