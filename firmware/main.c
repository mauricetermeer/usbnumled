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

#define DATA_SIZE 8
#define BUFFER_SIZE 9
uint8_t buffer[BUFFER_SIZE];

/*   5
 *  ---
 * |674|
 *  ---
 * |321|0
 *  --- .
 */

/*const uint8_t font[] = {
	//-------------76543210
	0x7e, // 0 = 0b01111110
	0x12, // 1 = 0b00010010
	0xbc, // 2 = 0b10111100
	0xb6, // 3 = 0b10110110
	0xd2, // 4 = 0b11010010
	0xe6, // 5 = 0b11100110
	0xee, // 6 = 0b11101110
	0x32, // 7 = 0b00110010
	0xfe, // 8 = 0b11111110
	0xf6, // 9 = 0b11110110
	0x01, // . = 0b00000001
	0x01, // . = 0b00000001
	0x01, // . = 0b00000001
	0x01, // . = 0b00000001
	0x01, // . = 0b00000001
	0x01, // . = 0b00000001
};*/

PROGMEM const char usbHidReportDescriptor[] = { /* USB report descriptor */
	0x06, 0x00, 0xff,			// USAGE_PAGE (Generic Desktop)
	0x09, 0x01,				// USAGE (Vendor Usage 1)
	0xa1, 0x01,				// COLLECTION (Application)
	0x15, 0x00,				//   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,			//   LOGICAL_MAXIMUM (255)
	0x75, 0x08,				//   REPORT_SIZE (8)
	0x95, 0x80,				//   REPORT_COUNT (128)
	0x09, 0x00,				//   USAGE (Undefined)
	0xb2, 0x02, 0x01,			//   FEATURE (Data,Var,Abs,Buf)
	0xc0					// END_COLLECTION
};

static uint8_t writeIndex;

uint8_t usbFunctionWrite(uint8_t *data, uint8_t len)
{
	uint8_t i;

	if (writeIndex == DATA_SIZE) return 1;
	if (len > DATA_SIZE - writeIndex) len = DATA_SIZE - writeIndex;

	for (i = writeIndex == 0 ? 1 : 0; i < len; ++i) {
		buffer[writeIndex++] = data[i];
	}

	return writeIndex == DATA_SIZE;
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

int main()
{
	uint8_t frame = 0, digit = 0;

	DDRB = 0xff;
	DDRD |= 0x2b;

	usbInit();
	sei();

	for (;;) {
		PORTD &= ~0x2b;
		PORTB = ~buffer[digit];

		if (buffer[4] > frame) {
			switch (digit) {
				case 0: PORTD |= 1 << 0; break;
				case 1: PORTD |= 1 << 1; break;
				case 2: PORTD |= 1 << 3; break;
				case 3: PORTD |= 1 << 5; break;
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

