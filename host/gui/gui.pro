TEMPLATE = app
TARGET = USBNumLED
DEPENDPATH += .
INCLUDEPATH += . ../api
unix { INCLUDEPATH += /usr/include/libusb-1.0 }
QT += network

HEADERS += numledgui.h
SOURCES += ../api/numled.c numledgui.cpp
unix { SOURCES += ../api/hid-libusb.c }
win32 { SOURCES += ../api/hid.c }
FORMS += numled_main.ui
unix { LIBS += -lrt -lusb-1.0 -ludev }
win32 { LIBS += setupapi.lib }
RESOURCES += numled.qrc
