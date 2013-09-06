package com.example.dynamicui;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import com.example.dynamicui.input.InputHandler;
import com.example.dynamicui.view.IEmuView;
import com.example.dynamicui.view.InputView;


import android.content.Intent;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Environment;
import android.view.Display;
import android.view.View;
import android.view.View.MeasureSpec;
import android.widget.FrameLayout.LayoutParams;



public class MainHelper {
	
	final static public  int SUBACTIVITY_USER_PREFS = 1;
	final static public  int SUBACTIVITY_HELP = 2;
	final static public  int BUFFER_SIZE = 1024*48;
	
	final static public  String MAGIC_FILE = "dont-delete-00001.bin";
	final static public  String MAGIC_FILE_2 = "dont-delete-00002.bin";
	
	protected MainActivity mm = null;
	
	public MainHelper(MainActivity value){
		mm = value;
	}
	
	

	
	public int getscrOrientation() {
		Display getOrient = mm.getWindowManager().getDefaultDisplay();
		//int orientation = getOrient.getOrientation();
		
		int orientation  = mm.getResources().getConfiguration().orientation;


		// Sometimes you may get undefined orientation Value is 0
		// simple logic solves the problem compare the screen
		// X,Y Co-ordinates and determine the Orientation in such cases
		if (orientation == Configuration.ORIENTATION_UNDEFINED) {

			Configuration config = mm.getResources().getConfiguration();
			orientation = config.orientation;

			if (orientation == Configuration.ORIENTATION_UNDEFINED) {
				// if emu_height and widht of screen are equal then
				// it is square orientation
				if (getOrient.getWidth() == getOrient.getHeight()) {
					orientation = Configuration.ORIENTATION_SQUARE;
				} else { // if widht is less than emu_height than it is portrait
					if (getOrient.getWidth() < getOrient.getHeight()) {
						orientation = Configuration.ORIENTATION_PORTRAIT;
					} else { // if it is not any of the above it will defineitly
								// be landscape
						orientation = Configuration.ORIENTATION_LANDSCAPE;
					}
				}
			}
		}
		return orientation; // return values 1 is portrait and 2 is Landscape
							// Mode
	}
	
	public void reload() {

	    Intent intent = mm.getIntent();

	    mm.overridePendingTransition(0, 0);
	    intent.addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
	    mm.finish();

	    mm.overridePendingTransition(0, 0);
	    mm.startActivity(intent);
	}
	
	

	
	public void setBorder(){
		
		int size = mm.getResources().getConfiguration().screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK; 
		
		if((size  == Configuration.SCREENLAYOUT_SIZE_LARGE || size  == Configuration.SCREENLAYOUT_SIZE_XLARGE) 
			&& mm.getMainHelper().getscrOrientation() == Configuration.ORIENTATION_PORTRAIT)
			{
		        LayoutParams lp  = (LayoutParams) mm.getEmuView().getLayoutParams();
				View v =  mm.findViewById(R.id.EmulatorFrame);
				LayoutParams lp2 = null;
//				if(mm.getFilterView()!=null)
//                   lp2 = (LayoutParams)mm.getFilterView().getLayoutParams();
			   				
			       v.setBackgroundDrawable(mm.getResources().getDrawable(R.drawable.border));
			       lp.setMargins(15, 15, 15, 15);
			       if(lp2!=null)
			    	 lp2.setMargins(15, 15, 15, 15);
			  		    
			}		   	
	}
	
	
	public void updateMAME4droid(){
		
		
		View emuView =  mm.getEmuView();
//		FilterView filterView = mm.getFilterView();

		InputView inputView =  mm.getInputView();
		InputHandler inputHandler = mm.getInputHandler();
		
//		String definedKeys = prefsHelper.getDefinedKeys();
//		final String[] keys = definedKeys.split(":");
//		for(int i=0;i<keys.length;i++)
//			InputHandler.keyMapping[i]=Integer.valueOf(keys[i]).intValue();
		
				
		int state = mm.getInputHandler().getInputHandlerState();
		

		{
			((IEmuView)emuView).setScaleType(5);
//			if(filterView!=null)
//			   filterView.setScaleType(mm.getPrefsHelper().getLandscapeScaleMode());
			
//			Emulator.setFrameFiltering(mm.getPrefsHelper().isLandscapeBitmapFiltering());
			
			
//			inputHandler.changeState();	
			
			state = mm.getInputHandler().getInputHandlerState();
			
		    inputView.bringToFront();
			
			if(state == InputHandler.STATE_SHOWING_NONE)
			{	
				inputView.setVisibility(View.GONE);
			}	
			else
			{	
			    inputView.setVisibility(View.VISIBLE);
			}   

			if(state == InputHandler.STATE_SHOWING_CONTROLLER)
			{			    				    		
				inputView.setImageDrawable(null);
			   	inputHandler.readControllerValues(R.raw.controller_landscape);			   	    
								 
			}
			else
			{

			}			
		}
		
		int op = inputHandler.getOpacity();
		if (op != -1 && (state == InputHandler.STATE_SHOWING_CONTROLLER) )
			inputView.setAlpha(op);

		inputView.requestLayout();		  				
		emuView.requestLayout();
	
				
		inputView.invalidate();
		emuView.invalidate();
					
	}
	
