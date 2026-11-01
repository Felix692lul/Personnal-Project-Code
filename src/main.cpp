//TO DO: Create different positions for when the character jump and runs
//TO DO: Create obstacles for the character to jump over
//TO DO: Create a scoring system for the game
//TO DO: Create a game over screen for when the character hits an obstacle
//TO DO: Create a dark mode for the game, when the score is high
//TO DO: ADD extra stuffs??


#include <Arduino.h>          // Core framework for GPIO and interrupts
#include <Wire.h>             // Built-in I2C master driver 
#include <LiquidCrystal.h> // For LCD display

//Pin Definitions
const int rs = 13;
const int e = 14;
const int d4 = 19;
const int d5 = 18;
const int d6 = 17;
const int d7 = 16;
int button = 27;
int LDR = 34;

LiquidCrystal GameDisplay(rs, e, d4, d5, d6, d7); //Create a digital manager for my display and which physical ESP32 wires it must use to send data
                                                  //Gamedisplay refers to the LCD

byte character[8] = { //Creates an index and tells the LCD which pixel to light up in the first column, bottom rectangle
  B01110,
  B01110,
  B00101,
  B11111,
  B10100,
  B01110,
  B01010,
  B01010,
};


//Functions:
enum GameMenu{
 Menu,
 Playing,
 ded


};

GameMenu currentState = Menu; //Set the current state of the game to Menu

void setup() {
  GameDisplay.begin(16, 2); //Initialize the display with 16 columns and 2 rows
  GameDisplay.createChar(0, character);  //save the character design using this function

  pinMode(button, INPUT); //Set the button pin to input mode
  int buttonState = digitalRead(button); //Read the button state and store it in a variable
}

void loop() {
  GameDisplay.setCursor(0, 1); //set the cursor to column 0, row 1
  GameDisplay.write(byte(0)); //print my character to the display

}