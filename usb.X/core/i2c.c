#include "i2c.h"

void send_serial(int* port, BYTE* data, int bits) {
    int width = 0;
    for (int i = 0; i < bits; i++, width++) {
        if (width == 2) {
            int temp = *port & 1;
            if (temp) {
                *port &= ~temp;
                continue;
            } else {
                *port |= temp;
            }
            width = 0;
        }
        width++;
    }
}
