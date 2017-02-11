#define __int64 long long
#include <jni.h>

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_initialise(JNIEnv*, jclass);
JNIEXPORT void JNICALL Java_com_diozero_sdl_joystick_JoystickNative_terminate(JNIEnv*, jclass);
JNIEXPORT jobject JNICALL Java_com_diozero_sdl_joystick_JoystickNative_openJoystick(JNIEnv*, jclass, int);
JNIEXPORT void JNICALL Java_com_diozero_sdl_joystick_JoystickNative_closeJoystick(JNIEnv*, jclass, int);
JNIEXPORT jobject JNICALL Java_com_diozero_sdl_joystick_JoystickNative_waitForEvent(JNIEnv*, jclass);
JNIEXPORT jobjectArray JNICALL Java_com_diozero_sdl_joystick_JoystickNative_listJoysticksNative(JNIEnv*, jclass);

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getNumJoysticks(JNIEnv*, jclass);
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getAxisValue(JNIEnv*, jclass, int, int);
JNIEXPORT jobject JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getBallValue(JNIEnv*, jclass, int, int);
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getButtonValue(JNIEnv*, jclass, int, int);
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getHatValue(JNIEnv*, jclass, int, int);
