# Kikyu!
# ©2013 YUICHIRO NAKADA

#CROSS=mipsel-uclibc-
CC=$(CROSS)g++
LD=$(CROSS)ld
AR=$(CROSS)ar
RANLIB=$(CROSS)ranlib

#CCFLAGS+=-Os -Wall -I. -Wno-pointer-sign -fomit-frame-pointer
CCFLAGS+=-Os -Wall -I. -I./include -fomit-frame-pointer -Wno-missing-braces
LDFLAGS+=-s -L.
LIBS=-lSoftPixelEngine

PACKAGE=kikyu
OBJS=$(PACKAGE).o


# for Windows (Digital Mars C++)
WCC=		wine ./dm/bin/dmc
WCFLAGS=	-I./dm/stlport/stlport -I. -D__LCC__
## http://curl.haxx.se/download.html
# wine dm/bin/implib.exe /s curl.lib libcurl.dll
# wine dm/bin/implib.exe /s miniupnpc.lib miniupnpc.dll
WLDFLAGS=	SoftPixelEngine.lib
WOBJS=$(PACKAGE).cpp


all: $(PACKAGE)

$(PACKAGE): $(OBJS) $(LIBS)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(OBJS) -o $(PACKAGE) $(LIBS)

%.o: %.cpp
	$(CC) $(CCFLAGS) -c $<

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

kikyu_exe: $(WOBJS)
	$(WCC) $(WCFLAGS) $(WOBJS) $(WLDFLAGS)

clean:
	rm -f *.o *.obj *.map $(PACKAGE)
