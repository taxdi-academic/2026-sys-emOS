#include "interrupt_handler.h"

volatile bool interrupted = false;

void initInterruptHandler() {
    interrupted = false;
}

void checkInterrupt() {
    if (Serial.available() > 0) {
        int c = Serial.peek();
        if (c == 3) { // ASCII 3 = Ctrl+C
            Serial.read(); // Consume the character
            interrupted = true;
            Serial.println("\r\n[Kernel] Interrupt detected (Ctrl+C)");
        }
    }
}
