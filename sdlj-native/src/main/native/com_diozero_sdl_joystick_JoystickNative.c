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

#define JOYSTICK_MAXSTICKS 32
static SDL_Joystick* joystick_stickdata[JOYSTICK_MAXSTICKS] = {NULL};

/* Java VM interface */
static JavaVM* globalJavaVM = NULL;

/* The VM calls this function upon loading the native library. */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void* reserved) {
	globalJavaVM = jvm;

	return JNI_VERSION_1_8;
}

/* This function is called when the native library gets unloaded by the VM. */
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* jvm, void* reserved) {
	globalJavaVM = NULL;
}

JavaVM* getGlobalJavaVM() {
	return globalJavaVM;
}

void throwException(JNIEnv* env, const char* exception, const char* message /*= NULL*/) {
	jclass clazz = (*env)->FindClass(env, exception);
	if (clazz != NULL) {
		(*env)->ThrowNew(env, clazz, message);
		(*env)->DeleteLocalRef(env, clazz);
	}
}

void throwIOException(JNIEnv* env, const char* message /*= NULL*/) {
	throwException(env, "java/io/IOException", message);
}

void throwRuntimeException(JNIEnv* env, const char* message /*= NULL*/) {
	throwException(env, "java/lang/RuntimeException", message);
}

void throwIllegalArgumentException(JNIEnv* env, const char* message /*= NULL*/) {
	throwException(env, "java/lang/IllegalArgumentException", message);
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_initialise(JNIEnv* env, jclass clz) {
	if (SDL_WasInit(SDL_INIT_EVENTS) == 0) {
		if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0) {
			printf("Unable to initialise SDL Events: %s\n", SDL_GetError());
			return -1;
		}
	}
	if (SDL_WasInit(SDL_INIT_JOYSTICK) == 0) {
		if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
			printf("Unable to initialise SDL Joystick: %s\n", SDL_GetError());
			return -1;
		}
		SDL_JoystickEventState(SDL_ENABLE);
	}

	return 0;
}

JNIEXPORT void JNICALL Java_com_diozero_sdl_joystick_JoystickNative_terminate(JNIEnv* env, jclass clz) {
	int i;
	for (i = 0; i < JOYSTICK_MAXSTICKS; i++) {
		if (joystick_stickdata[i] != NULL) {
			SDL_JoystickClose(joystick_stickdata[i]);
			joystick_stickdata[i] = NULL;
		}
	}

	if (SDL_WasInit(SDL_INIT_JOYSTICK) != 0) {
		SDL_JoystickEventState(SDL_IGNORE);
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	}

	if (SDL_WasInit(SDL_INIT_EVENTS) != 0) {
		SDL_QuitSubSystem(SDL_INIT_EVENTS);
	}
}

