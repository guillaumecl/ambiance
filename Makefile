LIBS=sndfile openal

NAME=ambiance

#CFLAGS=-DDEBUG -g

LDFLAGS=-lm

include base.mk

install:${BIN}
	install -D -m 0755 ${BIN} ${INSTALL_DIR}/bin/${NAME}
