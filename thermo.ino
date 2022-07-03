/*
    Thermometer
    Thermometer Arduino Starter Kit project using supplied parts.

    created 28 June 2022
    by Taneli Hongisto

    THIS CODE IS PUBLIC DOMAIN
*/
/*
    TODO Serial output? then
    TODO    Outputting and plotting statistics
*/
#include <LiquidCrystal.h>

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

const uint8_t CELSIUS_SYMBOL = 0b11011111; // From LCD datasheet

int tmpRead;
double tmpV;
double temperature;
uint8_t lcdState[2][16],
    newOutput[2][16] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

LiquidCrystal lcd(RS, E, D0, D1, D2, D3, D4, D5, D6, D7);

/*
    Insert substring into string starting from index
    NOTE std::string would probably be more readable
*/
void insertToStringAt(uint8_t *string, uint8_t *substring, int startingIndex,
                      int substringLength) {
    for (int i = 0; i < substringLength; i++) {
        string[startingIndex + i] = substring[i];
    }
}

/*
    Insert uint8_tacter into string at index
*/
void insertToStringAt(uint8_t *string, uint8_t substring, int index) {
    insertToStringAt(string, &substring, index, 1);
}

/*
    Get single digit of number and convert to ASCII uint8_tacter
*/
uint8_t getDigitAsAscii(double number, double decimal) {
    return ((int)floor(number / decimal) % 10) + 48;
}

/*
    Return ASCII NULL if zero, otherwise uint8_tacter
*/
uint8_t nonZeroOrNull(uint8_t digit) { return digit == 48 ? 0 : digit; }

/*
    Update LCD screen
    Only changed uint8_tacters updated to reduce flicker
*/
void updateLCDOutput(LiquidCrystal lcd, uint8_t newOutput[2][16]) {
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 15; i++) {
            if (lcdState[j][i] != newOutput[j][i]) {
                lcd.setCursor(i, j);
                lcd.print(newOutput[j][i]);
                delay(1);
            }
        }
    }
}

void setup() {
    Serial.begin(9600); // Initialize serial coms
    lcd.begin(16, 2);   // Initialize LCD screen

    insertToStringAt(newOutput[0], (uint8_t *)"Temp:", 2, 6);
    updateLCDOutput(lcd, newOutput);
}

void loop() {
    /*
        "TMP36 [has] an output scale factor of 10 mV/°C. [...] The TMP36 is
        specified from –40°C to +125°C, provides a 750 mV output at 25°C".

        ie. 0.75V => 25°C and 1V => 50°C
        Real temperature in celsius = (tmpV - 0.5) * 100
    */
    tmpRead = analogRead(TMP);
    tmpV = (5 * tmpRead) / 1024.0;
    temperature = (tmpV - 0.5) * 100;

    Serial.println("temperature: " + String(temperature)); // debug

    insertToStringAt(newOutput[0],
                     nonZeroOrNull(getDigitAsAscii(temperature, 10)), 8);
    insertToStringAt(newOutput[0], getDigitAsAscii(temperature, 1), 9);
    insertToStringAt(newOutput[0], '.', 10);
    insertToStringAt(newOutput[0], getDigitAsAscii(temperature, 0.1), 11);
    insertToStringAt(newOutput[0], CELSIUS_SYMBOL, 12);
    insertToStringAt(newOutput[0], 'C', 13);
    updateLCDOutput(lcd, newOutput);

    delay(1500);
}
