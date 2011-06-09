LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -g
LOCAL_MODULE := ffmpeg
LOCAL_SHARED_LIBRARIES := bambuser-libavcodec bambuser-libavcore bambuser-libavdevice bambuser-libavfilter bambuser-libavformat bambuser-libavutil bambuser-libswscale 
LOCAL_SRC_FILES := player.c
LOCAL_LDLIBS    := -L$(LOCAL_PATH)/../bffmpeg -lavcodec -lavcore -lavdevice -lavfilter -lavformat -lavutil -lswscale -llog -lz -lGLESv1_CM

include $(BUILD_SHARED_LIBRARY)

