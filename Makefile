# Define required macros here
SHELL = /bin/sh

export PKG_CONFIG_PATH = /usr/local/lib/pkgconfig/

SRC = ${wildcard ./src/*.c ./src/vendor/glad/*.c ./src/vendor/stbi/*.c}
OBJS = ${patsubst %.c,build/%.o,${SRC}}
CFLAGS = -Wall -g
CC = gcc
INCLUDES = -I "src/vendor" `pkg-config --cflags glfw3 gl`
LIBS = -Llib -lcglm `pkg-config --static --libs --cflags glfw3 gl`

bin:${OBJS}
	@echo "Linking..."
	@${CC} ${CFLAGS} ${INCLUDES} -o $@ ${OBJS} ${LIBS}
	@echo "Built"

clean :
	rm -rf main build

build/%.o: %.c
	@echo "Compiling $<..."
	@mkdir -p ${dir $@}
	@${CC} -o $@ ${CFLAGS} ${INCLUDES} -c $<