CC = gcc
CFLAGS  = -O2 -Wall -std=gnu99 -I../api `pkg-config libusb-1.0 libudev --cflags`
LDFLAGS  = `pkg-config libusb-1.0 libudev --libs` -lpthread
OBJS = numledcli.o ../api/numled.o ../api/hid-libusb.o

all: numledcli

clean:
	rm -f numledcli.o ../api/*.o

numledcli: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