	public ArrayList<Integer> measureWindow(int widthMeasureSpec, int heightMeasureSpec, int scaleType) {
		   
		int widthSize = 1;
		int heightSize = 1;
		
	
//		if (scaleType == PrefsHelper.PREF_STRETCH)// FILL ALL
//		{
			widthSize = MeasureSpec.getSize(widthMeasureSpec);
			heightSize = MeasureSpec.getSize(heightMeasureSpec);
//		} 
//		else 
//		{
//			
//			int emu_w = Emulator.getEmulatedWidth();
//		    int emu_h = Emulator.getEmulatedHeight();
//		    
//		    
//		    if(scaleType == PrefsHelper.PREF_15X)
//		    {
//		    	emu_w = (int)(emu_w * 1.5f);
//		    	emu_h = (int)(emu_h * 1.5f);
//		    }
//		    
//		    if(scaleType == PrefsHelper.PREF_20X)
//		    {
//		    	emu_w = emu_w * 2;
//		    	emu_h = emu_h * 2;
//		    }
//		    
//		    if(scaleType == PrefsHelper.PREF_25X)
//		    {
//		    	emu_w = (int)(emu_w * 2.5f);
//		    	emu_h = (int)(emu_h * 2.5f);
//		    }
//		    
//			int w = emu_w;
//			int h = emu_h;
//
//			widthSize = MeasureSpec.getSize(widthMeasureSpec);
//			heightSize = MeasureSpec.getSize(heightMeasureSpec);
//						
//			if(heightSize==0)heightSize=1;
//			if(widthSize==0)widthSize=1;
//
//			float scale = 1.0f;
//
//			if (scaleType == PrefsHelper.PREF_SCALE)
//				scale = Math.min((float) widthSize / (float) w,
//						(float) heightSize / (float) h);
//
//			w = (int) (w * scale);
//			h = (int) (h * scale);
//
//			float desiredAspect = (float) emu_w / (float) emu_h;
//
//			widthSize = Math.min(w, widthSize);
//			heightSize = Math.min(h, heightSize);
//
//			float actualAspect = (float) (widthSize / heightSize);
//
//			if (Math.abs(actualAspect - desiredAspect) > 0.0000001) {
//
//				boolean done = false;
//
//				// Try adjusting emu_width to be proportional to emu_height
//				int newWidth = (int) (desiredAspect * heightSize);
//
//				if (newWidth <= widthSize) {
//					widthSize = newWidth;
//					done = true;
//				}
//
//				// Try adjusting emu_height to be proportional to emu_width
//				if (!done) {
//					int newHeight = (int) (widthSize / desiredAspect);
//					if (newHeight <= heightSize) {
//						heightSize = newHeight;
//					}
//				}
//			}
//		}
		
		ArrayList<Integer> l = new ArrayList<Integer>();
		l.add(new Integer(widthSize));
		l.add(new Integer(heightSize));
		return l;
		
	}	
	
}
