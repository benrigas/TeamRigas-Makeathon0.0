// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

#define GREEN_BUTTON 8
#define WHITE_BUTTON 7
#define YELLOW_BUTTON 4
#define BLUE_BUTTON 2

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 60

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

unsigned long lastDebounceTimes[10] = {0};
int lastButtonStates[10] = {LOW};
int buttonStates[10];

int waitingForPlayer = 0;

int currentLevel = 1;
int consecutiveMatchesAtLevel = 0;

int greenSequence = 1;
int whiteSequence = 2;
int yellowSequence = 3;
int blueSequence = 4;

int currentSequenceToMatch[100] = {0};
int currentInputSequence[100] = {0};
int currentInputNextIndex = 0;

//int ledState = HIGH;         // the current state of the output pin
//int buttonState;             // the current reading from the input pin
//int lastButtonState = LOW;   // the previous reading from the input pin
//unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
int buttonReadCountStartupHack = 0;
bool gameStartupReady = false;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.


// setup() function -- runs once at startup --------------------------------

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  Serial.begin(9600);
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(WHITE_BUTTON, INPUT_PULLUP);
  pinMode(YELLOW_BUTTON, INPUT_PULLUP);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);
  randomSeed(analogRead(0));
}


// loop() function -- runs repeatedly as long as board is on ---------------


void showYellow(int delayMS) {
  delay(delayMS);
  colorWipe(strip.Color(255,   255,   0), 0);
  delay(delayMS);
  strip.clear();
  strip.show();
}

void showWhite(int delayMS) {
  delay(delayMS);
  colorWipe(strip.Color(255, 255, 255), 0);
  delay(delayMS);
  strip.clear();
  strip.show();
}

void showGreen(int delayMS) {
  delay(delayMS);
  colorWipe(strip.Color(0, 255, 0), 0);
  delay(delayMS);
  strip.clear();
  strip.show();
}

void showBlue(int delayMS) {
  delay(delayMS);
  colorWipe(strip.Color(0, 0, 255), 0);
  delay(delayMS);
  strip.clear();
  strip.show();
}

void levelUp() {
  Serial.println("Level up, now it gets harder!");
  currentLevel++;
  consecutiveMatchesAtLevel = 0;
}

void gameOverMan() {
  Serial.println("Game over man!");

  delay(100);
  colorWipe(strip.Color(255, 0, 0), 100);
  delay(500);
  strip.clear();
  strip.show();
  
  currentLevel = 1;
  consecutiveMatchesAtLevel = 0;
}

int delayForCurrentLevel() {
  switch(currentLevel) {
    case 1:
      return 500;
      break;
     case 2:
      return 450;
      break;
     case 3:
      return 400;
      break;
     case 4:
      return 350;
      break;
     case 5:
      return 300;
      break;
     case 6:
      return 200;
      break;
     default:
      return 100; 
  }
}

void setCurrentSequenceForLevel() {
  memset(currentSequenceToMatch, 0, sizeof(currentSequenceToMatch));
  memset(currentInputSequence, 0, sizeof(currentInputSequence));

  if (currentLevel == 1) {
    currentSequenceToMatch[0] = random(1, 5);
    currentSequenceToMatch[1] = random(1, 5);
    currentSequenceToMatch[2] = random(1, 5);
  } else if (currentLevel == 2) {
    currentSequenceToMatch[0] = random(1, 5);
    currentSequenceToMatch[1] = random(1, 5);
    currentSequenceToMatch[2] = random(1, 5);
    currentSequenceToMatch[3] = random(1, 5);
  } else if (currentLevel == 3) {
    currentSequenceToMatch[0] = random(1, 5);
    currentSequenceToMatch[1] = random(1, 5);
    currentSequenceToMatch[2] = random(1, 5);
    currentSequenceToMatch[3] = random(1, 5);
    currentSequenceToMatch[4] = random(1, 5);
  } else if (currentLevel == 4) {
    currentSequenceToMatch[0] = random(1, 5);
    currentSequenceToMatch[1] = random(1, 5);
    currentSequenceToMatch[2] = random(1, 5);
    currentSequenceToMatch[3] = random(1, 5);
    currentSequenceToMatch[4] = random(1, 5);
    currentSequenceToMatch[5] = random(1, 5);
  } else if (currentLevel == 5) {
    currentSequenceToMatch[0] = random(1, 5);
    currentSequenceToMatch[1] = random(1, 5);
    currentSequenceToMatch[2] = random(1, 5);
    currentSequenceToMatch[3] = random(1, 5);
    currentSequenceToMatch[4] = random(1, 5);
    currentSequenceToMatch[5] = random(1, 5);
    currentSequenceToMatch[6] = random(1, 5);
  } else if (currentLevel == 6) {
    currentSequenceToMatch[0] = random(1, 5);
    currentSequenceToMatch[1] = random(1, 5);
    currentSequenceToMatch[2] = random(1, 5);
    currentSequenceToMatch[3] = random(1, 5);
    currentSequenceToMatch[4] = random(1, 5);
    currentSequenceToMatch[5] = random(1, 5);
    currentSequenceToMatch[6] = random(1, 5);
    currentSequenceToMatch[7] = random(1, 5 );
  }
}

