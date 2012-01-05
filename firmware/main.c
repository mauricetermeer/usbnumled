/* Name: main.c
 * Project: hid-data, example how to use HID for data transfer
 * Author: Christian Starkjohann
 * Creation Date: 2008-04-11
 * Tabsize: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id: main.c 777 2010-01-15 18:34:48Z cs $
 */

/*
This example should run on most AVRs with only little changes. No special
hardware resources except INT0 are used. You may have to change usbconfig.h for
different I/O pins for USB. Please note that USB D+ must be the INT0 pin, or
at least be connected to INT0 as well.
*/

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include <avr/pgmspace.h>
#include "usbdrv.h"

PROGMEM const char usbHidReportDescriptor[] = { /* USB report descriptor */
	0x06, 0x00, 0xff,			// USAGE_PAGE (Generic Desktop)
	0x09, 0x01,				// USAGE (Vendor Usage 1)
	0xa1, 0x01,				// COLLECTION (Application)
	0x15, 0x00,				//   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,			//   LOGICAL_MAXIMUM (255)
	0x75, 0x08,				//   REPORT_SIZE (8)
	0x95, 0x08,				//   REPORT_COUNT (8)
	0x09, 0x00,				//   USAGE (Undefined)
	0xb2, 0x02, 0x01,			//   FEATURE (Data,Var,Abs,Buf)
	0xc0					// END_COLLECTION
};

#define BUFFER_SIZE 8
static uint8_t buffer[BUFFER_SIZE] = {
	0, 0, 0, 0, 0, 0, 0, 0
};

static uint8_t writeIndex;

uint8_t usbFunctionWrite(uint8_t *data, uint8_t len)
{
	uint8_t i;

	if (writeIndex == BUFFER_SIZE) return 1;
	if (len > BUFFER_SIZE - writeIndex) len = BUFFER_SIZE - writeIndex;

	for (i = 0; i < len; ++i) {
		buffer[writeIndex++] = data[i];
	}

	return writeIndex == BUFFER_SIZE;
}

/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uint8_t data[8])
{
	usbRequest_t *rq = (usbRequest_t*)data;

	if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
		if (rq->bRequest == USBRQ_HID_GET_REPORT) {
			usbMsgPtr = buffer;
			return BUFFER_SIZE;
		} else if (rq->bRequest == USBRQ_HID_SET_REPORT) {
			writeIndex = 0;
			return USB_NO_MSG;
		}
	}

	return 0;
}

int main(void)
{
	uint8_t frame = 0, digit = 0;

	usbInit();

	usbDeviceDisconnect();
	_delay_ms(250);
	usbDeviceConnect();

	DDRB = 0xff;
	DDRD |= 0x63;

	sei();

	for (;;) {
		PORTD &= ~0x63;
		PORTB = ~buffer[digit];

		if (buffer[4] > frame) {
			switch (digit) {
				case 0: PORTD |= 1 << 0; break;
				case 1: PORTD |= 1 << 1; break;
				case 2: PORTD |= 1 << 5; break;
				case 3: PORTD |= 1 << 6; break;
			}
		}

		digit = (digit + 1) & 3;

		if (digit == 0) {
			frame += 129;
		}

		usbPoll();
	}

	return 0;
}

