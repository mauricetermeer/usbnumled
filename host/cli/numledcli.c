#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "hidapi.h"
#include "numled.h"

int main(int argc, char* argv[])
{
	const char *usage = "usage: numledcli {get|set} [digit0] [digit1] [digit2] [digit3] [brightness]\n";

	if (argc < 2) {
		printf(usage);
		return -1;
	}

	if (strcmp(argv[1], "get") == 0) {
		NumledHandle handle = numled_open(NULL);

		if (handle == NULL) {
			printf("Could not open device.\n");
			return -1;
		}

		int result;
		NumledState state = numled_read(handle, &result);

		if (result < 0) {
			printf("Could not read from device.\n");
		} else {
			printf("digit 0: %02x\ndigit 1: %02x\ndigit 2: %02x\ndigit 3: %02x\nbrightness: %02x\n",
				state.digits[0], state.digits[1], state.digits[2], state.digits[3], state.brightness
			);
		}

		numled_close(handle);
	} else if (strcmp(argv[1], "set") == 0) {
		NumledHandle handle = numled_open(NULL);

		if (handle == NULL) {
			printf("Could not open device.\n");
			return -1;
		}

		NumledState state;
		int value;

		if (argc >= 3) {
			sscanf(argv[2], "0x%02x", &value);
			state.digits[0] = value;
		}

		if (argc >= 4) {
			sscanf(argv[3], "0x%02x", &value);
			state.digits[1] = value;
		}

		if (argc >= 5) {
			sscanf(argv[4], "0x%02x", &value);
			state.digits[2] = value;
		}

		if (argc >= 6) {
			sscanf(argv[5], "0x%02x", &value);
			state.digits[3] = value;
		}

		if (argc >= 7) {
			sscanf(argv[6], "0x%02x", &value);
			state.brightness = value;
		}

		int result = numled_write(handle, &state);

		if (result < 0) {
			printf("Could not write to device.\n");
		}

		numled_close(handle);
	} else {
		printf(usage);
		return -1;
	}

	return 0;
}
