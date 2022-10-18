#include <millisDelay.h>
#include <Wire.h> 
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

const int statePin = 9;     // pin D9 for IR proximity sensor
const int RED_LED = 5;      // pin D5 for red LED
const unsigned long delay_hold_red = 5000; // delay hold red LED in mS

const unsigned long interval_display = 500; // interval refresh OLED display in mS 
millisDelay displayDelay;                   // the delay object

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
    Serial.begin(9600);

    pinMode(RED_LED, OUTPUT);
    pinMode(statePin, INPUT);
  
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
        Serial.println(F("OLED Display allocation failed"));
        for(;;);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(20, 20);
    display.println("Initializing");
    display.display();
    delay(2500);
    display.clearDisplay();
  
    displayDelay.start(interval_display); //refresh display
}

void loop() {
    int state = digitalRead(statePin);  //IR sensor condition

    if (state == LOW) {
    digitalWrite(RED_LED, HIGH);
  }

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(35, 5);
    display.print("-----");
    display.setCursor(105, 20);
    display.print("");
    display.setTextSize(2);
    display.setCursor(35, 40);
    display.print("-----");
    display.setCursor(105, 46);
    display.print("");
    display.display();
    delay(25000);
    displayDelay.stop(); //stop refresh display
}