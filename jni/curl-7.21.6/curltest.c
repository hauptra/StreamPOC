
#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include "curl_config.h"
#include "curl/curl.h"

typedef struct pageInfo_t {
  char *data;
  int  len;
} pageInfo_t;

static size_t
HTTPData(void *buffer, size_t size, size_t nmemb, void *userData) {
  int len = size * nmemb;
  pageInfo_t *page = (pageInfo_t *)userData;


  if (buffer && page->data && (page->len + len < (16 * 1024)) ) {
    memcpy(&page->data[page->len], buffer, len);
    page->len += len;
  }
  return len;
}

   //Inteface funciton that will recieve web page fom Java
jstring
Java_com_mtterra_curltest_curltest_JNIGetWebpage( JNIEnv* env,
                                               jobject entryObject,
					       jstring webpageJStr )
{
  pageInfo_t page;
  CURL *curl;
  CURLcode res;
  char *buffer;

  const jbyte *webpage;
  webpage = (*env)->GetStringUTFChars(env, webpageJStr, NULL);
  if (webpage == NULL) {
      return NULL; /* OutOfMemoryError already thrown */
  }

  page.data = (char *)malloc(16 * 1024);
  page.len = 0;
  if (page.data)
    memset(page.data, 32, 16 * 1024);

  buffer = (char *)malloc(1024);

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, webpage);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HTTPData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &page);

    res = curl_easy_perform(curl);
        /* always cleanup */
    curl_easy_cleanup(curl);
    (*env)->ReleaseStringUTFChars(env, webpageJStr, webpage);
    if(res == 0) {
      if (buffer) {
        page.data[page.len < 256 ? page.len : 256] = '\0';
	sprintf(buffer, "pagedata(%d): %s. done.\n", page.len, page.data);
        return (*env)->NewStringUTF(env, buffer);
      }
    }
    sprintf(buffer, "Result %d", res);
    return (*env)->NewStringUTF(env, buffer);
   } else {
      return (*env)->NewStringUTF(env, "Unable to init cURL");
   }
} 


