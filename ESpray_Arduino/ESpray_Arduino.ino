// This example just provide basic function test;
// For more informations, please vist www.heltec.cn or mail to support@heltec.cn
#include <Bounce2.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

#define NUM_BUTTONS 3
const uint8_t BUTTON_PINS[NUM_BUTTONS] = {2,15,0};
const int MPU_addr=0x68; // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;


//OLED pins to ESP32 GPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16

#define SDA    4
#define SCL   15
#define RST   16 //RST must be set by software

#define V2     1

#ifdef V2 //WIFI Kit series V1 not support Vext control
  #define Vext  21
#endif
SSD1306  display(0x3c, SDA, SCL, RST);

#define DEMO_DURATION 3000
typedef void (*Demo)(void);

int demoMode = 0;
int counter = 1;

Bounce * buttons = new Bounce[NUM_BUTTONS];

void setup() {
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);    // OLED USE Vext as power supply, must turn ON Vext before OLED init
  delay(50); 

  Serial.begin(115200);
  Serial.println();
  Serial.println();


  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

    for (int i = 0; i < NUM_BUTTONS; i++) {
  buttons[i].attach(BUTTON_PINS[i],INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  buttons[i].interval(25); // Use a debounce interval of 25 milliseconds
  }
  Wire.begin(21,22,400000);
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
}

void rot() {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);    
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, "rot");
}

void gruen() {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);    
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, "gruen");
}
void schwarz() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);    
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, "Schwarz");
}

Demo demos[] = {schwarz,rot,gruen};

void loop() {
  // clear the display
  display.clear();
  // draw the current demo method
  demos[demoMode]();

    Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true); // request a total of 14 registers

  AcX=Wire.read()<<8|Wire.read();

for (int i = 0; i < NUM_BUTTONS; i++)  {
    // Update the Bounce instance :
    buttons[i].update();
  }
    // If it fell, flag the need to toggle the LED
    if ( !buttons[0].read() ) {
       Serial.print("x");
    }else if(!buttons[1].read()){
      Serial.print("y");
    }else if(!buttons[2].read()){
      Serial.print("z");
    }
    if (AcX > 16000) {
    Serial.print(AcX);
    }  

  // write the buffer to the display
  display.display();
  delay(16);
}
