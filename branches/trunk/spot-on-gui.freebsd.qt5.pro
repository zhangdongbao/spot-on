cache()
include(spot-on-gui-source.pro)
libntru.commands = gmake -C ../../libNTRU
libntru.depends =
libntru.target = libntru.so
libspoton.commands = gmake -C ../../libSpotOn library
libspoton.depends =
libspoton.target = libspoton.so

CONFIG		+= qt release warn_on
LANGUAGE	= C++
QT		+= concurrent \
                   gui \
                   multimedia \
                   network \
                   printsupport \
                   sql \
                   websockets \
                   widgets

DEFINES	+= SPOTON_LINKED_WITH_LIBGEOIP \
	   SPOTON_LINKED_WITH_LIBNTRU \
           SPOTON_LINKED_WITH_LIBPTHREAD \
           SPOTON_MCELIECE_ENABLED \
	   SPOTON_SCTP_ENABLED \
	   SPOTON_WEBSOCKETS_ENABLED

# Unfortunately, the clean target assumes too much knowledge
# about the internals of libNTRU and libSpotOn.

QMAKE_CLEAN            += ../../libNTRU/*.so \
                          ../../libNTRU/src/*.o \
                          ../../libNTRU/src/*.s \
                          ../../libSpotOn/*.o \
                          ../../libSpotOn/*.so \
                          ../../libSpotOn/test \
                          Spot-On
QMAKE_CXX              = clang++
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 \
                          -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wextra \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=5 \
                          -fPIE \
                          -fstack-protector-all \
                          -fwrapv \
                          -mtune=native
QMAKE_DISTCLEAN        += -r temp .qmake.cache .qmake.stash
QMAKE_EXTRA_TARGETS    = libntru libspoton purge

INCLUDEPATH	+= . \
                   ../../. \
                   /usr/local/include/postgresql \
                   GUI
LIBS		+= -L../../libNTRU \
                   -L../../libSpotOn \
                   -lGeoIP \
                   -lcrypto \
                   -lcurl \
                   -lgcrypt \
                   -lgmp \
                   -lgpg-error \
                   -lntl \
                   -lntru \
                   -lpq \
                   -lspoton \
                   -lssl
MOC_DIR         = temp/moc
OBJECTS_DIR     = temp/obj
PRE_TARGETDEPS  = libntru.so libspoton.so
PROJECTNAME	= Spot-On
RCC_DIR         = temp/rcc
TARGET		= Spot-On
TEMPLATE        = app
UI_DIR          = temp/ui

# Prevent qmake from stripping everything.

QMAKE_STRIP	= echo
