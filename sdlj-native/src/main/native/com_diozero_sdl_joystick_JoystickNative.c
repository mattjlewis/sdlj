/*
 * Test.c
 *
 *  Created on: 15 May 2016
 *      Author: MATTLEWI
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <SDL.h>
#include <SDL_joystick.h>

#include "com_diozero_sdl_joystick_JoystickNative.h"

static jint JNI_VERSION = JNI_VERSION_1_8;

#if __WORDSIZE == 32
#define long_t uint32_t
#else
#define long_t uint64_t
#endif

jclass sdlEventClassRef = NULL;
jmethodID sdlEventConstructor = NULL;
jclass joystickClassRef = NULL;
jmethodID joystickConstructor = NULL;
jclass gameControllerClassRef = NULL;
jmethodID gameControllerConstructor = NULL;
jclass ballClassRef = NULL;
jmethodID ballConstructor = NULL;

/* The VM calls this function upon loading the native library. */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void* reserved) {
	JNIEnv* env;
	if ((*jvm)->GetEnv(jvm, (void **) &env, JNI_VERSION) != JNI_OK) {
		fprintf(stderr, "Error, unable to get JNIEnv\n");
		return JNI_ERR;
	}

	// Cache the Joystick class and constructor on startup
	char* class_name = "com/diozero/sdl/joystick/SdlEvent";
	jclass sdl_event_class = (*env)->FindClass(env, class_name);
	if (sdl_event_class == NULL) {
		printf("Error, could not find class '%s'\n", class_name);
		return JNI_ERR;
	}
	char* method_name = "<init>";
	char* signature = "([B)V";
	sdlEventConstructor = (*env)->GetMethodID(env, sdl_event_class, method_name, signature);
	if (sdlEventConstructor == NULL) {
		printf("Error looking up methodID for %s.%s%s\n", class_name, method_name, signature);
		return JNI_ERR;
	}

	class_name = "com/diozero/sdl/joystick/Joystick";
	jclass joystick_class = (*env)->FindClass(env, class_name);
	if (joystick_class == NULL) {
		printf("Error, could not find class '%s'\n", class_name);
		return JNI_ERR;
	}
	method_name = "<init>";
	signature = "(ILjava/lang/String;IZIJZIIII)V";
	joystickConstructor = (*env)->GetMethodID(env, joystick_class, method_name, signature);
	if ((*env)->ExceptionCheck(env) || joystickConstructor == NULL) {
		fprintf(stderr, "Error looking up methodID for %s.%s%s\n", class_name, method_name, signature);
		return JNI_ERR;
	}

	class_name = "com/diozero/sdl/joystick/GameController";
	jclass game_controller_class = (*env)->FindClass(env, class_name);
	if (game_controller_class == NULL) {
		printf("Error, could not find class '%s'\n", class_name);
		return JNI_ERR;
	}
	method_name = "<init>";
	signature = "(ILjava/lang/String;IJZIIIIJLjava/lang/String;Ljava/lang/String;)V";
	gameControllerConstructor = (*env)->GetMethodID(env, game_controller_class, method_name, signature);
	if ((*env)->ExceptionCheck(env) || gameControllerConstructor == NULL) {
		fprintf(stderr, "Error looking up methodID for %s.%s%s\n", class_name, method_name, signature);
		return JNI_ERR;
	}

	class_name = "com/diozero/sdl/joystick/Ball";
	jclass ball_class = (*env)->FindClass(env, class_name);
	if (ball_class == NULL) {
		printf("Error, could not find class '%s'\n", class_name);
		return JNI_ERR;
	}
	method_name = "<init>";
	signature = "(II)V";
	ballConstructor = (*env)->GetMethodID(env, ball_class, method_name, signature);
	if (ballConstructor == NULL) {
		printf("Error looking up methodID for %s.%s%s\n", class_name, method_name, signature);
		return JNI_ERR;
	}

	//

	/*
	 * https://stackoverflow.com/questions/10617735/in-jni-how-do-i-cache-the-class-methodid-and-fieldids-per-ibms-performance-r
	 * Class IDs must be registered as global references to maintain the viability
	 * of any associated Method ID / Field IDs. If this isn't done and the class
	 * is unloaded from the JVM, on class reload, the Method IDs / Field IDs may
	 * be different. If the Class ID is registered as a global reference, the
	 * associated Method IDs and Field IDs do not need to be registered as global
	 * references. Registering a Class ID as a global reference prevents the
	 * associated Java class from unloading, therefore stabilising the Method ID
	 * / Field ID values. Global references, including the Class IDs should be
	 * removed in JNI_OnUnload().
	 */

	// Create global references to the classes
	sdlEventClassRef = (*env)->NewGlobalRef(env, sdl_event_class);
	(*env)->DeleteLocalRef(env, sdl_event_class);
	joystickClassRef = (*env)->NewGlobalRef(env, joystick_class);
	(*env)->DeleteLocalRef(env, joystick_class);
	gameControllerClassRef = (*env)->NewGlobalRef(env, game_controller_class);
	(*env)->DeleteLocalRef(env, game_controller_class);
	ballClassRef = (*env)->NewGlobalRef(env, ball_class);
	(*env)->DeleteLocalRef(env, ball_class);

	return JNI_VERSION;
}

