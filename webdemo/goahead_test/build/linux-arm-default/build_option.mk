SHARED := -shared -o
C_FLAGS := -g -O3 -fPIC -pipe -fno-strict-aliasing \
			-Wwrite-strings -march=armv7-a -mfloat-abi=hard -mfpu=neon \
			-Wno-deprecated-declarations -DLINUX_OS
BIN_FLAGS := -g -O3 -pipe -fno-strict-aliasing -Wwrite-strings -lstdc++ \
		-march=armv7-a -mfloat-abi=hard -mfpu=neon -fpermissive
		
LOCAL_CFLAGS :=
