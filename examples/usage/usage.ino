// Example usage for sds011 library by Alexey M.

#include "sds011-sh.h"

// Initialize objects from the lib
Sds011 sds011;

float pm25, pm10;

void setup() {
    // Call functions on initialized library objects that require hardware
    sds011.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    sds011.cmd_data_query(pm25, pm10);
}