/* This function is called when the native library gets unloaded by the VM. */
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* jvm, void* reserved) {
	JNIEnv* env;
	if ((*jvm)->GetEnv(jvm, (void **) &env, JNI_VERSION) != JNI_OK) {
		// Nothing we can do about this
		return;
	}

	if (sdlEventClassRef == NULL) {
		(*env)->DeleteGlobalRef(env, sdlEventClassRef);
		sdlEventClassRef = NULL;
	}
	if (joystickClassRef == NULL) {
		(*env)->DeleteGlobalRef(env, joystickClassRef);
		joystickClassRef = NULL;
	}
	if (gameControllerClassRef == NULL) {
		(*env)->DeleteGlobalRef(env, gameControllerClassRef);
		gameControllerClassRef = NULL;
	}
	if (ballClassRef == NULL) {
		(*env)->DeleteGlobalRef(env, ballClassRef);
		ballClassRef = NULL;
	}
}

jobject createSdlEvent(JNIEnv* env, SDL_Event sdlEvent) {
	jbyteArray byte_array = (*env)->NewByteArray(env, 56);
	(*env)->SetByteArrayRegion(env, byte_array, 0, 56, (const jbyte *) sdlEvent.padding);

	return (*env)->NewObject(env, sdlEventClassRef, sdlEventConstructor, byte_array);
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_initialise(
		JNIEnv* env, jclass clz) {
	SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI, "1");
	SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4, "1");
	SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1");

	// SDL_INIT_GAMECONTROLLER implies SDL_INIT_JOYSTICK
	// SDL_INIT_JOYSTICK implies SDL_INIT_EVENTS
	if (SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC | SDL_INIT_SENSOR) < 0) {
		printf("Unable to initialise SDL GameController: %s\n", SDL_GetError());
		return -1;
	}

	SDL_JoystickEventState(SDL_ENABLE);
	SDL_GameControllerEventState(SDL_ENABLE);

	return 0;
}

JNIEXPORT jstring JNICALL Java_com_diozero_sdl_joystick_JoystickNative_sdlGetCompiledVersion(
		JNIEnv* env, jclass clz) {
	SDL_version sdl_ver;
	SDL_VERSION(&sdl_ver);
	char* version_string;
	if (asprintf(&version_string, "%d.%d.%d", sdl_ver.major, sdl_ver.minor, sdl_ver.patch) < 0) {
		perror("Error creating compiled version string");
		return NULL;
	}

	return (*env)->NewStringUTF(env, version_string);
}

