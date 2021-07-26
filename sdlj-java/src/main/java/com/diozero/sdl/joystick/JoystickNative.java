package com.diozero.sdl.joystick;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicBoolean;

import org.tinylog.Logger;

@SuppressWarnings("resource")
public class JoystickNative {
	private static final int SDL_QUIT = 0x100;
	private static final String LIB_NAME = "sdljoystick";
	private static AtomicBoolean loaded = new AtomicBoolean();
	private static Map<Integer, Joystick> joysticks;
	private static AtomicBoolean eventLoopRunning = new AtomicBoolean();
	private static String compiledVersion;
	private static String linkedVersion;

	private synchronized static void init() {
		if (!loaded.get()) {
			if (!LibraryUtil.loadLibrary(LIB_NAME, JoystickNative.class)) {
				throw new RuntimeException("Error loading SDL Joystick native library");
			}

			loaded.set(true);

			if (initialise() < 0) {
				throw new RuntimeException("Error initialising SDL Joystick library");
			}

			compiledVersion = sdlGetCompiledVersion();
			linkedVersion = sdlGetLinkedVersion();

			// Also set as SDL_GAMECONTROLLERCONFIG
			// https://github.com/gabomdq/SDL_GameControllerDB (v2.0.10 and later)
			// FIXME Unpack the version string...
			if (!linkedVersion.equals("2.0.9")) {
				try (InputStream is = JoystickNative.class.getResourceAsStream("/gamecontrollerdb.txt")) {
					if (is != null) {
						Path path = Files.createTempFile("gamecontrollerdb", "txt");
						path.toFile().deleteOnExit();
						Files.copy(is, path, StandardCopyOption.REPLACE_EXISTING);
						if (addGameControllerMappingsFromFile(path.toString()) < 0) {
							Logger.warn("Failed to load controller mappings");
						}
						path.toFile().delete();
					}
				} catch (IOException e) {
					Logger.warn(e, "Can't load game controller db file: {}", e);
				}
			}

			joysticks = new ConcurrentHashMap<>();

			Runtime.getRuntime().addShutdownHook(new Thread(JoystickNative::shutdown));
		}

	}

	static {
		init();
	}

	static native int initialise();

	static native String sdlGetCompiledVersion();

	static native String sdlGetLinkedVersion();

	static native int addGameControllerMappingsFromFile(String mappingsFile);

	static native void terminate();

	public static native int getNumJoysticks();

	static native String[] getJoystickNames();

	static native SdlEvent waitForEvent();

	static native void stopSdlEvents();

	static native int getDeviceType(int deviceIndex);

	static native boolean isGameController(int deviceIndex);

	static native Joystick openJoystick(int deviceIndex);

	static native boolean isJoystickAttached(long joystickPointer);

	static native int getCurrentPowerLevel(long joystickPointer);

	// Added in v2.0.14
	static native boolean hasLed(long joystickPointer);

	// Added in v2.0.14
	static native int setLed(long joystickPointer, int red, int green, int blue);

	static native int getAxisValue(long joystickPointer, int axis);

	static native Ball getBallValue(long joystickPointer, int ball);

	static native int getButtonValue(long joystickPointer, int button);

	static native int getHatValue(long joystickPointer, int hat);

	static native int rumble(long joystickPointer, int lowFrequencyRumble, int highFrequencyRumble, long durationMs);

	// Added in v2.0.14
	static native int rumbleTriggers(long joystickPointer, int leftRumble, int rightRumble, long durationMs);

	static native void closeJoystick(long joystickPointer);

	static native GameController openGameController(int deviceIndex);

	static native boolean isGameControllerAttached(long gameControllerPointer);

	static native void closeGameController(long gameControllerPointer);

	static void shutdown() {
		Logger.info("shutdown");
		stopEventLoop();
		joysticks.values().forEach(Joystick::close);
		joysticks.clear();
		terminate();
	}

	public static String getCompiledVersion() {
		return compiledVersion;
	}

	public static String getLinkedVersion() {
		return linkedVersion;
	}

	public static Joystick getJoystickOrGameController(int id) {
		Joystick joystick;
		synchronized (joysticks) {
			joystick = joysticks.get(Integer.valueOf(id));
			if (joystick != null) {
				return joystick;
			}

			if (isGameController(id)) {
				Logger.debug("Calling openGameController({})", Integer.valueOf(id));
				joystick = openGameController(id);
				Logger.debug("After openGameController({})", Integer.valueOf(id));
			} else {
				Logger.debug("Calling openJoystick({})", Integer.valueOf(id));
				joystick = openJoystick(id);
				Logger.debug("After openJoystick({})", Integer.valueOf(id));
			}

			if (joystick != null) {
				Logger.debug("Adding joystick {} to hashmap", Integer.valueOf(id));
				joysticks.put(Integer.valueOf(id), joystick);
			}
		}

		return joystick;
	}

	static void closeJoystick(Joystick joystick) {
		Logger.debug("Removing joystick {} from hashmap", Integer.valueOf(joystick.getId()));
		joysticks.remove(Integer.valueOf(joystick.getId()));
		closeJoystick(joystick.getJoystickPointer());
	}

	static void closeGameController(GameController gameController) {
		Logger.debug("Removing joystick {} from hashmap", Integer.valueOf(gameController.getId()));
		joysticks.remove(Integer.valueOf(gameController.getId()));
		closeGameController(gameController.getGameControllerPointer());
	}

	public static void processEvents() {
		eventLoopRunning.set(true);
		while (eventLoopRunning.get()) {
			SdlEvent event = waitForEvent();
			if (event == null || event.getType() == SDL_QUIT) {
				Logger.warn("Got a null / quit event");
				eventLoopRunning.set(false);
			} else {
				if (event.isJoystickEvent()) {
					JoystickEvent j_ev = event.toJoystickEvent();
					Joystick joystick = joysticks.get(Integer.valueOf(j_ev.getJoystickId()));
					if (joystick == null) {
						Logger.error("Couldn't find joystick for id {}", Integer.valueOf(j_ev.getJoystickId()));
					} else {
						joystick.newEvent(j_ev);
					}
				}
			}
		}
		Logger.info("processEvents - DONE");
	}

	public static void stopEventLoop() {
		Logger.info("stopEventLoop");
		eventLoopRunning.set(false);
		stopSdlEvents();
	}

	public static List<JoystickInfo> listJoysticks() {
		String[] j_names = getJoystickNames();
		List<JoystickInfo> j_infos = new ArrayList<>();
		if (j_names == null) {
			return j_infos;
		}

		for (int i = 0; i < j_names.length; i++) {
			j_infos.add(new JoystickInfo(i, j_names[i], getDeviceType(i), isGameController(i)));
		}

		return j_infos;
	}
}
