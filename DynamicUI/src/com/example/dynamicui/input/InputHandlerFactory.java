
package com.example.dynamicui.input;

import java.lang.reflect.Method;

import android.view.MotionEvent;

import com.example.dynamicui.MainActivity;;


public class InputHandlerFactory {
	
	static public InputHandler createInputHandler(MainActivity mm){		
		try {
		      @SuppressWarnings("unused")
			  Method m = MotionEvent.class.getMethod("getPointerCount");		     
		      return new InputHandlerExt(mm);//MultiTouch  
		      //return new InputHandler(mm);//FAKED para pruebas
		} catch (NoSuchMethodException e) {
			return new InputHandler(mm);
		}		
	}	
}
