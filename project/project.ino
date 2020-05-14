/*
 * iSTEM project.
 * 
 * GROUP MEMBERS
 *  - Aiden G
 *  - Dana S
 *  - Bianca C
 *  - Shravanti HS
 *  
 *  CONFIG FILE
 *  A config file can be located in the working directory, "config.h.example".
 *  Remove the ".example" suffix and replace the values inside with your own
 */

// Imports
#include "config.h"
#include "AdafruitIO_WiFi.h"

// Set up feeds
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
AdafruitIO_Feed *input = io.feed(INPUT_FEED);
AdafruitIO_Feed *output = io.feed(OUTPUT_FEED);

// Button state variables
bool current = false;
bool last = false;

// Main functions
void setup() {
    // Initialise pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

    // Start the serial monitor
    Serial.begin(115200);
    Serial.print("\nConnecting to Adafruit IO");
    io.connect();

    // Set up a handler for the input
    input -> onMessage(handleInput);

    // Wait until connected to Adafruit IO
    while (io.status() < AIO_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.println(io.statusText());

    // Fetch the current status
    input -> get();
}

void loop() {
    // Let Adafruit do its connection behind-the-scenes stuff
    io.run();

    // Read the current state of the button
    if (digitalRead(BUTTON_PIN) == LOW)
        current = true;
    else
        current = false;

    // Return if the value hasn't changed
    if (current == last)
        return;

    // save the current state to the 'digital' feed on adafruit io
    Serial.print("PUSH -> ");
    if (current)
        Serial.println("HIGH");
    else
        Serial.println("LOW");

    // Save value
    output -> save(current);
    last = current;
}

void handleInput(AdafruitIO_Data *data) {
    Serial.print("PULL <- ");
    if (data -> toPinLevel() == HIGH)
        Serial.println("HIGH");
    else
        Serial.println("LOW");

    // Write input to LED
    digitalWrite(LED_PIN, data -> toPinLevel());
}