package com.rohaupt.RRD2;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;

public class player extends Activity
{
	private NativePlayer np;
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        np = new NativePlayer();
        np.execute("http://91.121.155.39");
        
    }

    public void quitActivity(View view)
    {
    	np.cancel();
    	this.finish();
    }
}