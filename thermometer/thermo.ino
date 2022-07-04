/*
    Thermometer
    Thermometer Arduino Starter Kit project using supplied parts.

    created 28 June 2022
    by Taneli Hongisto

    This code is public domain
*/
#include <LiquidCrystal.h>
#include <TimeLib.h> // Time by Michael Margolis v1.6.1

#define TIME_HEADER "T" // Header tag for serial time sync message
#define TIME_REQUEST 7  // ASCII bell character requests a time sync message

const char CELSIUS_SYMBOL = 0b11011111;         // From LCD datasheet
const unsigned long SERIAL_OUT_DELAY = 60000UL; // ms

enum {
    D7 = 9, // LCD data pins
    D6 = 8,
    D5 = 7,
    D4 = 6,
    D3 = 5,
    D2 = 4,
    D1 = 3,
    D0 = 2,

    RS = 10, // LCD register switch
    E = 11,  // LCD enable

    TMP = A0, // Temperature sensor
};

int tmpRead;
double tmpV;
double temperature;
char lcdState[2][16],
    newOutput[2][16] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
unsigned long lastSerialOutput = 0xffffffffUL - SERIAL_OUT_DELAY;

LiquidCrystal lcd(RS, E, D0, D1, D2, D3, D4, D5, D6, D7);

void setup() {
    lcd.begin(16, 2); // Initialize LCD screen
    lcd.print("waiting for pc");
    lcd.setCursor(0, 1);
    lcd.print("to send time...");

    Serial.begin(9600);               // Initialize serial coms
    setSyncProvider(requestTimeSync); // Send time sync request through serial
    while (!Serial.available()) {     // Wait for response
        delay(10);
    }
    processTimeSyncMessage(); // Process serial response

    lcd.clear(); // Initialize LCD screen
    insertToStringAt(newOutput[0], (char *)"Temp:", 2, 6);
    updateLCDOutput(lcd, newOutput);
}

void loop() {
    time_t t = now();
    /*
        "TMP36 [has] an output scale factor of 10 mV/°C. [...] The TMP36 is
        specified from –40°C to +125°C, provides a 750 mV output at 25°C".

        ie. 0.75V => 25°C and 1V => 50°C
        Real temperature in celsius = (tmpV - 0.5) * 100
    */
    tmpRead = analogRead(TMP);
    tmpV = (5 * tmpRead) / 1024.0;
    temperature = (tmpV - 0.5) * 100;

    insertToStringAt(newOutput[0],
                     nonZeroOrNull(getDigitAsAscii(temperature, 10)), 8);
    insertToStringAt(newOutput[0], getDigitAsAscii(temperature, 1), 9);
    insertToStringAt(newOutput[0], '.', 10);
    insertToStringAt(newOutput[0], getDigitAsAscii(temperature, 0.1), 11);
    insertToStringAt(newOutput[0], CELSIUS_SYMBOL, 12);
    insertToStringAt(newOutput[0], 'C', 13);
    updateLCDOutput(lcd, newOutput);

    if (millis() - lastSerialOutput > SERIAL_OUT_DELAY) { // Overflow safe delta
        printCSVToSerial();
        lastSerialOutput = millis();
    }

    delay(1500);
}

/*
    Insert substring into string starting from index
    NOTE std::string would probably be more readable
*/
void insertToStringAt(char *string, char *substring, int startingIndex,
                      int substringLength) {
    for (int i = 0; i < substringLength; i++) {
        string[startingIndex + i] = substring[i];
    }
}
/*
    Insert character into string at index
*/
void insertToStringAt(char *string, char substring, int index) {
    insertToStringAt(string, &substring, index, 1);
}

/*
    Get single digit of number and convert to ASCII character
*/
char getDigitAsAscii(double number, double decimal) {
    return ((int)floor(number / decimal) % 10) + 48;
}

/*
    Return ASCII NULL if zero, otherwise character
*/
char nonZeroOrNull(char digit) { return digit == 48 ? 0 : digit; }

/*
    Update LCD screen
    Only changed characters updated to reduce flicker
*/
void updateLCDOutput(LiquidCrystal lcd, char newOutput[2][16]) {
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 15; i++) {
            if (lcdState[j][i] != newOutput[j][i]) {
                lcd.setCursor(i, j);
                lcd.print(newOutput[j][i]);
            }
        }
    }
}

/*
    Helper function to print zero-padded time values
*/
void printDigits(int digits) {
    if (digits < 10) Serial.print('0');
    Serial.print(digits);
}

/*
    Send data formatted as CSV to serial coms
*/
void printCSVToSerial() {
    Serial.print(year());
    Serial.print('-');
    Serial.print(month());
    Serial.print('-');
    Serial.print(day());
    Serial.print(' ');
    printDigits(hour());
    Serial.print(':');
    printDigits(minute());
    Serial.print(':');
    printDigits(second());
    Serial.print(".000");

    Serial.print(", ");

    Serial.print(temperature);
    Serial.println();
}

/*
    Request current time from serial connection
*/
time_t requestTimeSync() {
    Serial.write(TIME_REQUEST);
    return 0;
}

/*
    Process serial current time response
*/
void processTimeSyncMessage() {
    Serial.find(TIME_HEADER) ? setTime(Serial.parseInt()) : setTime(0);
}
