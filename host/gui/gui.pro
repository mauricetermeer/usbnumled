TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . /usr/include/libusb-1.0

HEADERS += hidapi.h numled.h numledgui.h
SOURCES += hid-libusb.c numled.cpp numledgui.cpp
FORMS += numled_main.ui
LIBS += -lrt -lusb-1.0 -ludev
