/*
 * Test.c
 *
 *  Created on: 15 May 2016
 *      Author: MATTLEWI
 */

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

/*
jclass deviceEventClassRef = NULL;
jmethodID deviceEventConstructor = NULL;
jclass buttonEventClassRef = NULL;
jmethodID buttonEventConstructor = NULL;
jclass axisMotionEventClassRef = NULL;
jmethodID axisMotionEventConstructor = NULL;
jclass hatMotionEventClassRef = NULL;
jmethodID hatMotionEventConstructor = NULL;
jclass ballMotionEventClassRef = NULL;
jmethodID ballMotionEventConstructor = NULL;
jclass touchPadEventClassRef = NULL;
jmethodID touchPadEventConstructor = NULL;
*/

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
	signature = "(ILjava/lang/String;IJZIIIIJLjava/lang/String;)V";
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

	/*
	class_name = "com/diozero/sdl/joystick/JoystickEvent$DeviceEvent";
	jclass device_event_class = (*env)->FindClass(env, class_name);
	if (device_event_class == NULL) {
		printf("Error, could not find class '%s'\n", class_name);
		return JNI_ERR;
	}
	method_name = "<init>";
	signature = "(III)V";
	deviceEventConstructor = (*env)->GetMethodID(env, device_event_class, method_name, signature);
	if (deviceEventConstructor == NULL) {
		printf("Error looking up methodID for %s.%s%s\n", class_name, method_name, signature);
		return JNI_ERR;
	}

	class_name = "com/diozero/sdl/joystick/JoystickEvent$ButtonEvent";
	jclass button_event_class = (*env)->FindClass(env, class_name);
	if (button_event_class == NULL) {
		printf("Error, could not find class '%s'\n", class_name);
		return JNI_ERR;
	}
	method_name = "<init>";
	signature = "(IIIZ)V";
	buttonEventConstructor = (*env)->GetMethodID(env, button_event_class, method_name, signature);
	if (buttonEventConstructor == NULL) {
		printf("Error looking up methodID for %s.%s%s\n", class_name, method_name, signature);
		return JNI_ERR;
	}

	class_name = "com/diozero/sdl/joystick/JoystickEvent$AxisMotionEvent";
	jclass axis_motion_event_class = (*env)->FindClass(env, class_name);
	if (axis_motion_event_class == NULL) {
		printf("Error, could not find class '%s'\n", class_name);
		return JNI_ERR;
	}
	method_name = "<init>";
	signature = "(IIII)V";
	axisMotionEventConstructor = (*env)->GetMethodID(env, axis_motion_event_class, method_name, signature);
	if (axisMotionEventConstructor == NULL) {
		printf("Error looking up methodID for %s.%s%s\n", class_name, method_name, signature);
		return JNI_ERR;
	}

	class_name = "com/diozero/sdl/joystick/JoystickEvent$HatMotionEvent";
	jclass hat_motion_event_class = (*env)->FindClass(env, class_name);
	if (hat_motion_event_class == NULL) {
		printf("Error, could not find class '%s'\n", class_name);
		return JNI_ERR;
	}
	method_name = "<init>";
	signature = "(IIII)V";
	hatMotionEventConstructor = (*env)->GetMethodID(env, hat_motion_event_class, method_name, signature);
	if (hatMotionEventConstructor == NULL) {
		printf("Error looking up methodID for %s.%s%s\n", class_name, method_name, signature);
		return JNI_ERR;
	}

	class_name = "com/diozero/sdl/joystick/JoystickEvent$BallMotionEvent";
	jclass ball_motion_event_class = (*env)->FindClass(env, class_name);
	if (ball_motion_event_class == NULL) {
		printf("Error, could not find class '%s'\n", class_name);
		return JNI_ERR;
	}
	method_name = "<init>";
	signature = "(IIIII)V";
	ballMotionEventConstructor = (*env)->GetMethodID(env, ball_motion_event_class, method_name, signature);
	if (ballMotionEventConstructor == NULL) {
		printf("Error looking up methodID for %s.%s%s\n", class_name, method_name, signature);
		return JNI_ERR;
	}

	class_name = "com/diozero/sdl/joystick/JoystickEvent$TouchPadEvent";
	jclass touch_pad_event_class = (*env)->FindClass(env, class_name);
	if (touch_pad_event_class == NULL) {
		printf("Error, could not find class '%s'\n", class_name);
		return JNI_ERR;
	}
	method_name = "<init>";
	signature = "(IIIIIFFF)V";
	touchPadEventConstructor = (*env)->GetMethodID(env, touch_pad_event_class, method_name, signature);
	if (touchPadEventConstructor == NULL) {
		printf("Error looking up methodID for %s.%s%s\n", class_name, method_name, signature);
		return JNI_ERR;
	}
	*/

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
	/*
	deviceEventClassRef = (*env)->NewGlobalRef(env, device_event_class);
	(*env)->DeleteLocalRef(env, device_event_class);
	buttonEventClassRef = (*env)->NewGlobalRef(env, button_event_class);
	(*env)->DeleteLocalRef(env, button_event_class);
	axisMotionEventClassRef = (*env)->NewGlobalRef(env, axis_motion_event_class);
	(*env)->DeleteLocalRef(env, axis_motion_event_class);
	hatMotionEventClassRef = (*env)->NewGlobalRef(env, hat_motion_event_class);
	(*env)->DeleteLocalRef(env, hat_motion_event_class);
	ballMotionEventClassRef = (*env)->NewGlobalRef(env, ball_motion_event_class);
	(*env)->DeleteLocalRef(env, ball_motion_event_class);
	touchPadEventClassRef = (*env)->NewGlobalRef(env, touch_pad_event_class);
	(*env)->DeleteLocalRef(env, touch_pad_event_class);
	*/

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

	/*
	if (deviceEventClassRef == NULL) {
		(*env)->DeleteGlobalRef(env, deviceEventClassRef);
		deviceEventClassRef = NULL;
	}
	if (buttonEventClassRef == NULL) {
		(*env)->DeleteGlobalRef(env, buttonEventClassRef);
		buttonEventClassRef = NULL;
	}
	if (axisMotionEventClassRef == NULL) {
		(*env)->DeleteGlobalRef(env, axisMotionEventClassRef);
		axisMotionEventClassRef = NULL;
	}
	if (hatMotionEventClassRef == NULL) {
		(*env)->DeleteGlobalRef(env, hatMotionEventClassRef);
		hatMotionEventClassRef = NULL;
	}
	if (ballMotionEventClassRef == NULL) {
		(*env)->DeleteGlobalRef(env, ballMotionEventClassRef);
		ballMotionEventClassRef = NULL;
	}
	if (touchPadEventClassRef == NULL) {
		(*env)->DeleteGlobalRef(env, touchPadEventClassRef);
		touchPadEventClassRef = NULL;
	}
	*/
}

