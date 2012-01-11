/*
 * Numled control interface
 * © 2011 Maurice Termeer
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "hidapi.h"
#include "numled.h"

#define BUFFER_SIZE 9

NumledHandle numled_open(wchar_t *serial)
{
	hid_device *handle = hid_open(0x16c0, 0x05df, serial);
	return (NumledHandle)handle;
}

int numled_write(NumledHandle handle, const NumledState *state)
{
	uint8_t buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	buffer[1] = state->digits[0];
	buffer[2] = state->digits[1];
	buffer[3] = state->digits[2];
	buffer[4] = state->digits[3];
	buffer[5] = state->brightness;
	return hid_send_feature_report(
		(hid_device*)handle, buffer, BUFFER_SIZE
	);
}

NumledState numled_read(NumledHandle handle, int *result)
{
	NumledState state;
	uint8_t buffer[BUFFER_SIZE];
	int bytes_read;

	memset(buffer, 0, sizeof(buffer));

	bytes_read =
		hid_get_feature_report(
			(hid_device*)handle, buffer, BUFFER_SIZE
		);

	if (bytes_read >= 0) {
		state.digits[0] = buffer[1];
		state.digits[1] = buffer[2];
		state.digits[2] = buffer[3];
		state.digits[3] = buffer[4];
		state.brightness = buffer[5];
	} else {
		state.digits[0] = 0;
		state.digits[1] = 0;
		state.digits[2] = 0;
		state.digits[3] = 0;
		state.brightness = 0;
	}

	if (result != NULL) *result = bytes_read;

	return state;
}

void numled_close(NumledHandle handle)
{
	hid_close((hid_device*)handle);
	hid_exit();
}

