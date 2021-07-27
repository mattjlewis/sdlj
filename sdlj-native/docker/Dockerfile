FROM diozero/diozero-cc:latest

ARG HOME_DIR=/home/${RUN_AS_USER}

WORKDIR ${HOME_DIR}

# Install Debian Buster SDL2 (2.0.9)
RUN sudo apt-get update && sudo apt-get -y install libsdl2-2.0-0 libsdl2-dev

ARG DEBIAN_MIRROR=http://ftp.uk.debian.org/debian
ARG RASPBIAN_MIRROR=http://raspbian.raspberrypi.org/raspbian
ARG SDL2_VERSION=2.0.9+dfsg1-1
#ARG SDL2_VERSION=2.0.14+dfsg2-3
ARG LIB_SDL2=libsdl2-2.0-0_${SDL2_VERSION}
ARG LIB_SDL2_PATH=pool/main/libs/libsdl2
ARG LIB_SDL2_DEV=libsdl2-dev_${SDL2_VERSION}

# AArch64
WORKDIR ${HOME_DIR}/libsdl2/aarch64
# libsdl2
ARG DEB_FILE=${LIB_SDL2}_arm64.deb
RUN wget ${DEBIAN_MIRROR}/${LIB_SDL2_PATH}/${DEB_FILE}
RUN ar -p ${DEB_FILE} data.tar.xz | sudo tar Jxf - -C /usr/aarch64-linux-gnu/lib --strip-components=4 ./usr/lib/aarch64-linux-gnu
RUN rm ${DEB_FILE}
# libsdl2-dev
ARG DEB_FILE=${LIB_SDL2_DEV}_arm64.deb
RUN wget ${DEBIAN_MIRROR}/${LIB_SDL2_PATH}/${DEB_FILE}
RUN ar -x ${DEB_FILE} data.tar.xz
RUN rm ${DEB_FILE}
RUN sudo tar Jxf data.tar.xz -C /usr/aarch64-linux-gnu/bin --strip-components=3 ./usr/bin
RUN sudo tar Jxf data.tar.xz -C /usr/aarch64-linux-gnu/lib --strip-components=4 ./usr/lib/aarch64-linux-gnu
RUN sudo tar Jxf data.tar.xz -C /usr/aarch64-linux-gnu/include --strip-components=3 ./usr/include
RUN rm data.tar.xz

# ARMv7
WORKDIR ${HOME_DIR}/libsdl2/armv7
# libsdl2
ARG DEB_FILE=${LIB_SDL2}_armhf.deb
RUN wget ${DEBIAN_MIRROR}/${LIB_SDL2_PATH}/${DEB_FILE}
RUN ar -p ${DEB_FILE} data.tar.xz | sudo tar Jxf - -C /usr/arm-linux-gnueabihf/lib --strip-components=4 ./usr/lib/arm-linux-gnueabihf
RUN rm ${DEB_FILE}
# libsdl2-dev
ARG DEB_FILE=${LIB_SDL2_DEV}_armhf.deb
RUN wget ${DEBIAN_MIRROR}/${LIB_SDL2_PATH}/${DEB_FILE}
RUN ar -x ${DEB_FILE} data.tar.xz
RUN rm ${DEB_FILE}
RUN sudo tar Jxf data.tar.xz -C /usr/arm-linux-gnueabihf/bin --strip-components=3 ./usr/bin
RUN sudo tar Jxf data.tar.xz -C /usr/arm-linux-gnueabihf/lib --strip-components=4 ./usr/lib/arm-linux-gnueabihf
RUN sudo tar Jxf data.tar.xz -C /usr/arm-linux-gnueabihf/include --strip-components=3 ./usr/include
RUN rm data.tar.xz

