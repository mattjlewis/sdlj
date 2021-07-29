#define __int64 long long
#include <jni.h>

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_initialise(JNIEnv*, jclass);
JNIEXPORT jstring JNICALL Java_com_diozero_sdl_joystick_JoystickNative_sdlGetCompiledVersion(JNIEnv*, jclass);
JNIEXPORT jstring JNICALL Java_com_diozero_sdl_joystick_JoystickNative_sdlGetLinkedVersion(JNIEnv*, jclass);
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_addGameControllerMappingsFromFile(JNIEnv*, jclass, jstring);
JNIEXPORT void JNICALL Java_com_diozero_sdl_joystick_JoystickNative_terminate(JNIEnv*, jclass);

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getNumJoysticks(JNIEnv*, jclass);
JNIEXPORT jobjectArray JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getJoystickNames(JNIEnv*, jclass);
JNIEXPORT jobject JNICALL Java_com_diozero_sdl_joystick_JoystickNative_waitForEvent(JNIEnv*, jclass);
JNIEXPORT void JNICALL Java_com_diozero_sdl_joystick_JoystickNative_stopSdlEvents(JNIEnv*, jclass);

JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getDeviceType(JNIEnv*, jclass, jint);
JNIEXPORT jboolean JNICALL Java_com_diozero_sdl_joystick_JoystickNative_isGameController(JNIEnv*, jclass, jint);

JNIEXPORT jobject JNICALL Java_com_diozero_sdl_joystick_JoystickNative_openJoystick(JNIEnv*, jclass, jint);
JNIEXPORT jboolean JNICALL Java_com_diozero_sdl_joystick_JoystickNative_isJoystickAttached(JNIEnv*, jclass, jlong);
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getType(JNIEnv*, jclass, jlong);
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getCurrentPowerLevel(JNIEnv*, jclass, jlong);
// Added in SDL2 v2.0.14
JNIEXPORT jboolean JNICALL Java_com_diozero_sdl_joystick_JoystickNative_hasLed(JNIEnv*, jclass, jlong);
// Added in SDL2 v2.0.14
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_setLed(JNIEnv*, jclass, jlong, jint, jint, jint);
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getAxisValue(JNIEnv*, jclass, jlong, jint);
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getButtonValue(JNIEnv*, jclass, jlong, jint);
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getHatValue(JNIEnv*, jclass, jlong, jint);
JNIEXPORT jobject JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getBallValue(JNIEnv*, jclass, jlong, jint);
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_rumble(JNIEnv*, jclass, jlong, jint, jint, jlong);
// Added in SDL2 v2.0.14
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_rumbleTriggers(JNIEnv*, jclass, jlong, jint, jint, jlong);
JNIEXPORT void JNICALL Java_com_diozero_sdl_joystick_JoystickNative_closeJoystick(JNIEnv*, jclass, jlong);

JNIEXPORT jobject JNICALL Java_com_diozero_sdl_joystick_JoystickNative_openGameController(JNIEnv*, jclass, jint);
JNIEXPORT jboolean JNICALL Java_com_diozero_sdl_joystick_JoystickNative_isGameControllerAttached(JNIEnv*, jclass, jlong);
JNIEXPORT jint JNICALL Java_com_diozero_sdl_joystick_JoystickNative_getGameControllerBindForAxis(JNIEnv*, jclass, jlong, jint);
JNIEXPORT void JNICALL Java_com_diozero_sdl_joystick_JoystickNative_closeGameController(JNIEnv*, jclass, jlong);
