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

void Java_com_rohaupt_RRD2_player_createEngine(JNIEnv* env, jobject obj)
	{
		avcodec_init();
		
		av_register_all();

	}
	
	jstring Java_com_rohaupt_RRD2_player_loadFile(JNIEnv* env, jobject obj,jstring file,jbyteArray array)
	{	
		jboolean 			isCopy;  
		int 				i;
    	int 				audioStream=-1;
		int 				res;
		int 				decoded = 0;
		int					out_size;
    	AVFormatContext 	*pFormatCtx;
    	AVCodecContext 		*aCodecCtx;
    	AVCodecContext 		*c= NULL;
    	AVCodec         	*aCodec, *oCodec;
    	AVPacket        	packet;
    	jclass 				cls = (*env)->GetObjectClass(env, obj);
		jmethodID 			play = (*env)->GetMethodID(env, cls, "playSound", "([BI)V");//At the begining of your main function
    	const char * 		szfile = (*env)->GetStringUTFChars(env, file, &isCopy);
    	int16_t *			pAudioBuffer = (int16_t *) av_malloc (AUDIO_INBUF_SIZE);
    	int16_t *			outBuffer = (int16_t *) av_malloc (AUDIO_INBUF_SIZE);
    	
    	
    	__android_log_print(ANDROID_LOG_INFO, DEBUG_TAG, "RAH28 Starting");
    	res = av_open_input_file(&pFormatCtx, szfile, NULL, 0, NULL);
    	if(res!=0)
    	{
    		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH opening input failed with result: [%d]", res);
    		return file;
    	}
    	
    	// Dump information about file onto standard error
    	dump_format(pFormatCtx, 0, szfile, 0);

    	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH getting stream info");
    	res = av_find_stream_info(pFormatCtx);
    	if(res<0)
  		{
  			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH getting stream info failed with result: [%d]", res);
    		return file;
    	}
    	
    	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH getting audio stream in count of [%d]", pFormatCtx->nb_streams);
		for(i=0; i < pFormatCtx->nb_streams; i++) {
		  if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO &&
		     audioStream < 0) {
		    audioStream=i;
		  }
		}


		/*oCodec = avcodec_find_encoder(CODEC_ID_PCM_S16LE);
		if(!oCodec)
		{
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH couldn't find encoder");
			return file;
		}

		c = avcodec_alloc_context();
		c->sample_rate=32000;
		c->channels=2;

		if(avcodec_open(c,oCodec)<0)
		{
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH couldn't open encoder");
			return file;
		}
		*/

		if(audioStream==-1)
		{
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH couldn't find audio stream");
		  	return file;
		}
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH audio stream found with result: [%d]", res);
		
				
		aCodecCtx=pFormatCtx->streams[audioStream]->codec;
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH audio codec info loaded sample rate [%d] channels [%d] frame size [%d] frame number [%d]", aCodecCtx->sample_rate,aCodecCtx->channels,aCodecCtx->frame_size,aCodecCtx->frame_number);
		
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH audio codec info [%d]", aCodecCtx->codec_id);
		
		aCodec = avcodec_find_decoder(aCodecCtx->codec_id);
	    if(!aCodec) {
	       __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH audio codec unsupported");
	       return file;
	    }
	    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH audio codec info found");
	    
	    
	    res = avcodec_open(aCodecCtx, aCodec);
	    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH audio codec loaded [%d] [%d]",aCodecCtx->sample_fmt,res);
	    
	    av_init_packet(&packet);
	    
	    
	    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH channels [%d] sample rate [%d] sample format [%d]",aCodecCtx->channels,aCodecCtx->sample_rate,aCodecCtx->sample_fmt);
	    
	    /*ReSampleContext* rsc = av_audio_resample_init(aCodecCtx->channels,
	    											  aCodecCtx->channels,
	    											  aCodecCtx->sample_rate,
	    											  aCodecCtx->sample_rate,
	    											  av_get_sample_fmt("s16"), 
	    											  aCodecCtx->sample_fmt,
          											  16,
          											  10,
          											  0,
          											  1);*/
	    
	    int x,y;
	    x=0;y=0;

	    /*uint8_t *outbuf;
	    int outbuf_size=10000;
	    outbuf = av_malloc(outbuf_size);*/

      	while (av_read_frame(pFormatCtx, &packet)>= 0) {
      		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH frame read: [%d] [%d]",x++,y);
      		
      		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH pts [%d] dts [%d]",packet.pts,packet.dts);
      		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH si [%d] flags [%d]",packet.stream_index,packet.flags);
      		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH duration [%d] pos [%d]",packet.duration,packet.pos);
      		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH cd [%d]",packet.convergence_duration);
      		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH audio codec info loaded sample rate [%d] channels [%d] frame size [%d] frame number [%d]", aCodecCtx->sample_rate,aCodecCtx->channels,aCodecCtx->frame_size,aCodecCtx->frame_number);
            if (aCodecCtx->codec_type == AVMEDIA_TYPE_AUDIO) {
            			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH audio ready");
                        int data_size = AUDIO_INBUF_SIZE;
                        int size=packet.size;
                        y=0;
                        decoded = 0;
                        __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH packet size: [%d]", size);
                        while(size > 0) {
                        
                        		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH decoding: [%d] [%d]",x,y++);
                                int len = avcodec_decode_audio3(aCodecCtx, pAudioBuffer, &data_size, &packet);
                                if(len<0)
                                	break;
                                //out_size = avcodec_encode_audio(c,outbuf,outbuf_size,pAudioBuffer);
                                //out_size = audio_resample(rsc, outBuffer,pAudioBuffer,len);
                                
                                
								__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH 1 size [%d] len [%d] data_size [%d] out_size [%d]",size,len,data_size,out_size);
                                jbyte *bytes = (*env)->GetByteArrayElements(env, array, NULL);
                                
                                memcpy(bytes + decoded, pAudioBuffer, len); //
                                //memcpy(bytes+decoded,outbuf,out_size);
                                
                                __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH 2");
                                
                                (*env)->ReleaseByteArrayElements(env, array, bytes, 0);
                                __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH 3");

								(*env)->CallVoidMethod(env, obj, play, array, len);
								//(*env)->CallVoidMethod(env, obj, play, array, out_size);
								__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH 4");
								
								
                                size -= len;
								decoded += len;
								//decoded += out_size;
                       }
                       av_free_packet(&packet);
            }

     }

		  // Close the video file
  		av_close_input_file(pFormatCtx);
		
    	//__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH Finished Running result: [%d]", res);
    	(*env)->ReleaseStringUTFChars(env, file, szfile);	
    	return file;  
	}


	void Java_com_rohaupt_RRD2_player_Example(JNIEnv* env, jobject obj,jstring file,jbyteArray array)
	{
		jboolean 			isfilenameCopy;
		const char * 		filename = (*env)->GetStringUTFChars(env, file, &isfilenameCopy);
	    AVCodec *codec;
	    AVCodecContext *c= NULL;
	    int out_size, len;
	    FILE *f, *outfile;
	    uint8_t *outbuf;
	    uint8_t inbuf[AUDIO_INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
	    AVPacket avpkt;
	    jclass 				cls = (*env)->GetObjectClass(env, obj);
	    jmethodID 			play = (*env)->GetMethodID(env, cls, "playSound", "([BI)V");//At the begining of your main function

	    av_init_packet(&avpkt);

	    printf("Audio decoding\n");

	    /* find the mpeg audio decoder */
	    codec = avcodec_find_decoder(CODEC_ID_MP3);
	    if (!codec) {
	        fprintf(stderr, "codec not found\n");
	        exit(1);
	    }

	    c= avcodec_alloc_context();

	    /* open it */
	    if (avcodec_open(c, codec) < 0) {
	        fprintf(stderr, "could not open codec\n");
	        exit(1);
	    }

	    outbuf = malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE);

	    f = fopen(filename, "rb");
	    if (!f) {
	        fprintf(stderr, "could not open %s\n", filename);
	        exit(1);
	    }

	    /* decode until eof */
	    avpkt.data = inbuf;
	    avpkt.size = fread(inbuf, 1, AUDIO_INBUF_SIZE, f);

	    while (avpkt.size > 0) {
	        out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
	        len = avcodec_decode_audio3(c, (short *)outbuf, &out_size, &avpkt);
	        if (len < 0) {
	            fprintf(stderr, "Error while decoding\n");
	            exit(1);
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
	        if (avpkt.size < AUDIO_REFILL_THRESH) {
	            /* Refill the input buffer, to avoid trying to decode
	             * incomplete frames. Instead of this, one could also use
	             * a parser, or use a proper container format through
	             * libavformat. */
	            memmove(inbuf, avpkt.data, avpkt.size);
	            avpkt.data = inbuf;
	            len = fread(avpkt.data + avpkt.size, 1,
	                        AUDIO_INBUF_SIZE - avpkt.size, f);
	            if (len > 0)
	                avpkt.size += len;
	        }
	    }

	    fclose(f);
	    free(outbuf);

	    avcodec_close(c);
	    av_free(c);
	}

	void Java_com_rohaupt_RRD2_player_ExampleB(JNIEnv* env, jobject obj,jstring file,jbyteArray array)
	{
		jboolean 			isfilenameCopy;
		const char * 		url = (*env)->GetStringUTFChars(env, file, &isfilenameCopy);
	    AVCodec *codec;
	    AVCodecContext *c= NULL;
	    int out_size, len;
	    FILE *f, *outfile;
	    uint8_t *outbuf;
	    uint8_t inbuf[AUDIO_INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
	    AVPacket avpkt;
	    jclass 				cls = (*env)->GetObjectClass(env, obj);
	    jmethodID 			play = (*env)->GetMethodID(env, cls, "playSound", "([BI)V");//At the begining of your main function

	    av_init_packet(&avpkt);

	    printf("Audio decoding\n");

	    /* find the mpeg audio decoder */
	    codec = avcodec_find_decoder(CODEC_ID_MP3);
	    if (!codec) {
	        fprintf(stderr, "codec not found\n");
	        exit(1);
	    }

	    c= avcodec_alloc_context();

	    /* open it */
	    if (avcodec_open(c, codec) < 0) {
	        fprintf(stderr, "could not open codec\n");
	        exit(1);
	    }

	    outbuf = malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE);

	    /*f = fopen(filename, "rb");
	    if (!f) {
	        fprintf(stderr, "could not open %s\n", filename);
	        exit(1);
	    }*/

	    URLContext *cntx;

	    int result;


	    result = url_open(&cntx,url,URL_RDONLY);
	    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH url_open [%d]", result);

	    len = url_read_complete(cntx,inbuf,AUDIO_INBUF_SIZE);
	    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH url_read [%d]", len);

	    /* decode until eof */
	    avpkt.data = inbuf;
	    avpkt.size = len;
	    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH 2");
	    while (avpkt.size > 0) {
	        out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
	        len = avcodec_decode_audio3(c, (short *)outbuf, &out_size, &avpkt);
	        if (len < 0) {
	        	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH Bad Len");
	            fprintf(stderr, "Error while decoding\n");
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
	            //exit(1);
	            continue;
	        }

	        __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH 3");

	        if (out_size > 0) {
	            /* if a frame has been decoded, output it */
	        	jbyte *bytes = (*env)->GetByteArrayElements(env, array, NULL);
				memcpy(bytes, outbuf, out_size); //
	        	(*env)->ReleaseByteArrayElements(env, array, bytes, 0);
	        	(*env)->CallVoidMethod(env, obj, play, array, out_size);

	        }

	        __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH 4");

	        avpkt.size -= len;
	        avpkt.data += len;
	        if (avpkt.size < AUDIO_REFILL_THRESH) {
	            /* Refill the input buffer, to avoid trying to decode
	             * incomplete frames. Instead of this, one could also use
	             * a parser, or use a proper container format through
	             * libavformat. */
	        	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "RAH 5");
	            memmove(inbuf, avpkt.data, avpkt.size);
	            avpkt.data = inbuf;
	            len = url_read(cntx,avpkt.data + avpkt.size,AUDIO_INBUF_SIZE - avpkt.size);
	            		                   //fread(avpkt.data + avpkt.size, 1,
	                                       //      AUDIO_INBUF_SIZE - avpkt.size, f);
	            if (len > 0)
	                avpkt.size += len;
	        }
	    }

	    free(outbuf);

	    avcodec_close(c);
	    av_free(c);
	}