JNIEXPORT jobjectArray JNICALL Java_com_diozero_sdl_joystick_JoystickNative_listJoysticksNative(JNIEnv* env, jclass clz) {
	int num_joysticks = SDL_NumJoysticks();

	if (num_joysticks == 0) {
		printf("No joysticks found\n");
		return NULL;
	}
	printf("Found %d joysticks\n", num_joysticks);

	jobjectArray joystick_names = (*env)->NewObjectArray(env, num_joysticks,
			(*env)->FindClass(env, "java/lang/String"), NULL);
	int i;
	for (i=0; i<num_joysticks; i++) {
		printf("Name: %s\n", SDL_JoystickNameForIndex(i));
		(*env)->SetObjectArrayElement(env, joystick_names, i, (*env)->NewStringUTF(env, SDL_JoystickNameForIndex(i)));

		printf("GUID: ");
		int j;
		for (j=0; j<16; j++) {
			printf("%u", SDL_JoystickGetDeviceGUID(i).data[j]);
		}
		printf("\n");
	}

	return joystick_names;
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getNumJoysticks(JNIEnv* env, jclass clz) {
	return SDL_NumJoysticks();
}

JNIEXPORT jobject JNICALL Java_com_diozero_sdl_joystick_JoystickNative_openJoystick(JNIEnv* env, jclass clz, int id) {
	SDL_Joystick* joystick = SDL_JoystickOpen(id);
	if (joystick == NULL) {
		printf("Couldn't open joystick %d\n", id);
		return NULL;
	}
	joystick_stickdata[id] = joystick;

	jclass joystick_class;
	jmethodID joystick_constructor;

	char* joystick_class_name = "com/diozero/sdl/joystick/Joystick";
	joystick_class = (*env)->FindClass(env, joystick_class_name);
	if (joystick_class == NULL) {
		printf("Error, could not find class '%s'\n", joystick_class_name);
		return NULL;
	}
	char* joystick_signature = "(ILjava/lang/String;IIII)V";
	joystick_constructor = (*env)->GetMethodID(env, joystick_class, "<init>", joystick_signature);
	if (joystick_constructor == NULL) {
		printf("Error, could not find constructor %s %s\n", joystick_class_name, joystick_signature);
		return NULL;
	}

	return (*env)->NewObject(env, joystick_class, joystick_constructor,
			id, (*env)->NewStringUTF(env, SDL_JoystickName(joystick)),
			SDL_JoystickNumAxes(joystick), SDL_JoystickNumBalls(joystick),
			SDL_JoystickNumButtons(joystick), SDL_JoystickNumHats(joystick));
}

JNIEXPORT void JNICALL Java_com_diozero_sdl_joystick_JoystickNative_closeJoystick(JNIEnv* env, jclass clz, int id) {
	SDL_Joystick* joystick = joystick_stickdata[id];
	if (joystick == NULL) {
		printf("Joystick %d not opened\n", id);
		// Ignore
		return;
	}

	// Close if opened
	if (SDL_JoystickGetAttached(joystick) == SDL_TRUE) {
		SDL_JoystickClose(joystick);
	}
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getAxisValue(JNIEnv* env, jclass clz, int id, int axis) {
	SDL_Joystick* joystick = joystick_stickdata[id];
	if (joystick == NULL) {
		printf("Joystick %d not opened\n", id);
		// Raise error
		throwIllegalArgumentException(env, "Joystick not opened");
		return -1;
	}

	if (axis < 0 || axis >= SDL_JoystickNumAxes(joystick)) {
		printf("Invalid axis %d\n", axis);
		// Raise error
		throwIllegalArgumentException(env, "Invalid Joystick axis");
		return -1;
	}

	SDL_JoystickUpdate();
	return SDL_JoystickGetAxis(joystick, axis);
}

JNIEXPORT jobject JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getBallValue(JNIEnv* env, jclass clz, int id, int ball) {
	SDL_Joystick* joystick = joystick_stickdata[id];
	if (joystick == NULL) {
		printf("Joystick %d not opened\n", id);
		// Raise error
		throwIllegalArgumentException(env, "Joystick not opened");
		return NULL;
	}

	if (ball < 0 || ball >= SDL_JoystickNumBalls(joystick)) {
		printf("Invalid ball %d\n", ball);
		// Raise error
		throwIllegalArgumentException(env, "Invalid Joystick ball");
		return NULL;
	}
	int dx, dy;
	SDL_JoystickUpdate();
	SDL_JoystickGetBall(joystick, ball, &dx, &dy);

	jclass ball_class;
	jmethodID ball_constructor;
	char* ball_class_name = "com/diozero/sdl/joystick/Ball";
	ball_class = (*env)->FindClass(env, ball_class_name);
	if (ball_class == NULL) {
		printf("Error, could not find class '%s'\n", ball_class_name);
		return NULL;
	}
	char* ball_signature = "(II)V";
	ball_constructor = (*env)->GetMethodID(env, ball_class, "<init>", ball_signature);
	if (ball_constructor == NULL) {
		printf("Error, could not find constructor %s %s\n", ball_class_name, ball_signature);
		return NULL;
	}

	return (*env)->NewObject(env, ball_class, ball_constructor, dx, dy);
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getButtonValue(JNIEnv* env, jclass clz, int id, int button) {
	SDL_Joystick* joystick = joystick_stickdata[id];
	if (joystick == NULL) {
		printf("Joystick %d not opened\n", id);
		// Raise error
		throwIllegalArgumentException(env, "Joystick not opened");
		return -1;
	}

	if (button < 0 || button >= SDL_JoystickNumButtons(joystick)) {
		printf("Invalid button %d\n", button);
		// Raise error
		throwIllegalArgumentException(env, "Invalid Joystick button");
		return -1;
	}

	SDL_JoystickUpdate();
	return SDL_JoystickGetButton(joystick, button);
}

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getHatValue(JNIEnv* env, jclass clz, int id, int hat) {
	SDL_Joystick* joystick = joystick_stickdata[id];
	if (joystick == NULL) {
		printf("Joystick %d not opened\n", id);
		// Raise error
		throwIllegalArgumentException(env, "Joystick not opened");
		return -1;
	}

	if (hat < 0 || hat >= SDL_JoystickNumHats(joystick)) {
		printf("Invalid hat %d\n", hat);
		// Raise error
		throwIllegalArgumentException(env, "Invalid Joystick hat");
		return -1;
	}

	SDL_JoystickUpdate();
	return SDL_JoystickGetHat(joystick, hat);
}

jobject createJDeviceEvent(JNIEnv* env, SDL_JoyDeviceEvent jDeviceEvent) {
	char* device_event_class_name = "com/diozero/sdl/joystick/JoystickEvent$DeviceEvent";
	jclass device_event_class = (*env)->FindClass(env, device_event_class_name);
	if (device_event_class == NULL) {
		printf("Error, could not find class '%s'\n", device_event_class_name);
		return NULL;
	}
	char* device_event_signature = "(IIZ)V";
	jmethodID device_event_constructor = (*env)->GetMethodID(env,
			device_event_class, "<init>", device_event_signature);
	if (device_event_constructor == NULL) {
		printf("Error, could not find constructor %s %s\n",
				device_event_class_name, device_event_signature);
		return NULL;
	}

	return (*env)->NewObject(env, device_event_class,
			device_event_constructor, jDeviceEvent.timestamp,
			jDeviceEvent.which, jDeviceEvent.type == SDL_JOYDEVICEADDED);
}

jobject createAxisMotionEvent(JNIEnv* env, SDL_JoyAxisEvent jAxisEvent) {
	char* axis_motion_event_class_name = "com/diozero/sdl/joystick/JoystickEvent$AxisMotionEvent";
	jclass axis_motion_event_class = (*env)->FindClass(env, axis_motion_event_class_name);
	if (axis_motion_event_class == NULL) {
		printf("Error, could not find class '%s'\n",
				axis_motion_event_class_name);
		return NULL;
	}
	char* axis_motion_event_signature = "(IIII)V";
	jmethodID axis_motion_event_constructor = (*env)->GetMethodID(env,
			axis_motion_event_class, "<init>", axis_motion_event_signature);
	if (axis_motion_event_constructor == NULL) {
		printf("Error, could not find constructor %s %s\n",
				axis_motion_event_class_name, axis_motion_event_signature);
		return NULL;
	}

	return (*env)->NewObject(env, axis_motion_event_class,
			axis_motion_event_constructor, jAxisEvent.timestamp,
			jAxisEvent.which, jAxisEvent.axis,
			jAxisEvent.value);
}

jobject createBallMotionEvent(JNIEnv* env, SDL_JoyBallEvent jBallEvent) {
	char* ball_motion_event_class_name = "com/diozero/sdl/joystick/JoystickEvent$BallMotionEvent";
	jclass ball_motion_event_class = (*env)->FindClass(env, ball_motion_event_class_name);
	if (ball_motion_event_class == NULL) {
		printf("Error, could not find class '%s'\n", ball_motion_event_class_name);
		return NULL;
	}
	char* ball_motion_event_signature = "(IIIII)V";
	jmethodID ball_motion_event_constructor = (*env)->GetMethodID(env,
			ball_motion_event_class, "<init>", ball_motion_event_signature);
	if (ball_motion_event_constructor == NULL) {
		printf("Error, could not find constructor %s %s\n",
				ball_motion_event_class_name, ball_motion_event_signature);
		return NULL;
	}

	return (*env)->NewObject(env, ball_motion_event_class,
			ball_motion_event_constructor, jBallEvent.timestamp,
			jBallEvent.which, jBallEvent.ball,
			jBallEvent.xrel, jBallEvent.yrel);
}

jobject createButtonEvent(JNIEnv* env, SDL_JoyButtonEvent jButtonEvent) {
	char* button_event_class_name = "com/diozero/sdl/joystick/JoystickEvent$ButtonEvent";
	jclass button_event_class = (*env)->FindClass(env, button_event_class_name);
	if (button_event_class == NULL) {
		printf("Error, could not find class '%s'\n", button_event_class_name);
		return NULL;
	}
	char* button_event_signature = "(IIIZ)V";
	jmethodID button_event_constructor = (*env)->GetMethodID(env,
			button_event_class, "<init>", button_event_signature);
	if (button_event_constructor == NULL) {
		printf("Error, could not find constructor %s %s\n",
				button_event_class_name, button_event_signature);
		return NULL;
	}

	return (*env)->NewObject(env, button_event_class,
			button_event_constructor, jButtonEvent.timestamp,
			jButtonEvent.which, jButtonEvent.button,
			jButtonEvent.state == SDL_PRESSED);
}

jobject createHatMotionEvent(JNIEnv* env, SDL_JoyHatEvent jHatEvent) {
	char* hat_motion_event_class_name = "com/diozero/sdl/joystick/JoystickEvent$HatMotionEvent";
	jclass hat_motion_event_class = (*env)->FindClass(env, hat_motion_event_class_name);
	if (hat_motion_event_class == NULL) {
		printf("Error, could not find class '%s'\n", hat_motion_event_class_name);
		return NULL;
	}
	char* hat_motion_event_signature = "(IIII)V";
	jmethodID hat_motion_event_constructor = (*env)->GetMethodID(env,
			hat_motion_event_class, "<init>", hat_motion_event_signature);
	if (hat_motion_event_constructor == NULL) {
		printf("Error, could not find constructor %s %s\n",
				hat_motion_event_class_name, hat_motion_event_signature);
		return NULL;
	}

	return (*env)->NewObject(env, hat_motion_event_class,
			hat_motion_event_constructor, jHatEvent.timestamp,
			jHatEvent.which, jHatEvent.hat, jHatEvent.value);
}

JNIEXPORT jobject JNICALL Java_com_diozero_sdl_joystick_JoystickNative_waitForEvent(JNIEnv* env, jclass clz) {
	SDL_Event sdl_event;
	if (SDL_WaitEvent(&sdl_event) == 0) {
		printf("Error waiting for next event: %s\n", SDL_GetError());
		return NULL;
	}

	jobject event;
	switch (sdl_event.type) {
	case SDL_JOYDEVICEADDED:
	case SDL_JOYDEVICEREMOVED:
		event = createJDeviceEvent(env, sdl_event.jdevice);
		break;
	case SDL_JOYAXISMOTION:
		event = createAxisMotionEvent(env, sdl_event.jaxis);
		break;
	case SDL_JOYBALLMOTION:
		event = createBallMotionEvent(env, sdl_event.jball);
		break;
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		event = createButtonEvent(env, sdl_event.jbutton);
		break;
	case SDL_JOYHATMOTION:
		event = createHatMotionEvent(env, sdl_event.jhat);
		break;
	default:
		printf("Unhandled event type %d\n", sdl_event.type);
		event = NULL;
	}

	return event;
}
