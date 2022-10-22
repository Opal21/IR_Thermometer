#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>
#include <Wire.h>
#include <millisDelay.h>

#define SCREEN_WIDTH 128 // OLED display width [px]
#define SCREEN_HEIGHT 64 // OLED display height [px]

const int PROXIMITY_SENSOR = 9; // pin D9 for IR proximity sensor
const int RED_LED = 5;          // pin D5 for red LED

bool isTempMeasured = false; 

const unsigned long delay_hold_red = 5000; // LED delay hold [ms]
millisDelay holdDelay;  

const unsigned long interval_sensor = 50; // interval refresh sensor [mS] 
millisDelay sensorDelay;

const unsigned long interval_display = 500; // interval refresh display [mS] 
millisDelay displayDelay;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
    Serial.begin(9600);

    pinMode(RED_LED, OUTPUT);
    pinMode(PROXIMITY_SENSOR, INPUT);
  
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("OLED Display allocation failed")); // Address 0x3D for 128x64
        for(;;);
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(20, 20);
    display.println("Initializing");
    display.display();
    delay(1500);
    display.clearDisplay();
    displayDelay.start(interval_display);

    mlx.begin();
}

void loop() {
    static float temp = -1;
    int state = digitalRead(PROXIMITY_SENSOR);

    if (state == LOW && isTempMeasured == false) {
        digitalWrite(RED_LED, HIGH);
        sensorDelay.start(interval_sensor);
        displayDelay.finish();
        isTempMeasured = true;
    } else if (isTempMeasured == true) {
        temp = getTemp();
    } else {
        temp = -1;
    }
    
    showTemp(temp);
    holdReading();
}

float getTemp() {
    static int i = 0;
    static float tempSum = 0;
    float result = 0;

    if (sensorDelay.justFinished()) {
        sensorDelay.repeat();
        tempSum += mlx.readObjectTempC();
        i++;
    }

    if(i == 19) {
        result = tempSum / 20;
        tempSum = 0;
        i = 0;
        sensorDelay.stop(); 
        displayDelay.finish();
    }

  return result;
}

void showTemp(float temperature) {
    if (displayDelay.justFinished()) {
        displayDelay.repeat();

        if(temperature == -1) { // if there are no object in front of sensor
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
        } else if(temperature == 0) { // if still reading temperature
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(WHITE);
            display.setCursor(20, 25);
            display.println("WAIT ....");
            display.display();
        } else { // if there is a new result
            temperature += 8.5;
            display.clearDisplay();     
            display.setTextSize(3);
            display.setCursor(10, 20);
            display.print(temperature,1);      
            display.print(" C");
            display.display();
            displayDelay.stop();
        }
    }
}

void  holdReading() {
    if (holdDelay.justFinished()) {
        digitalWrite(RED_LED, LOW);
        isTempMeasured = false;
        displayDelay.start(interval_display);
  }
}
