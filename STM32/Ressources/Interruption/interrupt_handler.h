#pragma once
#include <Arduino.h>

// Global variable indicating if an interrupt has been triggered
extern volatile bool interrupted;

// Initialize interrupt handling
void initInterruptHandler();

// Periodically check if Ctrl+C has been received
void checkInterrupt();