/*
jobject createDeviceEvent(JNIEnv* env, Uint32 type, Uint32 timestamp, Sint32 which) {
	return (*env)->NewObject(env, deviceEventClassRef, deviceEventConstructor,
			timestamp, which, type);
}

jobject createButtonEvent(JNIEnv* env, SDL_JoyButtonEvent jButtonEvent) {
	return (*env)->NewObject(env, buttonEventClassRef, buttonEventConstructor,
			jButtonEvent.timestamp, jButtonEvent.which, jButtonEvent.button,
			jButtonEvent.state == SDL_PRESSED);
}

jobject createAxisMotionEvent(JNIEnv* env, SDL_JoyAxisEvent jAxisEvent) {
	return (*env)->NewObject(env, axisMotionEventClassRef, axisMotionEventConstructor,
			jAxisEvent.timestamp, jAxisEvent.which, jAxisEvent.axis, jAxisEvent.value);
}

jobject createHatMotionEvent(JNIEnv* env, SDL_JoyHatEvent jHatEvent) {
	return (*env)->NewObject(env, hatMotionEventClassRef, hatMotionEventConstructor,
			jHatEvent.timestamp, jHatEvent.which, jHatEvent.hat, jHatEvent.value);
}

jobject createBallMotionEvent(JNIEnv* env, SDL_JoyBallEvent jBallEvent) {
	return (*env)->NewObject(env, ballMotionEventClassRef, ballMotionEventConstructor,
			jBallEvent.timestamp, jBallEvent.which, jBallEvent.ball,
			jBallEvent.xrel, jBallEvent.yrel);
}

jobject createTouchPadEvent(JNIEnv* env, SDL_ControllerTouchpadEvent touchPadEvent) {
	return (*env)->NewObject(env, touchPadEventClassRef, touchPadEventConstructor,
			touchPadEvent.timestamp, touchPadEvent.which, touchPadEvent.type,
			touchPadEvent.touchpad, touchPadEvent.finger,
			touchPadEvent.x, touchPadEvent.y,
			touchPadEvent.pressure);
}
*/

