LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# !!! Uncommnet this to build native code on Android v >= lollipop
#
# Enable PIE manually. Will get reset on $(CLEAR_VARS). This
# # is what enabling PIE translates to behind the scenes.
# LOCAL_CFLAGS += -fPIE
# LOCAL_LDFLAGS += -fPIE -pie
#

LOCAL_MODULE := term_emu
LOCAL_SRC_FILES := main.c
LOCAL_LDLIBS := -llog -L$(LOCAL_PATH)/lib  # -lmystuff link to libmystuff.so

LOCAL_DISABLE_FORMAT_STRING_CHECKS := true

include $(BUILD_EXECUTABLE)    # <-- Use this to build an executable
