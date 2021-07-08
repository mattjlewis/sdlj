package com.diozero.sdl.joystick;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicBoolean;

import org.tinylog.Logger;

public class JoystickNative {
	private static final String LIB_NAME = "sdljoystick";
	private static AtomicBoolean loaded = new AtomicBoolean();
	private static Map<Integer, Joystick> joysticks;
	private static AtomicBoolean eventLoopRunning = new AtomicBoolean();

	private synchronized static void init() {
		if (!loaded.get()) {
			if (!LibraryUtil.loadLibrary(LIB_NAME, JoystickNative.class)) {
				throw new RuntimeException("Error loading SDL Joystick native library");
			}

			loaded.set(true);

			if (initialise() < 0) {
				throw new RuntimeException("Error initialising SDL Joystick library");
			}

			joysticks = new ConcurrentHashMap<>();

			Runtime.getRuntime().addShutdownHook(new Thread(JoystickNative::shutdown));
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

	static void shutdown() {
		stopEventLoop();
		joysticks.values().forEach(JoystickNative::closeJoystick);
		joysticks.clear();
		terminate();
	}

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
		while (eventLoopRunning.get()) {
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

	public static void stopEventLoop() {
		eventLoopRunning.set(false);
	}

	public static List<JoystickInfo> listJoysticks() {
		String[] j_names = listJoysticksNative();
		List<JoystickInfo> j_infos = new ArrayList<>();
		if (j_names == null) {
			return j_infos;
		}

		for (int i = 0; i < j_names.length; i++) {
			j_infos.add(new JoystickInfo(i, j_names[i]));
		}

		return j_infos;
	}
}
