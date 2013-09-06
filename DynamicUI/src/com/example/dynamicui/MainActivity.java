package com.example.dynamicui;

import com.example.dynamicui.input.InputHandler;
import com.example.dynamicui.input.InputHandlerFactory;
import com.example.dynamicui.view.*;

import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.FrameLayout;

public class MainActivity extends Activity {

	
	protected MainHelper mainHelper = null;
	protected View emuView = null;

	protected InputView inputView = null;
	
	protected InputHandler inputHandler = null;
	
	public MainHelper getMainHelper() {
		return mainHelper;
	}
	
	public View getEmuView() {
		return emuView;
	}
	
	public InputView getInputView() {
		return inputView;
	}
	
	public InputHandler getInputHandler() {
		return inputHandler;
	}
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
		
        Log.d("jack", "main onCreate");
		Log.d("jack", "this is trunk change 2");
        
        setContentView(R.layout.main);
        
        inputHandler = InputHandlerFactory.createInputHandler(this);
        
        FrameLayout fl = (FrameLayout)this.findViewById(R.id.EmulatorFrame);
        
        this.getLayoutInflater().inflate(R.layout.emuview_sw, fl);
    	emuView = this.findViewById(R.id.EmulatorViewSW);
    	
    	inputView = (InputView) this.findViewById(R.id.InputView);
        
        //((IEmuView)emuView).setMAME4droid(this);

        inputView.setMAME4droid(this);
        
        //Emulator.setMAME4droid(this);
        
        mainHelper = new MainHelper(this);
        
        Log.e("jack","this is branch change 2");
    	View frame = this.findViewById(R.id.EmulatorFrame);
        frame.setOnTouchListener(inputHandler); 
        
        
        emuView.setOnKeyListener(inputHandler);
        emuView.setOnTouchListener(inputHandler);
                     
        inputView.setOnTouchListener(inputHandler);
        inputView.setOnKeyListener(inputHandler);
        
        mainHelper.updateMAME4droid();
        
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.activity_main, menu);
        return true;
    }
    
}
