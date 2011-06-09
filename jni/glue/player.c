#include <assert.h>
#include <jni.h>
#include <string.h>
#include <android/log.h>

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"

#define DEBUG_TAG "ROHAUPT"  
#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

	int cancel=0;

	void Java_com_rohaupt_RRD2_NativePlayer_createEngine(JNIEnv* env, jobject obj)
	{
		//Initialize FFMpeg
		avcodec_init();
		av_register_all();
	}

	void Java_com_rohaupt_RRD2_NativePlayer_cancel(JNIEnv* env, jobject obj)
	{
		cancel = 1;
	}

	void Java_com_rohaupt_RRD2_NativePlayer_playURL(JNIEnv* env, jobject obj,jstring file,jbyteArray array)
	{
		//Variables to Convert Java String containing URL to C Style String
		jboolean 			isfilenameCopy;
		const char * 		url = (*env)->GetStringUTFChars(env, file, &isfilenameCopy);
		
		//Codec Variables
	    AVCodec *			codec;
	    AVCodecContext *	c= NULL;
	    
	    //FFMpeg Data
	    AVPacket 			avpkt;
	    
	    //Buffers
	    int 				out_size, len;
	    uint8_t *			outbuf;
	    uint8_t 			inbuf[AUDIO_INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
	    
	    //Variables to callback to Java Code
	    jclass 				cls = (*env)->GetObjectClass(env, obj);
	    jmethodID 			play = (*env)->GetMethodID(env, cls, "playSound", "([BI)V");//At the begining of your main function
	    
	    //URL Handling
	    URLContext *		cntx;
	    int 				result;
	    
	    //Initialize Packet
	    av_init_packet(&avpkt);


	    /* find the mpeg audio decoder */
	    codec = avcodec_find_decoder(CODEC_ID_MP3);
	    if (!codec) {
	        fprintf(stderr, "codec not found\n");
	        exit(1);
	    }

	    //Initialize Codec Context
	    c= avcodec_alloc_context();

	    /* open it */
	    if (avcodec_open(c, codec) < 0) {
	        fprintf(stderr, "could not open codec\n");
	        exit(1);
	    }

	    //Allocate output buffer
	    outbuf = malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE);

	    //Open URL for Read Only
	    result = url_open(&cntx,url,URL_RDONLY);
	    if(result != 0)
	    {
	    	fprintf(stderr,"could not open url\n");
	    	exit(1);
	    }

	    //Set Data Buffer and Size for packet
	    avpkt.data = inbuf;
	    avpkt.size = url_read_complete(cntx,inbuf,AUDIO_INBUF_SIZE);;

	    /**
	     * Loops through, decodes and outputs.  This will need to be improved with more robust
	     * buffer implementation and a producer-consumer thread model
	     */
	    while (avpkt.size > 0) {

	        out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;

	        len = avcodec_decode_audio3(c, (short *)outbuf, &out_size, &avpkt);


	        if (len < 0) {
	        	/**
	        	 * Based on the location of the pointer in a packet and the Song information between I am skipping bytes to find the start
	        	 * of a frame.  This brute force method will need to be improved to properly account for those gaps and handle them more
	        	 * gracefully
	        	 */
	            if(avpkt.size < AUDIO_REFILL_THRESH)
	            {
	            	avpkt.data = inbuf;
					len = url_read(cntx,avpkt.data + avpkt.size,AUDIO_INBUF_SIZE - avpkt.size);
					if (len > 0)
						avpkt.size += len;
	            }
	            else
	            {
	            	avpkt.data +=1;
					avpkt.size -=1;
	            }
	            continue;
	        }

	        if (out_size > 0) {
	            /* if a frame has been decoded, output it */
	        	jbyte *bytes = (*env)->GetByteArrayElements(env, array, NULL);
				memcpy(bytes, outbuf, out_size); //
	        	(*env)->ReleaseByteArrayElements(env, array, bytes, 0);
	        	(*env)->CallVoidMethod(env, obj, play, array, out_size);

	        }

	        avpkt.size -= len;
	        avpkt.data += len;

	        //Refills Input buffer if it gets low
	        if (avpkt.size < AUDIO_REFILL_THRESH) {
	            memmove(inbuf, avpkt.data, avpkt.size);
	            avpkt.data = inbuf;
	            len = url_read(cntx,avpkt.data + avpkt.size,AUDIO_INBUF_SIZE - avpkt.size);
	            if (len > 0)
	                avpkt.size += len;
	        }

	        if(cancel)
	        	break;
	    }

	    free(outbuf);

	    avcodec_close(c);
	    av_free(c);
	}