JNIEXPORT jstring JNICALL Java_com_diozero_sdl_joystick_JoystickNative_sdlGetLinkedVersion(
		JNIEnv* env, jclass clz) {
	SDL_version sdl_ver;
	SDL_GetVersion(&sdl_ver);
	char* version_string;
	if (asprintf(&version_string, "%d.%d.%d", sdl_ver.major, sdl_ver.minor, sdl_ver.patch) < 0) {
		perror("Error creating linked version string");
		return NULL;
	}

	return (*env)->NewStringUTF(env, version_string);
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_addGameControllerMappingsFromFile(
		JNIEnv* env, jclass clz, jstring mappingsFile) {
	jboolean is_copy;
	const char* str = (*env)->GetStringUTFChars(env, mappingsFile, &is_copy);
	int rc = SDL_GameControllerAddMappingsFromFile((char*) str);
	(*env)->ReleaseStringUTFChars(env, mappingsFile, str);
	return rc;
}

JNIEXPORT void JNICALL Java_com_diozero_sdl_joystick_JoystickNative_terminate(
		JNIEnv* env, jclass clz) {
	SDL_JoystickEventState(SDL_IGNORE);
	SDL_GameControllerEventState(SDL_IGNORE);

	if (SDL_WasInit(SDL_INIT_EVENTS) != 0) {
		SDL_QuitSubSystem(SDL_INIT_EVENTS);
	}

	if (SDL_WasInit(SDL_INIT_JOYSTICK) != 0) {
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	}

	if (SDL_WasInit(SDL_INIT_GAMECONTROLLER) != 0) {
		SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
	}

	SDL_Quit();
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getNumJoysticks(
		JNIEnv* env, jclass clz) {
	return SDL_NumJoysticks();
}

JNIEXPORT jobjectArray JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getJoystickNames(
		JNIEnv* env, jclass clz) {
	int num_joysticks = SDL_NumJoysticks();

	jobjectArray joystick_names = (*env)->NewObjectArray(env, num_joysticks,
			(*env)->FindClass(env, "java/lang/String"), NULL);

	if (num_joysticks > 0) {
		int i;
		for (i=0; i<num_joysticks; i++) {
			(*env)->SetObjectArrayElement(env, joystick_names, i,
					(*env)->NewStringUTF(env, SDL_JoystickNameForIndex(i)));
		}
	}

	return joystick_names;
}

JNIEXPORT jobject JNICALL Java_com_diozero_sdl_joystick_JoystickNative_waitForEvent(
		JNIEnv* env, jclass clz) {
	SDL_Event sdl_event;
	if (SDL_WaitEvent(&sdl_event) == 0) {
		return NULL;
	}

	/*
	jobject event;

	switch (sdl_event.type) {
	case SDL_JOYDEVICEADDED:
	case SDL_JOYDEVICEREMOVED:
	case SDL_CONTROLLERDEVICEADDED:
	case SDL_CONTROLLERDEVICEREMOVED:
	case SDL_CONTROLLERDEVICEREMAPPED:
		event = createDeviceEvent(env, sdl_event.cdevice.type, sdl_event.cdevice.timestamp, sdl_event.cdevice.which);
		break;
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		event = createButtonEvent(env, sdl_event.jbutton);
		break;
	case SDL_JOYAXISMOTION:
		event = createAxisMotionEvent(env, sdl_event.jaxis);
		break;
	case SDL_JOYHATMOTION:
		event = createHatMotionEvent(env, sdl_event.jhat);
		break;
	case SDL_JOYBALLMOTION:
		event = createBallMotionEvent(env, sdl_event.jball);
		break;
	// Not present in SDL2 v2.0.9
	case SDL_CONTROLLERTOUCHPADDOWN:
	case SDL_CONTROLLERTOUCHPADMOTION:
	case SDL_CONTROLLERTOUCHPADUP:
		printf("touchpad event %d\n", sdl_event.ctouchpad.type);
		event = createTouchPadEvent(env, sdl_event.ctouchpad);
		break;
	default:
		event = createSdlEvent(env, sdl_event);
	}

	return event;
	*/
	return createSdlEvent(env, sdl_event);
}


JNIEXPORT void JNICALL Java_com_diozero_sdl_joystick_JoystickNative_stopSdlEvents(
		JNIEnv* env, jclass clz) {
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getDeviceType(
		JNIEnv* env, jclass clz, jint deviceIndex) {
	return SDL_JoystickGetDeviceType(deviceIndex);
}

JNIEXPORT jboolean JNICALL Java_com_diozero_sdl_joystick_JoystickNative_isGameController(
		JNIEnv* env, jclass clz, jint deviceIndex) {
	return SDL_IsGameController(deviceIndex) == SDL_TRUE ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jobject JNICALL Java_com_diozero_sdl_joystick_JoystickNative_openJoystick(
		JNIEnv* env, jclass clz, jint deviceIndex) {
	SDL_Joystick* joystick = SDL_JoystickOpen(deviceIndex);
	if (joystick == NULL) {
		printf("Couldn't open joystick %d\n", deviceIndex);
		return NULL;
	}

	return (*env)->NewObject(env, joystickClassRef, joystickConstructor,
			deviceIndex, (*env)->NewStringUTF(env, SDL_JoystickName(joystick)),
			SDL_JoystickGetType(joystick),
			SDL_IsGameController(deviceIndex) == SDL_TRUE ? JNI_TRUE : JNI_FALSE,
			SDL_JoystickInstanceID(joystick),
			(jlong) (long_t) joystick,
			SDL_JoystickIsHaptic(joystick) == SDL_TRUE ? JNI_TRUE : JNI_FALSE,
			SDL_JoystickNumAxes(joystick), SDL_JoystickNumBalls(joystick),
			SDL_JoystickNumButtons(joystick), SDL_JoystickNumHats(joystick));
}

JNIEXPORT jboolean JNICALL Java_com_diozero_sdl_joystick_JoystickNative_isJoystickAttached(
		JNIEnv* env, jclass clz, jlong joystickPointer) {
	return SDL_JoystickGetAttached((SDL_Joystick*) (long_t) joystickPointer);
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getType(
		JNIEnv* env, jclass clz, jlong joystickPointer) {
	return SDL_JoystickGetType((SDL_Joystick*) (long_t) joystickPointer);
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getCurrentPowerLevel(
		JNIEnv* env, jclass clz, jlong joystickPointer) {
	return SDL_JoystickCurrentPowerLevel((SDL_Joystick*) (long_t) joystickPointer);
}

// Added in SDL2 v2.0.14
JNIEXPORT jboolean JNICALL Java_com_diozero_sdl_joystick_JoystickNative_hasLed(
		JNIEnv* env, jclass clz, jlong joystickPointer) {
#if SDL_MAJOR_VERSION >= 2 && SDL_PATCH_VERSION >= 14
	return SDL_JoystickHasLED((SDL_Joystick*) (long_t) joystickPointer) == SDL_TRUE ? JNI_TRUE : JNI_FALSE;
#else
	return JNI_FALSE;
#endif
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_setLed(
		JNIEnv* env, jclass clz, jlong joystickPointer, jint red, jint green, jint blue) {
#if SDL_MAJOR_VERSION >= 2 && SDL_PATCH_VERSION >= 14
	return SDL_JoystickSetLED((SDL_Joystick*) (long_t) joystickPointer,
			(Uint8) red, (Uint8) green, (Uint8) blue);
#else
	return -1;
#endif
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getAxisValue(
		JNIEnv* env, jclass clz, jlong joystickPointer, jint axis) {
	SDL_Joystick* joystick = (SDL_Joystick*) (long_t) joystickPointer;

	if (axis < 0 || axis >= SDL_JoystickNumAxes(joystick)) {
		printf("Invalid axis %d\n", axis);
		return -1;
	}

	SDL_JoystickUpdate();
	return SDL_JoystickGetAxis(joystick, axis);
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getButtonValue(
		JNIEnv* env, jclass clz, jlong joystickPointer, jint button) {
	SDL_Joystick* joystick = (SDL_Joystick*) (long_t) joystickPointer;

	if (button < 0 || button >= SDL_JoystickNumButtons(joystick)) {
		printf("Invalid button %d\n", button);
		return -1;
	}

	SDL_JoystickUpdate();
	return SDL_JoystickGetButton(joystick, button);
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getHatValue(
		JNIEnv* env, jclass clz, jlong joystickPointer, jint hat) {
	SDL_Joystick* joystick = (SDL_Joystick*) (long_t) joystickPointer;

	if (hat < 0 || hat >= SDL_JoystickNumHats(joystick)) {
		printf("Invalid hat %d\n", hat);
		return -1;
	}

	SDL_JoystickUpdate();
	return SDL_JoystickGetHat(joystick, hat);
}

JNIEXPORT jobject JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getBallValue(
		JNIEnv* env, jclass clz, jlong joystickPointer, jint ball) {
	SDL_Joystick* joystick = (SDL_Joystick*) (long_t) joystickPointer;

	if (ball < 0 || ball >= SDL_JoystickNumBalls(joystick)) {
		printf("Invalid ball %d\n", ball);
		return NULL;
	}
	int dx, dy;
	SDL_JoystickUpdate();
	SDL_JoystickGetBall(joystick, ball, &dx, &dy);

	return (*env)->NewObject(env, ballClassRef, ballConstructor, dx, dy);
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_rumble(
		JNIEnv* env, jclass clz, jlong joystickPointer, jint lowFrequencyRumble, jint highFrequencyRumble, jlong durationMs) {
	SDL_Joystick* joystick = (SDL_Joystick*) (long_t) joystickPointer;

	return SDL_JoystickRumble(joystick, (Uint16) (lowFrequencyRumble & 0xffff),
			(Uint16) (highFrequencyRumble & 0xffff), (Uint32) (durationMs & 0xffffffff));
}

// Added in SDL2 v2.0.14
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_rumbleTriggers(
		JNIEnv* env, jclass clz, jlong joystickPointer, jint leftRumble, jint rightRumble, jlong durationMs) {
#if SDL_MAJOR_VERSION >= 2 && SDL_PATCH_VERSION >= 14
	SDL_Joystick* joystick = (SDL_Joystick*) (long_t) joystickPointer;

	return SDL_JoystickRumbleTriggers(joystick, (Uint16) (leftRumble & 0xffff),
			(Uint16) (rightRumble & 0xffff), (Uint32) (durationMs & 0xffffffff));
#else
	return -1;
#endif
}

JNIEXPORT void JNICALL Java_com_diozero_sdl_joystick_JoystickNative_closeJoystick(
		JNIEnv* env, jclass clz, jlong joystickPointer) {
	printf("closeJoystick() - START\n");
	SDL_JoystickClose((SDL_Joystick*) (long_t) joystickPointer);
	printf("closeJoystick() - END\n");
}

JNIEXPORT jobject JNICALL Java_com_diozero_sdl_joystick_JoystickNative_openGameController(
		JNIEnv* env, jclass clz, jint deviceIndex) {
	SDL_GameController* game_controller = SDL_GameControllerOpen(deviceIndex);
	if (game_controller == NULL) {
		printf("Couldn't open game_controller %d: %s\n", deviceIndex, SDL_GetError());
		return NULL;
	}

	// Added in SDL2 v2.0.14
#if SDL_MAJOR_VERSION >= 2 && SDL_PATCH_VERSION >= 14
	SDL_version linked;
	SDL_GetVersion(&linked);
	if (linked.major >= 2 && linked.patch >= 14) {
		SDL_GameControllerSetSensorEnabled(game_controller, SDL_SENSOR_GYRO, SDL_TRUE);
	}
#endif

	char* gc_mapping = SDL_GameControllerMapping(game_controller);
	SDL_Joystick* joystick = SDL_GameControllerGetJoystick(game_controller);

	// TODO Remove
	if (SDL_JoystickIsHaptic(joystick) == SDL_TRUE) {
		SDL_Haptic* haptic = SDL_HapticOpenFromJoystick(joystick);
		printf("Haptic Effects: %d\n", SDL_HapticNumEffects(haptic));
		printf("Haptic Query: %x\n", SDL_HapticQuery(haptic));
		/*
		if (SDL_HapticRumbleSupported(haptic)) {
			if (SDL_HapticRumbleInit(haptic) != 0) {
				printf("Haptic Rumble init failed: %s\n", SDL_GetError());
			} else {
				if (SDL_HapticRumblePlay(haptic, 0.5, 250) != 0) {
					printf("Haptic Rumble play failed: %s\n", SDL_GetError());
				}
			}
		}
		*/
		SDL_HapticClose(haptic);
	} else {
		printf("Not a haptic device\n");
	}

	jobject obj = (*env)->NewObject(env, gameControllerClassRef, gameControllerConstructor,
			deviceIndex,
			(*env)->NewStringUTF(env, SDL_JoystickName(joystick)),
			SDL_JoystickInstanceID(joystick),
			(jlong) (long_t) joystick,
			SDL_JoystickIsHaptic(joystick) == SDL_TRUE ? JNI_TRUE : JNI_FALSE,
			SDL_JoystickNumAxes(joystick), SDL_JoystickNumBalls(joystick),
			SDL_JoystickNumButtons(joystick), SDL_JoystickNumHats(joystick),
			(jlong) (long_t) game_controller,
			(*env)->NewStringUTF(env, SDL_GameControllerName(game_controller)),
			(*env)->NewStringUTF(env, gc_mapping));

	SDL_free(gc_mapping);

	return obj;
}

JNIEXPORT jboolean JNICALL Java_com_diozero_sdl_joystick_JoystickNative_isGameControllerAttached(
		JNIEnv* env, jclass clz, jlong gameControllerPointer) {
	return SDL_GameControllerGetAttached((SDL_GameController*) (long_t) gameControllerPointer);
}

JNIEXPORT void JNICALL Java_com_diozero_sdl_joystick_JoystickNative_closeGameController(
		JNIEnv* env, jclass clz, jlong gameControllerPointer) {
	printf("closeGameController() - START\n");
	SDL_GameControllerClose((SDL_GameController*) (long_t) gameControllerPointer);
	printf("closeGameController() - END\n");
}
