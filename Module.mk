json_headers += JsonWriter.h \
   JsonReader.h \
   JsonValue.h

json_sources += JsonWriter.cpp \
    JsonReader.cpp \
    JsonValue.cpp

json_dir = json


util_headers += Util.h
util_sources += Util.cpp
util_dir = util

LOCAL_DIR = $(call my-dir)/



INCLUDES +=  $(LOCAL_DIR)$(json_dir)
INCLUDES +=  $(LOCAL_DIR)$(util_dir)

SOURCES += $(addprefix $(LOCAL_DIR)$(json_dir)/, $(json_sources))
SOURCES += $(addprefix $(LOCAL_DIR)$(util_dir)/, $(util_sources))
