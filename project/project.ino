/*
 * iSTEM project.
 * 
 *  CONFIG FILE
 *  A config file can be located in the working directory, "config.h.example".
 *  Remove the ".example" suffix and replace the values inside with your own
 */

// Imports
#include "config.h"
#include "pitches.h"
#include "AdafruitIO_WiFi.h"

// Set up feeds
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
AdafruitIO_Feed *input = io.feed(INPUT_FEED);
AdafruitIO_Feed *output = io.feed(OUTPUT_FEED);

// Button state variables
bool current = false;
bool last = false;

// Melody data
unsigned long lastBuzz = 0;
int melody[][2] = {
    {NOTE_C4, 4},
    {NOTE_G3, 8},
    {NOTE_G3, 8},
    {NOTE_A3, 4},
    {NOTE_G3, 4},
    {0, 4},
    {NOTE_B3, 4},
    {NOTE_C4, 4}
};

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

    // Update internal and Adafruit
    input -> get();
    output -> save(digitalRead(BUTTON_PIN) == LOW);
}

void loop() {
    io.run();
    handleOutput();
}

void handleOutput() {
    // Read the current state of the button
    if (digitalRead(BUTTON_PIN) == LOW)
        current = true;
    else
        current = false;

    // Return if the value hasn't changed
    if (current == last)
        return;

    // Save the current state on Adafruit
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
    if (PLAY_TUNE) {
        handlePiezo();
    }
}

void handlePiezo() {
    lastBuzz = millis();
    unsigned long thisBuzz = lastBuzz;
    int length = sizeof(melody)/sizeof(melody[0]);
    
    for (int thisNote = 0; thisNote < length; thisNote++) {
        // Check if a new tune has been triggered since this one started
        if (lastBuzz != thisBuzz) {return;}

        // Calculate note duration and play note
        int noteDuration = 1000 / melody[thisNote][1];
        tone(PIEZO_PIN, melody[thisNote][0], noteDuration);

        // Add a delay between notes to distinguish them
        int pauseBetweenNotes = noteDuration * 1.30;
        unsigned long startMillis = millis();
        
        // Simulate `loop` function
        io.run();
        handleOutput();

        // Calculate remaining delay required
        int delayRequired = pauseBetweenNotes-(millis()-startMillis);
        if (delayRequired >= 0) {delay(delayRequired);}

        noTone(PIEZO_PIN);
    }
}
