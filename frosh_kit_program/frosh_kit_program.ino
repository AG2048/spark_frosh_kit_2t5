// Spark Design Club 2T5 F!rosh PCB Program
// Copyright (C) 2025 Andy Gong
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Define pin mappings corresponding to https://github.com/kimio-kosaka/bitDuino10-arduinoTPI/blob/master/variants/bitDuino10/pins_arduino.h
#define INPUT_PORT 0
#define OUTPUT_PORT 1
#define BATTERY_PORT 2

// Define delays for final circular output
#define X_INIT_VALUE 250
#define X_DEC_VALUE 25

// Global variables
bool is_main = false;
bool has_prev = false;
int x = X_INIT_VALUE;
int i = 0;

// Util functions to wait for signal edges
void wait_falling_edge() {
    while (digitalRead(INPUT_PORT) != 1);
    while (digitalRead(INPUT_PORT) != 0);
}

void wait_rising_edge() {
    while (digitalRead(INPUT_PORT) != 0);
    while (digitalRead(INPUT_PORT) != 1);
}

void wait_high(){
    while (digitalRead(INPUT_PORT) != 1);
}

void wait_low(){
    while (digitalRead(INPUT_PORT) != 0);
}

void setup() {
    // Initialize pin modes
    pinMode(INPUT_PORT, INPUT_PULLUP);
    pinMode(OUTPUT_PORT, OUTPUT);
    pinMode(BATTERY_PORT, INPUT);
    digitalWrite(OUTPUT_PORT, LOW);
    delay(500);

    // Determine if the device is the main one (based on BATTERY_PORT reading)
    if (digitalRead(BATTERY_PORT)) {
        is_main = true;
    } else {
        is_main = false;
    }

    // Determine if the board has any previous board attached
    if (digitalRead(INPUT_PORT) == LOW) {
        has_prev = true;
    } else {
        has_prev = false;
    }
    delay(500);
}

void loop() {
    if (!has_prev) {
        // In the event of the board having no previous board attached, do constant high
        digitalWrite(OUTPUT_PORT, HIGH);
    } else {
        // If the board has previous board connected, enable more complex patterns
        if (is_main) {
            // Main board initiates patterns

            // Initial ON/OFF 500ms
            digitalWrite(OUTPUT_PORT, HIGH);
            delay(500);
            digitalWrite(OUTPUT_PORT, LOW);
            delay(500);
            
            // First 250ms Loop
            digitalWrite(OUTPUT_PORT, HIGH);
            delay(250);
            digitalWrite(OUTPUT_PORT, LOW);
            wait_falling_edge();
            // Second 250ms Loop
            digitalWrite(OUTPUT_PORT, HIGH);
            delay(800);

            // All turn off for a brief moment
            digitalWrite(OUTPUT_PORT, LOW);
            delay(10);

            // 500ms alternate
            digitalWrite(OUTPUT_PORT, HIGH);
            delay(500);
            digitalWrite(OUTPUT_PORT, LOW);
            delay(500);
            digitalWrite(OUTPUT_PORT, HIGH);
            delay(500);
            digitalWrite(OUTPUT_PORT, LOW);
            delay(500);

            // Speeding up
            delay(100);
            x = X_INIT_VALUE;
            while (x > X_DEC_VALUE) {
                digitalWrite(OUTPUT_PORT, HIGH);
                delay(x);
                digitalWrite(OUTPUT_PORT, LOW);
                x -= X_DEC_VALUE;
                wait_falling_edge();
            }

            // On 1500 before repeating
            digitalWrite(OUTPUT_PORT, HIGH);
            delay(1500);
        } else {
            // Secondary board follows main board's lead

            // Initially be equal to main until falling edge
            wait_high();
            digitalWrite(OUTPUT_PORT, HIGH);
            wait_falling_edge(); // If main board has no previous board, this falling edge will never be reached
            digitalWrite(OUTPUT_PORT, LOW);

            // Loop 1
            wait_falling_edge();
            digitalWrite(OUTPUT_PORT, HIGH);
            delay(250);
            digitalWrite(OUTPUT_PORT, LOW);

            // Loop 2 -- keep on after
            wait_high();
            delay(250);
            digitalWrite(OUTPUT_PORT, HIGH);

            // Wait for falling edge initiated by main
            wait_falling_edge();
            digitalWrite(OUTPUT_PORT, LOW);
            
            // The next alternating sequence will last 2000 ms, so do 10ms periodic check
            for (i = 0; i < 200; i++) {
                delay(10);
                if (digitalRead(INPUT_PORT) == LOW) {
                    digitalWrite(OUTPUT_PORT, HIGH);
                } else {
                    digitalWrite(OUTPUT_PORT, LOW);
                }
            }

            // Write LOW to begin the final cycle
            digitalWrite(OUTPUT_PORT, LOW);
            delay(100);

            // Cycle: light after falling edge, duration speeds up
            x = X_INIT_VALUE;
            while (x > X_DEC_VALUE) {
                wait_falling_edge();
                digitalWrite(OUTPUT_PORT, HIGH);
                delay(x);
                digitalWrite(OUTPUT_PORT, LOW);
                x -= X_DEC_VALUE;
            }

            // On 1500 before repeating
            digitalWrite(OUTPUT_PORT, HIGH);
            delay(1500);
        }
    }
}