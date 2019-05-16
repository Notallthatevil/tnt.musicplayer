//
// Created by Nate on 5/15/2019.
//

#ifndef TNT_MUSICPLAYER_LOGGING_MACROS_H
#define TNT_MUSICPLAYER_LOGGING_MACROS_H

#include <android/log.h>

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, __FILE__ __TIME__, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, __FILE__ __TIME__, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, __FILE__ __TIME__, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,__FILE__ __TIME__, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,__FILE__ __TIME__, __VA_ARGS__)
#define LOGA(...) __android_log_print(ANDROID_LOG_FATAL,__FILE__ __TIME__, __VA_ARGS__)



#endif //TNT_MUSICPLAYER_LOGGING_MACROS_H
