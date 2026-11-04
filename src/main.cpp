//TO DO: Create different positions for when the character jump and runs
//TO DO: Create a scoring system for the game
//TO DO: Create a game over screen for when the character hits an obstacle
//TO DO: Create a dark mode for the game, when the score is high
//TO DO: ADD extra stuffs??

//Common mistakes:
//Using buttonState as a variable in gameState function when it can only be seen by void setup function
//Using .print for the LCD because println may spawn corrupted blocks, tries to proceed new lines
//Not adding curly brackets after declaring calling an enum using case in the switch state
//Not telling where to print the text to the LCD in case Menu

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

//Global Variables:
unsigned long previousObstacleMillis = 0; //Store the last time an obstacle was created

const long gameSpeed = 150; //Set the speed of the game
const long gameObstacleInterval = 1000; //Set at which interval the obstacles will appear

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

byte groundRow[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //From left to right, 0 is nothing and 1 is obstacles

//Global constants and Enums
enum GameMenu{
 Menu,
 Playing,
 ded


};

GameMenu InitialState = Menu; //Set the current state of the game to Menu


void setup() {
  GameDisplay.begin(16, 2); //Initialize the display with 16 columns and 2 rows
  GameDisplay.createChar(0, character);  //save the character design using this function

  pinMode(button, INPUT); //Set the button pin to input mode
  
}

void loop() {
  GameState(); //Handle the GameState function continiously
}

void GameState(){
  switch (InitialState) { // Switch on the variable InitialState
    
    case Menu:{
    GameDisplay.setCursor(0,0); //Places cursor at row 0, column 0
    GameDisplay.print("Press button to "); //Prints "Press Button to Start" on the LCD
    GameDisplay.setCursor(0,1); //Places cursor at row 1, column 0
    GameDisplay.print("start :D");

    if(digitalRead(button) == HIGH){
     GameDisplay.clear(); //Clear the LCD before starting the minigame
     InitialState = Playing; 
    }
    break;
    }


    case Playing:{
    GameDisplay.setCursor(0, 1); //set the cursor to column 0, row 1
    GameDisplay.write(byte(0)); //print my character to the display

    unsigned long currentMillis = millis(); //Store the current time in a variableime to the l
    if(currentMillis - previousObstacleMillis >= gameObstacleInterval){ //compares the current tast time an obstacle was created
     previousObstacleMillis = currentMillis; //update the last time an obstacle was created to the current time
   
  
    for(int i = 0; i < 15; i++){
     groundRow[i] = groundRow[i + 1]; //Shift the obstacles to the left by one column
                                      //If Row 15 is left alone, it duplicates whatever nb next to it
   
    }
    if (random(0, 100) < 20) { //check any nb from 0 to 19, if any number between these 2 or any of those 2 are picked, spawn at obstacle(20% basically)
     groundRow[15] = 1; 
    } else {
     groundRow[15] = 0;
    }
    }

   for (int i = 0; i < 16; i++) {
    GameDisplay.setCursor(i, 1); // Row 1 is the bottom row
    if (groundRow[i] == 1) {
     GameDisplay.print("*"); // Draw obstacle on LCD
    } else if (i > 0) { // Will not overwrite my character
       GameDisplay.print(" "); // Clear old trail
    }
    }

    // TO DO: If character hits an obstacle, change InitialState = ded;
    break;
}

    case ded:{
    GameDisplay.print("Game Over! Score: "); // TO DO: Print "Game Over! Score: X"
    // Read button press: if pressed, clear the array and change InitialState = Playing;
    break;
  }
  }   
  }

