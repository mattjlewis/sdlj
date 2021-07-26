FROM diozero/diozero-cc:latest

ARG HOME_DIR=/home/${RUN_AS_USER}
ARG SDL2_VERSION=2.0.14

WORKDIR ${HOME_DIR}
RUN wget https://www.libsdl.org/release/SDL2-${SDL2_VERSION}.tar.gz
RUN tar zxf SDL2-${SDL2_VERSION}.tar.gz
RUN rm SDL2-${SDL2_VERSION}.tar.gz
WORKDIR ${HOME_DIR}/SDL2-${SDL2_VERSION}

# Build for AArch64
RUN ./configure CFLAGS="-march=armv8-a" --target=aarch64-linux-gnu --host=aarch64-linux-gnu --prefix=/usr/aarch64-linux-gnu \
      --enable-arm-simd --enable-arm-neon --enable-hidapi --enable-hidapi-libusb --disable-pulseaudio --disable-esd --disable-video-wayland \
    && make -j8 && sudo make install && make clean

# Build for ARMv7
RUN ./configure CFLAGS="-march=armv7" --target=arm-linux-gnueabihf --host=arm-linux-gnueabihf --prefix=/usr/arm-linux-gnueabihf \
      --enable-arm-simd --enable-arm-neon --enable-hidapi --enable-hidapi-libusb --disable-pulseaudio --disable-esd --disable-video-wayland \
    && make -j8 && sudo make install && make clean

# Build for ARMv6
RUN PATH=${PI_GCC_TARGET_DIR}/bin:${PATH} \
    && ./configure CFLAGS="-march=armv6 -mfpu=vfp -mfloat-abi=hard" --target=arm-linux-gnueabihf --host=arm-linux-gnueabihf \
      --prefix=${PI_GCC_TARGET_DIR}/arm-linux-gnueabihf --enable-arm-simd --enable-arm-neon --enable-hidapi \
      --enable-hidapi-libusb --disable-pulseaudio --disable-esd --disable-video-wayland \
    && make -j8 && sudo make install && make clean

WORKDIR ${HOME_DIR}
RUN rm -rf SDL2-${SDL2_VERSION}
