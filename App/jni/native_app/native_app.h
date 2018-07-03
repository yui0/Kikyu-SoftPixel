//---------------------------------------------------------
//	Android Native App
//
//		©2013 Yuichiro Nakada
//---------------------------------------------------------

// /android-toolchain/bin/arm-linux-androideabi-readelf -d libs/armeabi/libKikyu.so
#define LIB_PATH	"/data/data/berry.kikyu/lib/"
#define APP_LIB		"libKikyu.so"
#define LOAD_LIBS	{ load_lib(LIB_PATH "libSoftPixelEngine.so"); }
