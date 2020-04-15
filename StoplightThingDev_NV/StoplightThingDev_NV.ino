#include <Wire.h>

// Headers for persistent memory
#include <EEPROM.h>

#include "EspMQTTClient.h"

#include <FastLED.h>

#define NUM_LEDS 64 
#define LED_PIN 14
#define LED_TYPE    WS2812
#define BRIGHTNESS  100
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 40

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedPalette;
extern const TProgmemPalette16 myRedPalette_p PROGMEM;

extern CRGBPalette16 myYellowPalette;
extern const TProgmemPalette16 myYellowPalette_p PROGMEM;

extern CRGBPalette16 myGreenPalette;
extern const TProgmemPalette16 myGreenPalette_p PROGMEM;

extern CRGBPalette16 myRainbowPalette;
extern const TProgmemPalette16 myRainbowPalette_p PROGMEM;

#define REDLED 16
#define YELLOWLED 12
#define GREENLED 13
#define BUTTON 5

int redState, yellowState, greenState;

int counter = 0;
String LEDstatus = "Stoplight Ready!";

typedef struct
{
  char ssid[16];
  char password[16];
  char brokerIP[20];
 
} nv_data_type;

nv_data_type nv_data;

  EspMQTTClient client(
  nv_data.ssid,
  nv_data.password,
  nv_data.brokerIP 
  );

void setup() {
  Wire.begin();
  EEPROM.begin(sizeof(nv_data_type));
  EEPROM.get(0, nv_data);
  
  Serial.begin(115200);
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  

  pinMode(REDLED, OUTPUT);
  pinMode(YELLOWLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);  
  pinMode(BUTTON, INPUT_PULLUP);
  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  
  currentPalette = myRainbowPalette_p;
  currentBlending = LINEARBLEND;

  showLEDs();
  publishStatus();
  
}

void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe("stopLight/status", [](const String & payload) 
  {
    Serial.print("Received: ");
    Serial.println(payload);
    if (payload == "Stoplight READY!")
      counter = 0;   
    if (payload == "RED")
      counter = 1;
    if (payload == "YELLOW")
      counter = 2; 
    if (payload == "GREEN")
      counter = 3; 
    showLEDs(); 
  }
  );

  // Publish a message to "mytopic/test"
  client.publish("stopLight/status", LEDstatus); // You can activate the retain flag by setting the third parameter to true
  delay(50); // avoid bogging up serial monitor

 
}

void loop() {
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */
    
  FillLEDsFromPaletteColors( startIndex);
  
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
  
  if (digitalRead(BUTTON)==LOW)
  {
    counter++;
    if (counter > 3) 
      counter = 1;
    delay(250);
    showLEDs(); 
    publishStatus(); 
  }
   
  client.loop();
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

void showLEDs(){
    if (counter == 0)
    {
    redState = HIGH;
    yellowState = HIGH;
    greenState = HIGH;
    LEDstatus="Stoplight READY!";
    currentPalette = myRainbowPalette_p;
    }
    else if (counter == 1)
    {
      redState = HIGH;
      yellowState = LOW;
      greenState = LOW;
      LEDstatus="RED";
      currentPalette = myRedPalette_p;
    }
   else if (counter == 2)
    {
      redState = LOW;
      yellowState = HIGH;
      greenState = LOW;
      LEDstatus="YELLOW";
      currentPalette = myYellowPalette_p;
    }
   else if (counter == 3)
    {
      redState = LOW;
      yellowState = LOW;
      greenState = HIGH;
      LEDstatus = "GREEN";  
      currentPalette = myGreenPalette_p;
    }  
    digitalWrite(REDLED, redState);
    digitalWrite(YELLOWLED, yellowState);
    digitalWrite(GREENLED, greenState);
    
}

void publishStatus(){
  client.publish("stopLight/status", LEDstatus);

}

const TProgmemPalette16 myRainbowPalette_p PROGMEM =
{
    CRGB::Indigo,
    CRGB::Yellow, 
    CRGB::DarkMagenta,
    CRGB::Red,
    
    CRGB::BlueViolet,
    CRGB::White,
    CRGB::Black,
    CRGB::Indigo,
    
    CRGB::Purple,
    CRGB::DodgerBlue,
    CRGB::Black,
    CRGB::DarkOrchid,
    
    CRGB::Red,
    CRGB::BlueViolet,
    CRGB::Green,
    CRGB::Purple
};

const TProgmemPalette16 myRedPalette_p PROGMEM =
{
    CRGB::Crimson,
    CRGB::FireBrick, // 'white' is too bright compared to red and blue
    CRGB::Crimson,
    CRGB::Fuchsia,
    
    CRGB::Crimson,
    CRGB::FireBrick,
    CRGB::Crimson,
    CRGB::FireBrick,
    
    CRGB::FireBrick,
    CRGB::Fuchsia,
    CRGB::FireBrick,
    CRGB::Red,
    
    CRGB::OrangeRed,
    CRGB::Red,
    CRGB::OrangeRed,
    CRGB::Red
};

const TProgmemPalette16 myYellowPalette_p PROGMEM =
{
    CRGB::Wheat,
    CRGB::Gold, 
    CRGB::DarkGoldenrod,
    CRGB::Yellow,
    
    CRGB::Yellow,
    CRGB::Orange,
    CRGB::DarkGoldenrod,
    CRGB::Gold,
    
    CRGB::LightYellow,
    CRGB::DarkGoldenrod,
    CRGB::Orange,
    CRGB::Gold,
    
    CRGB::Wheat,
    CRGB::Coral,
    CRGB::Yellow,
    CRGB::DarkOrange
};

const TProgmemPalette16 myGreenPalette_p PROGMEM =
{
    CRGB::LimeGreen,
    CRGB::ForestGreen, // 'white' is too bright compared to red and blue
    CRGB::Green,
    CRGB::MediumSeaGreen,
    
    CRGB::Green,
    CRGB::LimeGreen,
    CRGB::Lime,
    CRGB::Chartreuse,
    
    CRGB::MediumSeaGreen,
    CRGB::ForestGreen,
    CRGB::Chartreuse,
    CRGB::Green,
    
    CRGB::DarkSeaGreen,
    CRGB::GreenYellow,
    CRGB::Green,
    CRGB::Chartreuse
};
