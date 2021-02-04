
CC = arm-buildroot-linux-gnueabi-gcc
CPP = arm-buildroot-linux-gnueabi-g++
STRIP = arm-buildroot-linux-gnueabi-strip
AR = arm-buildroot-linux-gnueabi-ar

SUBDIR = . misc media ui app util app/picture app/RTS app/setup app/test

INC = $(foreach dir,$(SUBDIR),-I$(dir))
CFLAGS += $(INC) -I./include -I./include/freetype2
CXXFLAGS += $(INC) -I./include -I./include/freetype2
CFLAGS += -O2 -Wall -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare
CXXFLAGS += -O2 -Wall -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare

LIBS += ./libs/libcharset.a \
	./libs/libiconv.a \
	./libs/libfreetype.a \
	./libs/libjpeg.a \
	./libs/libpng.a \
	./libs/libz.a \
	./libs/libmisc.a \
	./libs/libswscale.a \
	./libs/libavdevice.a \
	./libs/libavfilter.a \
	./libs/libavformat.a \
	./libs/libavcodec.a \
	./libs/libavutil.a \
	./libs/libswresample.a \
	./libs/libspeex.a \
	./libs/libspeexdsp.a \
	-lpthread -lnsl -lrt -lresolv -lz

CSRCS    = $(foreach dir,$(SUBDIR),$(wildcard $(dir)/*.c))
CPPSRCS  = $(foreach dir,$(SUBDIR),$(wildcard $(dir)/*.cpp))
OBJS     = $(CSRCS:%.c=%.o) $(CPPSRCS:%.cpp=%.o)

TARGET := desktop
.PHONY : clean all

all: upVersion $(TARGET)

$(TARGET): $(OBJS)
	$(CPP) -O2 -o $@ $^ $(LIBS)
	$(STRIP) $@
	cp $@ ../../romfs/rootfs/dnake/bin

upVersion:
	@touch sys.cpp

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CPP) -c $(CXXFLAGS) -o "$@" "$<"

.cc.o:
	$(CPP) -c $(CXXFLAGS) -o "$@" "$<"

.cxx.o:
	$(CPP) -c $(CXXFLAGS) -o "$@" "$<"

.C.o:
	$(CPP) -c $(CXXFLAGS) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) -o "$@" "$<"

clean:
	@rm -f $(TARGET)
	@rm -f $(OBJS)
