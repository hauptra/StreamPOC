package com.rohaupt.RRD2;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.AsyncTask;

public class NativePlayer extends AsyncTask<String,Void,String>{

	private AudioTrack track;
	
	@Override
	protected String doInBackground(String... urls) {
        //Setup FFMpeg
        createEngine();
        
        /**
         * Setup Audio Track information.  This may be able to get pushed down into the
         * native layer to avoid the need to callback to java with the decoded data
         */
        int bufSize = AudioTrack.getMinBufferSize(44100,
        										  AudioFormat.CHANNEL_CONFIGURATION_STEREO, 
        										  AudioFormat.ENCODING_PCM_16BIT);


        track = new AudioTrack(AudioManager.STREAM_MUSIC, 
        					   44100, 
        					   AudioFormat.CHANNEL_CONFIGURATION_STEREO, 
        					   AudioFormat.ENCODING_PCM_16BIT, 
        					   bufSize,
        					   AudioTrack.MODE_STREAM);

        //Allocate Buffer
        byte[] bytes = new byte[bufSize];
        
        //Start decoding the audio
		playURL(urls[0],bytes);
		
		return "Done";
	}


    void playSound(byte[] buf, int size) {  
    	//Start the track if it isn't already playing
    	if(track.getPlayState()!=AudioTrack.PLAYSTATE_PLAYING)
    		track.play();
    	
    	//Write the buffer to the AudioTrack
        track.write(buf, 0, size);
    }

    //Native functions
    private native void createEngine();
    private native void playURL(String file, byte[] array);
    public native void cancel();
    
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
