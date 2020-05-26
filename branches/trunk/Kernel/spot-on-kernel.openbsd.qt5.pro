include(spot-on-kernel-source.pro)
libntru.commands = gmake -C ../../../libNTRU
libntru.depends =
libntru.target = libntru.so
libspoton.commands = gmake -C ../../../libSpotOn library
libspoton.depends =
libspoton.target = libspoton.so
purge.commands = rm -f *~

CONFIG		+= qt release warn_on
LANGUAGE	= C++
QT		+= concurrent network sql websockets
QT		-= gui

DEFINES += SPOTON_LINKED_WITH_LIBGEOIP \
	   SPOTON_LINKED_WITH_LIBNTRU \
           SPOTON_LINKED_WITH_LIBPTHREAD \
           SPOTON_WEBSOCKETS_ENABLED

# Unfortunately, the clean target assumes too much knowledge
# about the internals of libNTRU and libSpotOn.

QMAKE_CLEAN            += ../../../libNTRU/*.so \
                          ../../../libNTRU/src/*.o \
                          ../../../libNTRU/src/*.s \
                          ../../../libSpotOn/*.o \
                          ../../../libSpotOn/*.so \
                          ../../../libSpotOn/test \
                          ../Spot-On-Kernel
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 \
                          -Wall \
                          -Wcast-qual \
                          -Wextra \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=5 \
                          -fPIE \
                          -fstack-protector-all \
                          -fwrapv \
                          -std=c++11
QMAKE_DISTCLEAN        += -r temp
QMAKE_EXTRA_TARGETS    = libntru libspoton purge

INCLUDEPATH	+= . \
                   ../. \
                   ../../../. \
                   /usr/local/include/postgresql
LIBS		+= -L../../../libNTRU \
                   -L../../../libSpotOn \
                   -L/usr/local/lib \
                   -lGeoIP \
                   -lcrypto \
                   -lcurl \
                   -lgcrypt \
                   -lgpg-error \
                   -lntru \
                   -lpq \
                   -lspoton \
                   -lssl
MOC_DIR         = temp/moc
OBJECTS_DIR     = temp/obj
PRE_TARGETDEPS  = libntru.so libspoton.so
PROJECTNAME	= Spot-On-Kernel
RCC_DIR         = temp/rcc
TARGET		= ../Spot-On-Kernel
TEMPLATE        = app
UI_DIR          = temp/ui

# Prevent qmake from stripping everything.

QMAKE_STRIP	= echo