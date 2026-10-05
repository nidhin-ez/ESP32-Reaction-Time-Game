#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define screenWidth 128
#define screenHeight 64
#define led 4
#define button 13



Adafruit_SSD1306 display(screenWidth,screenHeight,&Wire,-1);

unsigned long ledOnTime = 0;
unsigned long roundStartTime = 0;
byte stableButtonState = HIGH;
byte lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceTime = 50;
unsigned long buttonPressedTime = 0;
byte previousStableButtonState = HIGH;
unsigned long randomTimeLedStart;
bool waiting =false;
unsigned long reaction_time =0;
unsigned long bestTime = 0;

enum gameState {
    w8fLedOn,
    w8fButtonPress,
    result,
    waitForNextRound
};

gameState state = w8fLedOn;
gameState previousState =waitForNextRound ;

void setup() {

    Serial.begin(115200);
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
        Serial.println("displaynotFound");
        while(1);
    }
    pinMode(button, INPUT_PULLUP);
    pinMode(led, OUTPUT);

    randomTimeLedStart = random(3000, 8000);
    
}

void loop() {
    
    unsigned long currentTime = millis();
    if(state != previousState){
        switch(state){
            case w8fLedOn:{
                display.clearDisplay();
                display.setTextSize(2);
                display.setTextColor(SSD1306_WHITE);
                display.setCursor(5,25);
                display.println("GET READY!");
                display.display();
                break;
            }
            case w8fButtonPress:{
                display.clearDisplay();
                display.setTextSize(2);
                display.setTextColor(SSD1306_WHITE);
                display.setCursor(40,25);
                display.print("GO!!");
                display.display();
                break;
            }
            case result:{
                display.clearDisplay();
                display.setTextSize(1);
                display.setTextColor(SSD1306_WHITE);
                display.setCursor(20,10);
                display.println("REACTION TIME: ");
                display.setCursor(40,20);
                display.print(reaction_time);
                display.println("ms");
                display.setCursor(10,30);
                display.print("best time = ");
                display.print(bestTime);
                display.display();

                break;
            }
            case waitForNextRound:
            break;
        }
        previousState = state;
    }

    
     switch (state) {
        case w8fLedOn:{
           if (currentTime - roundStartTime > randomTimeLedStart) {

                ledOnTime = currentTime;
                digitalWrite(led, HIGH);
                state = w8fButtonPress;
            }
            break;
        }
        case w8fButtonPress:{
            byte currentState = digitalRead(button);

            if (lastButtonState != currentState) {
                lastDebounceTime = currentTime;
            }
            if (currentTime - lastDebounceTime > debounceTime) {
                if (currentState != stableButtonState) {
                    stableButtonState = currentState;
                    if (stableButtonState == LOW &&
                        previousStableButtonState == HIGH) {

                        buttonPressedTime = currentTime;
                        reaction_time = buttonPressedTime - ledOnTime;
                        
                        if(bestTime == 0 || reaction_time<bestTime){
                            bestTime = reaction_time;
                        }
                        digitalWrite(led,LOW);

                        state = result;
                    }
                    previousStableButtonState = stableButtonState;
                }
            }
            lastButtonState = currentState;
            break;
        }
        case result:{
            state = waitForNextRound;
            break;
        }
        case waitForNextRound:{
            if (!waiting){
                roundStartTime = currentTime;
                waiting = true;
            }
            if(currentTime - roundStartTime > 15000){
                randomTimeLedStart = random(3000, 8000);
                roundStartTime = currentTime;
                waiting = false;
                state = w8fLedOn;
         
            }
         }
        break;
    }
}