void flashCurrentSequence() {
  Serial.print("New Sequence: ");
    for (int x = 0; x < 100; x++) {
      if (currentSequenceToMatch[x] == greenSequence) {
        showGreen(delayForCurrentLevel());
        Serial.print("green ");
      } else if (currentSequenceToMatch[x] == whiteSequence) {
        showWhite(delayForCurrentLevel());
        Serial.print("white ");
      } else if (currentSequenceToMatch[x] == yellowSequence) {
        showYellow(delayForCurrentLevel());
        Serial.print("yellow ");
      } else if (currentSequenceToMatch[x] == blueSequence) {
        showBlue(delayForCurrentLevel());
        Serial.print("blue ");
      }
    }
//    strip.clear();
//    strip.show();
}

void appendToCurrentInput(int pinNumber) {
  if (!gameStartupReady) { return; }
  
  if (pinNumber == GREEN_BUTTON) {
    currentInputSequence[currentInputNextIndex] = greenSequence;
    Serial.println("append green");
  } else if (pinNumber == WHITE_BUTTON) {
    currentInputSequence[currentInputNextIndex] = whiteSequence;
    Serial.println("append white");
  } else if (pinNumber == YELLOW_BUTTON) {
    currentInputSequence[currentInputNextIndex] = yellowSequence;
    Serial.println("append yellow");
  } else if (pinNumber == BLUE_BUTTON) {
    currentInputSequence[currentInputNextIndex] = blueSequence;
    Serial.println("append blue");
  }
  currentInputNextIndex++;
}

int sizeForCurrentMatch() {
  int c = 0;
  for (int x = 0; x < 100; x++) {
    if (currentSequenceToMatch[x] != 0) {
      c++;
    } else {
      break;
    }
  }
  Serial.print("Size for currentSequenceToMatch = ");
  Serial.println(c);
  return c;
}

int sizeForCurrentInput() {
  int c = 0;
  for (int x = 0; x < 100; x++) {
    if (currentInputSequence[x] != 0) {
      c++;
    } else {
      break;
    }
  }
  Serial.print("Size for current input = ");
  Serial.println(c);
  return c;
}

void checkCurrentInputForMatch() {
  if (!gameStartupReady) { return; }
  
  if (sizeForCurrentInput() < sizeForCurrentMatch()) {
    Serial.println("Not same count yet, skip check");
    return;
  }

  bool matches = true;

  for (int x = 0; x < 100; x++) {
    if (currentInputSequence[x] == currentSequenceToMatch[x]) {
      continue;
    } else {
      matches = false;
      break;
      // NO MATCH -> YOU LOSE
    }
  }

  if (matches) {
    consecutiveMatchesAtLevel++;
    if (consecutiveMatchesAtLevel == 2) {
      levelUp();
    } else {
      Serial.println("Match, good job!");
    }
  } else {
    gameOverMan();
  }
  currentInputNextIndex = 0;
  waitingForPlayer = 0;
}

void readPin(int pinNumber) {
  int reading = digitalRead(pinNumber);

    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonStates[pinNumber]) {
      // reset the debouncing timer
      lastDebounceTimes[pinNumber] = millis();
    }
  
   if ((millis() - lastDebounceTimes[pinNumber]) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
      if (reading != buttonStates[pinNumber]) {
        
        buttonStates[pinNumber] = reading;

        if (buttonStates[pinNumber] == HIGH) {
          Serial.print(pinNumber);
          Serial.println("BUTTON");
          buttonReadCountStartupHack++; // wait for all the buttons to be read once for some reason

          appendToCurrentInput(pinNumber);
          checkCurrentInputForMatch();
//          waitingForPlayer = 0;
          //colorWipe(strip.Color(255,   0,   0), 0); // Red
        }
      }
    }

    lastButtonStates[pinNumber] = reading;
}


/*
void loop() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(YELLOW_BUTTON);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
//        ledState = !ledState;
        Serial.println("BUTTON");
      }
    }
  }

  // set the LED:
//  digitalWrite(ledPin, ledState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}
*/

void loop() {
    readPin(GREEN_BUTTON);
    readPin(WHITE_BUTTON);
    readPin(YELLOW_BUTTON);
    readPin(BLUE_BUTTON);
    
  if (buttonReadCountStartupHack <= 4) {
//      rainbow(1);             // Flowing rainbow cycle along the whole strip
    return;
  } else {
    gameStartupReady = true;
  }

  if (waitingForPlayer == 1) {
    readPin(GREEN_BUTTON);
    readPin(WHITE_BUTTON);
    readPin(YELLOW_BUTTON);
    readPin(BLUE_BUTTON);
  } else {
    Serial.println("Starting level");
    setCurrentSequenceForLevel();
    flashCurrentSequence();

    waitingForPlayer = 1;
  }
//  
//  // Fill along the length of the strip in various colors...
//  colorWipe(strip.Color(255,   0,   0), 50); // Red
//  colorWipe(strip.Color(  0, 255,   0), 50); // Green
//  colorWipe(strip.Color(  0,   0, 255), 50); // Blue
//
//  // Do a theater marquee effect in various colors...
//  theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
//  theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
//  theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness
//
//  rainbow(10);             // Flowing rainbow cycle along the whole strip
//  theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
}


// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}


// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