# Raspbian ARMv6
WORKDIR ${HOME_DIR}/libsdl2/armv6
# libsdl2
ARG DEB_FILE=${LIB_SDL2}_armhf.deb
RUN wget ${RASPBIAN_MIRROR}/${LIB_SDL2_PATH}/${DEB_FILE}
RUN ar -p ${DEB_FILE} data.tar.xz | sudo tar Jxf - -C ${PI_GCC_TARGET_DIR}/arm-linux-gnueabihf/lib --strip-components=4 ./usr/lib/arm-linux-gnueabihf
RUN rm ${DEB_FILE}
# libsdl2-dev
ARG DEB_FILE=${LIB_SDL2_DEV}_armhf.deb
RUN wget ${RASPBIAN_MIRROR}/${LIB_SDL2_PATH}/${DEB_FILE}
RUN ar -x ${DEB_FILE} data.tar.xz
RUN rm ${DEB_FILE}
RUN sudo tar Jxf data.tar.xz -C ${PI_GCC_TARGET_DIR}/arm-linux-gnueabihf/bin --strip-components=3 ./usr/bin
RUN sudo tar Jxf data.tar.xz -C ${PI_GCC_TARGET_DIR}/arm-linux-gnueabihf/lib --strip-components=4 ./usr/lib/arm-linux-gnueabihf
RUN sudo tar Jxf data.tar.xz -C ${PI_GCC_TARGET_DIR}/arm-linux-gnueabihf/include --strip-components=3 ./usr/include
RUN rm data.tar.xz

WORKDIR ${HOME_DIR}
RUN rm -rf libsdl2

# Install SDL2 latest (2.0.14)
ARG SDL2_VERSION=2.0.14

RUN wget https://www.libsdl.org/release/SDL2-${SDL2_VERSION}.tar.gz
RUN tar zxf SDL2-${SDL2_VERSION}.tar.gz
RUN rm SDL2-${SDL2_VERSION}.tar.gz
WORKDIR ${HOME_DIR}/SDL2-${SDL2_VERSION}

# Build for x86-64
RUN ./configure --prefix=/usr \
      --enable-events --enable-joystick --enable-haptic --enable-hidapi --enable-hidapi-libusb \
      --disable-video --disable-video-vulkan --disable-render --disable-audio --disable-esd --disable-pulseaudio --disable-alsa \
    && make clean && make -j8 && sudo make install && make clean

# Build for AArch64
RUN ./configure CFLAGS="-march=armv8-a" --target=aarch64-linux-gnu --host=aarch64-linux-gnu --prefix=/usr/aarch64-linux-gnu \
      --enable-arm-simd --enable-arm-neon --enable-events --enable-joystick --enable-haptic --enable-hidapi --enable-hidapi-libusb \
      --disable-video --disable-video-vulkan --disable-render --disable-audio --disable-esd --disable-pulseaudio --disable-alsa \
    && make clean && make -j8 && sudo make install && make clean

# Build for ARMv7
RUN ./configure CFLAGS="-march=armv7" --target=arm-linux-gnueabihf --host=arm-linux-gnueabihf --prefix=/usr/arm-linux-gnueabihf \
      --enable-arm-simd --enable-arm-neon --enable-events --enable-joystick --enable-haptic --enable-hidapi --enable-hidapi-libusb \
      --disable-video --disable-video-vulkan --disable-render --disable-audio --disable-esd --disable-pulseaudio --disable-alsa \
    && make clean && make -j8 && sudo make install && make clean

# Build for ARMv6
RUN PATH=${PI_GCC_TARGET_DIR}/bin:${PATH} \
    && ./configure CFLAGS="-march=armv6 -mfpu=vfp -mfloat-abi=hard" --target=arm-linux-gnueabihf --host=arm-linux-gnueabihf \
      --prefix=${PI_GCC_TARGET_DIR}/arm-linux-gnueabihf \
      --enable-arm-simd --enable-arm-neon --enable-events --enable-joystick --enable-haptic --enable-hidapi --enable-hidapi-libusb \
      --disable-video --disable-video-vulkan --disable-render --disable-audio --disable-esd --disable-pulseaudio --disable-alsa \
    && make clean && make -j8 && sudo make install && make clean

WORKDIR ${HOME_DIR}
RUN rm -rf SDL2-${SDL2_VERSION}
