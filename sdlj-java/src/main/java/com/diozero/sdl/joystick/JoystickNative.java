package com.diozero.sdl.joystick;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.util.*;

import org.pmw.tinylog.Logger;

public class JoystickNative {
	private static final String LIB_NAME = "sdljoystick";
	private static Boolean loaded = Boolean.FALSE;
	private static Map<Integer, Joystick> joysticks;
	
	private static void init() {
		synchronized (loaded) {
			if (!loaded.booleanValue()) {
				try {
					Path path = Files.createTempFile("lib" + LIB_NAME, ".so");
					path.toFile().deleteOnExit();
					Files.copy(Joystick.class.getResourceAsStream("/lib/lib" + LIB_NAME + ".so"),
							path, StandardCopyOption.REPLACE_EXISTING);
					System.load(path.toString());
				} catch (IOException e) {
					Logger.error(e, "Error loading library from classpath: {}", e);
					
					// Try load the usual way...
					System.loadLibrary(LIB_NAME);
				}
				
				if (initialise() < 0) {
					throw new RuntimeException("Error initialising SDL Joystick library");
				}
				
				loaded = Boolean.TRUE;
				
				joysticks = new HashMap<>();
				
				Runtime.getRuntime().addShutdownHook(new Thread(() -> { Logger.info("Terminating"); terminate(); }));
			}
		}
	}
	
	static {
		init();
	}
	
	private static native int initialise();
	private static native void terminate();
	private static native Joystick openJoystick(int id);
	private static native void closeJoystick(int id);
	public static native JoystickEvent waitForEvent();
	private static native String[] listJoysticksNative();
	
	public static native int getNumJoysticks();
	public static native int getAxisValue(int id, int axis);
	public static native Ball getBallValue(int id, int ball);
	public static native int getButtonValue(int id, int button);
	public static native int getHatValue(int id, int hat);
	
	public static Joystick getJoystick(int id) {
		Joystick joystick;
		synchronized (joysticks) {
			joystick = joysticks.get(Integer.valueOf(id));
			if (joystick != null) {
				return joystick;
			}
			
			Logger.debug("Calling openJoystick({})", Integer.valueOf(id));
			joystick = openJoystick(id);
			Logger.debug("After openJoystick({})", Integer.valueOf(id));
			if (joystick == null) {
				return null;
			}
			
			Logger.debug("Adding joystick {} to hashmap", Integer.valueOf(id));
			joysticks.put(Integer.valueOf(id), joystick);
		}
		
		return joystick;
	}
	
	static void closeJoystick(Joystick joystick) {
		Logger.debug("Removing joystick {} from hashmap", Integer.valueOf(joystick.getId()));
		joysticks.remove(Integer.valueOf(joystick.getId()));
		closeJoystick(joystick.getId());
	}
	
	public static void processEvents() {
		while (true) {
			JoystickEvent event = waitForEvent();
			if (event == null) {
				Logger.warn("Got a null event");
			} else {
				Joystick joystick = joysticks.get(Integer.valueOf(event.getJoystickId()));
				if (joystick == null) {
					Logger.error("Couldn't find joystick for id {}", Integer.valueOf(event.getJoystickId()));
				} else {
					joystick.newEvent(event);
				}
			}
		}
	}
	
	public static List<JoystickInfo> listJoysticks() {
		String[] j_names = listJoysticksNative();
		List<JoystickInfo> j_infos = new ArrayList<>();
		if (j_names == null) {
			return j_infos;
		}
		
		for (int i=0; i<j_names.length; i++) {
			j_infos.add(new JoystickInfo(i, j_names[i]));
		}
		
		return j_infos;
	}
}
