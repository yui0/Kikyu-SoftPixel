package berry.kikyu;

import android.app.NativeActivity;

public class LoadLibs extends NativeActivity {

	static {

		System.loadLibrary("SoftPixelEngine");
		System.loadLibrary("Kikyu");
	}

}
