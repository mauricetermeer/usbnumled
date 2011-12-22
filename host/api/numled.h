/*
 * Numled control interface
 * © 2011 Maurice Termeer
 */

#ifndef NUMLED_H
#define NUMLED_H

#include <stdint.h>

typedef void *NumledHandle;

typedef struct NumledState {
	uint8_t digits[4];
	uint8_t brightness;
} NumledState;

NumledHandle numled_open(wchar_t *serial);
int numled_write(NumledHandle handle, const NumledState &state);
NumledState numled_read(NumledHandle handle, int *result);
void numled_close(NumledHandle handle);

#endif // NUMLED_H