jobject createSdlEvent(JNIEnv* env, SDL_Event sdlEvent) {
	jbyteArray byte_array = (*env)->NewByteArray(env, 56);
	(*env)->SetByteArrayRegion(env, byte_array, 0, 56, (const jbyte *) sdlEvent.padding);

	return (*env)->NewObject(env, sdlEventClassRef, sdlEventConstructor, byte_array);
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_initialise(
		JNIEnv* env, jclass clz) {
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

JNIEXPORT void JNICALL Java_com_diozero_sdl_joystick_JoystickNative_terminate(
		JNIEnv* env, jclass clz) {
	printf("terminate() - START\n");

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

	//SDL_Quit();
	printf("terminate() - END\n");
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
			(*env)->SetObjectArrayElement(env, joystick_names, i, (*env)->NewStringUTF(env, SDL_JoystickNameForIndex(i)));
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

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getDeviceType(JNIEnv* env, jclass clz, jint deviceIndex) {
	return SDL_JoystickGetDeviceType(deviceIndex);
}

JNIEXPORT jboolean JNICALL Java_com_diozero_sdl_joystick_JoystickNative_isGameController(JNIEnv* env, jclass clz, jint deviceIndex) {
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

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getType(
		JNIEnv* env, jclass clz, jlong joystickPointer) {
	SDL_Joystick* joystick = (SDL_Joystick*) (long_t) joystickPointer;

	return SDL_JoystickGetType(joystick);
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getCurrentPowerLevel(
		JNIEnv* env, jclass clz, jlong joystickPointer) {
	SDL_Joystick* joystick = (SDL_Joystick*) (long_t) joystickPointer;

	return SDL_JoystickCurrentPowerLevel(joystick);
}

/*
JNIEXPORT jboolean JNICALL Java_com_diozero_sdl_joystick_JoystickNative_hasLed(
		JNIEnv* env, jclass clz, jlong joystickPointer) {
	SDL_Joystick* joystick = (SDL_Joystick*) (long_t) joystickPointer;

	return SDL_JoystickHasLED(joystick);
}
*/

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

/*
 * Not present in SDL2 v2.0.9
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_rumbleTriggers(
		JNIEnv* env, jclass clz, jlong joystickPointer, jint leftRumble, jint rightRumble, jlong durationMs) {
	SDL_Joystick* joystick = (SDL_Joystick*) (long_t) joystickPointer;

	return SDL_JoystickRumbleTriggers(joystick, (Uint16) (leftRumble & 0xffff),
			(Uint16) (rightRumble & 0xffff), (Uint32) (durationMs & 0xffffffff));
}
*/

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

	// Not present in SDL2 v2.0.9
	//SDL_GameControllerSetSensorEnabled(game_controller, SDL_SENSOR_GYRO, SDL_TRUE);

	char* gc_mapping = SDL_GameControllerMapping(game_controller);
	SDL_Joystick* joystick = SDL_GameControllerGetJoystick(game_controller);

	if (SDL_JoystickIsHaptic(joystick)) {
		SDL_Haptic* haptic = SDL_HapticOpenFromJoystick(joystick);
		printf("Haptic Effects: %d\n", SDL_HapticNumEffects(haptic));
		printf("Haptic Query: %x\n", SDL_HapticQuery(haptic));
		if (SDL_HapticRumbleSupported(haptic)) {
			if (SDL_HapticRumbleInit(haptic) != 0) {
				printf("Haptic Rumble init failed: %s\n", SDL_GetError());
			}
		}
		SDL_HapticClose(haptic);
	} else {
		printf("Not a haptic device\n");
	}

	jobject obj = (*env)->NewObject(env, gameControllerClassRef, gameControllerConstructor,
			deviceIndex,
			(*env)->NewStringUTF(env, SDL_GameControllerName(game_controller)),
			SDL_JoystickInstanceID(joystick),
			(jlong) (long_t) joystick,
			SDL_JoystickIsHaptic(joystick) == SDL_TRUE ? JNI_TRUE : JNI_FALSE,
			SDL_JoystickNumAxes(joystick), SDL_JoystickNumBalls(joystick),
			SDL_JoystickNumButtons(joystick), SDL_JoystickNumHats(joystick),
			(jlong) (long_t) game_controller, (*env)->NewStringUTF(env, gc_mapping));

	SDL_free(gc_mapping);

	return obj;
}

JNIEXPORT void JNICALL Java_com_diozero_sdl_joystick_JoystickNative_closeGameController(
		JNIEnv* env, jclass clz, jlong gameControllerPointer) {
	printf("closeGameController() - START\n");
	SDL_GameControllerClose((SDL_GameController*) (long_t) gameControllerPointer);
	printf("closeGameController() - END\n");
}
