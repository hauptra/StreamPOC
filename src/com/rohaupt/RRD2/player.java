package com.rohaupt.RRD2;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.Activity;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.SystemClock;

public class player extends Activity
{
	private AudioTrack track;
	private FileOutputStream os;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        createEngine();
        
        int bufSize = AudioTrack.getMinBufferSize(44100,
        										  AudioFormat.CHANNEL_CONFIGURATION_STEREO, 
        										  AudioFormat.ENCODING_PCM_16BIT);


        track = new AudioTrack(AudioManager.STREAM_MUSIC, 
        					   44100, 
        					   AudioFormat.CHANNEL_CONFIGURATION_STEREO, 
        					   AudioFormat.ENCODING_PCM_16BIT, 
        					   bufSize,
        					   AudioTrack.MODE_STREAM);

        byte[] bytes = new byte[bufSize];
        
        try {
			os = new FileOutputStream("/sdcard/a.out",false);
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
       // String result = loadFile("/sdcard/a.ogg",bytes);
		//Example("/sdcard/a.mp3",bytes);
		//"http://kissfm.live24.gr/kiss2111"
		//"http://91.121.155.39"
		//playURL("http://91.121.155.39",bytes);
		ExampleB("http://91.121.155.39",bytes);
        try {
			os.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }

    void playSound(byte[] buf, int size) {  
    	//android.util.Log.v("ROHAUPT", "RAH Playing");
    	if(track.getPlayState()!=AudioTrack.PLAYSTATE_PLAYING)
    		track.play();
        track.write(buf, 0, size);
        
        try {
			os.write(buf,0,size);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }

    
    private native void createEngine();
    private native String loadFile(String file, byte[] array);
    private native void Example(String file, byte[] array);
    private native void ExampleB(String file, byte[] array);
    
    /** Load jni .so on initialization*/ 
    static {
   	     System.loadLibrary("avutil"); 
    	 System.loadLibrary("avcore"); 
    	 System.loadLibrary("avcodec");
    	 System.loadLibrary("avformat");
    	 System.loadLibrary("avdevice");
    	 System.loadLibrary("swscale");
    	 System.loadLibrary("avfilter");
         System.loadLibrary("ffmpeg");
    }
}