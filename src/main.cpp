//TO DO: Create a scoring system for the game
//TO DO: REPLACE ALL THE DIGITALREAD(BUTTON WITH A GLOBAL VARIABLE!?)
//TO DO: Create a dark mode for the game, when the score is high

//Common mistakes:
//Using buttonState as a variable in gameState function when it can only be seen by void setup function
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
bool isJumping = false; //Variable for the character jumping mechanic
unsigned long TimePassedSinceJump = 0; //Variable to track the time since the character last jumped
int lastbuttonstate = HIGH;
int lastdebouncetime = 0;
const unsigned long DEBOUNCE_TIME = 50; 

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

  pinMode(button, INPUT); //Set the button pin to input modes
  
  
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
      while(digitalRead(button) == HIGH){
        // Wait for button release to avoid bouncing issues
      }
     GameDisplay.clear(); //Clear the LCD before starting the minigame
     InitialState = Playing; 
    }
    }
break;

    case Playing:{
    unsigned long currentMillis = millis(); //Store the current time in a variable
    if(currentMillis - previousObstacleMillis >= gameObstacleInterval){ //compares the current time to the last time an obstacle was created
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

    int currentbuttonstate = digitalRead(button);//Read the button and store it
    if(currentbuttonstate != lastbuttonstate && (millis()-lastdebouncetime > DEBOUNCE_TIME)){ 
    lastdebouncetime = millis();
    lastbuttonstate = currentbuttonstate;

    if(currentbuttonstate == HIGH && !isJumping){
      isJumping = true; //Set the jumping mechanic to true
      TimePassedSinceJump = millis();
      }
    }

    if(isJumping && (millis() - TimePassedSinceJump) >= 2000){ //If the character is in the air and 500ms has passed since the jump
      isJumping = false; //Set the jumping mechanic to false

    }

    if(isJumping){
      GameDisplay.setCursor(0, 0); 
      GameDisplay.write(byte(0));

      GameDisplay.setCursor(0, 1); //Places cursor at row 0, column 0
      GameDisplay.print(" "); 
      }else{
      GameDisplay.setCursor(0, 0);
      GameDisplay.print(" ");
      
      GameDisplay.setCursor(0, 1); 
      GameDisplay.write(byte(0)); 
      }

if(!isJumping && groundRow[0] == 1 ){ //character collision logic
GameDisplay.clear(); //Clear the LCD before showing the GameOver
GameDisplay.setCursor(3,0); //Sets the cursor to column 7, row 0
GameDisplay.print("Game Over!");
GameDisplay.setCursor(0,1); //Sets the cursor to column 7, row 1
GameDisplay.print("Press to start");
InitialState = ded;

}
break;
}

    case ded:{
    if(digitalRead(button) == HIGH){
      while(digitalRead(button) == HIGH){ // Wait for button release to avoid bouncing issues
      }
      for(int i = 15; i >= 0; i--){ //Clears the array completely
        groundRow[i] = 0;
        }
        previousObstacleMillis = millis(); //refresh the old obstacle timer 
        isJumping = false; //Sets the jump to false


      GameDisplay.clear();
      InitialState = Playing;
      

    
}
  break;
  }
  }
  }

