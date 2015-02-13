LIBS=sndfile openal libxdg-basedir

NAME=ambiance

#CFLAGS=-DDEBUG -g

LDFLAGS=-lm

include base.mk

install:${BIN}
	install -D -m 0755 ${BIN} ${INSTALL_DIR}/bin/${NAME}
	install -D -m 0644 ambiance.man ${INSTALL_DIR}/share/man/man1/ambiance.1
