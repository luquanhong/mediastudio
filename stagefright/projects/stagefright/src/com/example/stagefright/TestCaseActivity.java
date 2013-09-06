package com.example.stagefright;

import java.io.FileInputStream;
import java.io.IOException;

import android.app.Activity;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;

public class TestCaseActivity extends Activity implements Runnable,SurfaceHolder.Callback {

	public Surface mSurface;
	private SurfaceView mSurfaceView;
	private SurfaceHolder holder;
	private final String LAG = "This is a test";
	
	MyView vv = null;

	int mTrans = 0x0F0F0F0F;
	String PathFileName;

	// Menu item Ids
	public static final int PLAY_ID = Menu.FIRST;
	public static final int EXIT_ID = Menu.FIRST + 1;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//setContentView(R.layout.activity_test_case);

	
		
		/*** first
		//²âÊÔÈí½âÂë
		//vv = new MyView(this);
		// setContentView(vv);
		******/
		
		LinearLayout tp = new LinearLayout(this);
		LayoutParams ltp = new LayoutParams(LayoutParams.WRAP_CONTENT,
				LayoutParams.WRAP_CONTENT);
		tp.setOrientation(1);

		
		//circle lsit test Button 
		Button list = new Button(this);
		// list.setWidth(100);
		list.setText("test circle list");
		list.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				Log.d("", "test circke list click");
				//test_circle_list();
			}

		});

		//esay test case Button 
		Button test = new Button(this);
		test.setText("test case");
		test.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				Log.d("", "test case");
				//test();
			}

		});
		
		//video decode test case Button 
		Button decode = new Button(this);
		decode.setText("test video decode");
		decode.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				Log.d("", "test test_video_dec()");
				PathFileName = "/mnt/sdcard/predecode.h264"; // 352x288.264"; //240x320.264";
				
				startDecode();
			}

		});
		
		//video display surface 
		mSurfaceView = new SurfaceView(this);
		mSurfaceView.setVisibility(View.GONE);
		mSurfaceView.setVisibility(View.VISIBLE);
		holder = mSurfaceView.getHolder();
		holder.addCallback(this);
		holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);	
		
		//set layout display
		//tp.addView(list, ltp);
		//tp.addView(test, ltp);
		tp.addView(decode, ltp);
		tp.addView(mSurfaceView,ltp);
		setContentView(tp);	
	}

	//invoke the run()
	private void startDecode(){
		new Thread(this).start();
	}
	
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);

		menu.add(0, PLAY_ID, 0, "play");
		menu.add(0, EXIT_ID, 1, "exit");

		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case PLAY_ID: {

			return true;
		}
		case EXIT_ID: {
			Log.e("jack", "EXIT_ID");
			finish();
			return true;
		}
		}
		return super.onOptionsItemSelected(item);
	}


	public native String stringFromJNI();



	public native int um_vdec_init();

	public native int um_vdec_create();
	
	public native int um_vdec_decode(byte[] buf, int len);

	public native int um_vdec_destroy();
	
	public native int um_vdec_fini();
	
	public native int um_vdec_setSurface();
	
	
	static {

		System.loadLibrary("test");
	}

	
	
	private boolean checkStartMar(byte[] buf) {

		if ((buf[0] == 0) && (buf[1] == 0) && (buf[2] == 0) && (buf[3] == 10)) {

			Log.d(LAG, "find the start mark");
			return true;
		}

		return false;
	}



	private int byteToInteger(byte[] buf) {
		int iCount = 0;

		for (int i = 0; i < 4; i++) {

			int n = (buf[i] < 0 ? (int) buf[i] + 256 : (int) buf[i] << (8 * i));
			iCount += n;
		}

		return iCount;
	}

	
	@Override
	public void run() {
		Log.d("test", "test thread start");

		FileInputStream fileIS = null;

		byte[] bLen = new byte[4];
		byte[] bFrame = new byte[40980];
		int frameLen;

		try {
			Log.d("jack", "FileInputStream PathFileName =" + PathFileName);
			fileIS = new FileInputStream(PathFileName);
		} catch (IOException e) {
			Log.d("jack", "FileInputStream exception");
			return;
		}

		
		Log.d("jack", "InitDecoder ");
		um_vdec_init();

		um_vdec_create();
		
		Log.d("jack", "getSurface ");
		mSurface = holder.getSurface();
		um_vdec_setSurface();
		
		
		while (!Thread.currentThread().isInterrupted()) {

			try {

				int bytesRead = fileIS.read(bLen, 0, 4);
				Log.e(LAG,"bytesRead " + bytesRead);
				if(bytesRead == -1){
					Thread.currentThread().interrupt();
				}
						
				bytesRead = fileIS.read(bLen, 0, 4);
				Log.e(LAG,"bytesRead " + bytesRead);
				frameLen = byteToInteger(bLen);
				
				bytesRead = fileIS.read(bFrame, 0, frameLen);
				Log.e(LAG,"bytesRead " + bytesRead);
				
//				try {
//					Thread.sleep(10);
//				} catch (InterruptedException e) {
//					// TODO Auto-generated catch block
//					e.printStackTrace();
//				}

				um_vdec_decode(bFrame, frameLen);

			} catch (IOException e) {
				Log.d(LAG, "read the video resource exception");
			}
			

		}
		try {
			if (fileIS != null){
				Log.d(LAG, "read the video resource end========");
				um_vdec_destroy();
				Log.d(LAG, "read the video resource um_vdec_destroy");
				fileIS.close();
			}
			
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	
	// test the surface view 
	private Canvas canvas;
	private Paint paint;
	
	private void draw() {
		try {
			
			paint = new Paint();
			paint.setAntiAlias(true);
			
			canvas = holder.lockCanvas();
			canvas.drawColor(Color.WHITE);
		
			paint.setColor(0x70ff0000);
			paint.setTextSize(30);

			canvas.drawText("this is a test surface", 300, 300, paint);
		} catch (Exception e) {
			// TODO: handle exception
		} finally {
			try {
				if (canvas != null)
					holder.unlockCanvasAndPost(canvas);
			} catch (Exception e2) {

			}
		}
	}
	
	
	private void updateSurfaceScreenOn() {
		if (holder != null) {
			holder.setKeepScreenOn( true);
		}
	}
	

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		// TODO Auto-generated method stub
		Log.d("test", "avoid black screen on galaxyNote");
		
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		//draw();
		
		//this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		mSurfaceView.postInvalidate();
	
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		
	}

}
