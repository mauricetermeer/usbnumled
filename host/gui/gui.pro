TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . ../api /usr/include/libusb-1.0

HEADERS += numledgui.h
SOURCES += ../api/hid-libusb.c ../api/numled.cpp numledgui.cpp
FORMS += numled_main.ui
LIBS += -lrt -lusb-1.0 -ludev
