CC = g++
CFLAGS  = -O2 -Wall -I../api `pkg-config libusb-1.0 libudev --cflags`
LDFLAGS  = `pkg-config libusb-1.0 libudev --libs` -lpthread
OBJS = numledcli.o ../api/numled.o ../api/hid-libusb.o

all: numledcli

clean:
	rm -f numledcli.o

numledcli: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

.cpp.o:
	$(CC) -c $(CFLAGS) $< -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
