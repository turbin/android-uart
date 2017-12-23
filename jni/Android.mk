# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


LOCAL_PATH := $(call my-dir)

#################################################################
# compile as lib, which will depend on and include the first one
#################################################################
include $(CLEAR_VARS)
LOCAL_MODULE    := libuartapi
LOCAL_SRC_FILES := uart.c
LOCAL_LDLIBS += -llog

include $(BUILD_SHARED_LIBRARY)

#################################################################
# compile as lib, which will depend on and include the first one
#################################################################
include $(CLEAR_VARS)
LOCAL_MODULE    := uartjnihelper
LOCAL_SRC_FILES := uartjnihelper uart.c
LOCAL_LDLIBS += -llog
include $(BUILD_SHARED_LIBRARY)
# #################################################################
# second lib, which will depend on and include the first one
#################################################################
include $(CLEAR_VARS)

LOCAL_MODULE    := uartTester
LOCAL_SRC_FILES := uartTester.c uart.c

LOCAL_LDLIBS += -llog -lnlposapi1
LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE
include $(BUILD_EXECUTABLE)