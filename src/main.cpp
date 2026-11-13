//Common mistakes:
//Using buttonState as a variable in gameState function when it can only be seen by void setup function
//Not adding curly brackets after declaring calling an enum using case in the switch state
//Not telling where to print the text to the LCD in case Menu
//lastdebouncetime never reset to the current time



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
int readButton = LOW;
unsigned long previousObstacleMillis = 0; //Store the last time an obstacle was created
bool isJumping = false; //Variable for the character jumping mechanic
unsigned long TimePassedSinceJump = 0; //Variable to track the time since the character last jumped
int lastbuttonstate = HIGH;
int lastdebouncetime = 0;
const unsigned long DEBOUNCE_TIME = 50; 
unsigned long lastscoreupdate = 0;
unsigned long currentscore = 0;
const long scoreupdateinterval = 500;
unsigned long Charlastposition = 0;
const long Charpositioninterval = 400;
byte currentSprite = 0;
const long gameObstacleInterval = 700; //Set at which interval the obstacles will appear and the speed of the game
const int GHOST_THRESHOLD = 1500;

LiquidCrystal GameDisplay(rs, e, d4, d5, d6, d7); //Create a digital manager for my display and which physical ESP32 wires it must use to send data
                                                    //Gamedisplay refers to the LCD
                                                                                                                                      
byte character[8] = { //Creates an index and tells the LCD which pixel to light up in the first column, bottom rectangle
  B01110,             //Character position 1
  B01110,
  B00101,
  B11111,
  B10100,
  B00100,
  B01010,
  B01010,
};

byte character2[8] = { //Character position 2
  B01110,
  B01110,
  B10100,
  B11111,
  B00101,
  B00100,
  B11011,
  B00000,
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
  GameDisplay.createChar(0, character);  //save characters design using this function
  GameDisplay.createChar(1, character2);
  pinMode(button, INPUT); //Set the button pin to input modes
   
}

void loop() {
  GameState(); //Handle the GameState function continiously
  readButton = digitalRead(button);
}

void GameState(){
  switch (InitialState) { // Switch on the variable InitialState
    
    case Menu:{
      GameDisplay.setCursor(0,0); //Places cursor at row 0, column 0
      GameDisplay.print("Press button to "); //Prints "Press Button to Start" on the LCD
      GameDisplay.setCursor(0,1); //Places cursor at row 1, column 0
      GameDisplay.print("start :D");

      if(readButton == HIGH){
        GameDisplay.clear(); //Clear the LCD before starting the minigame
        currentscore = 0; 
        InitialState = Playing;
      }
    }
    break;

    case Playing:{

      //OBSTACLES LOGIC
      unsigned long currentMillis = millis(); //Store the current time in a variable
      if(currentMillis - previousObstacleMillis >= gameObstacleInterval){ //compares the current time to the last time an obstacle was created
        previousObstacleMillis = currentMillis; //update the last time an obstacle was created to the current time
      
        for(int i = 0; i < 15; i++){
          groundRow[i] = groundRow[i + 1]; //Shift the obstacles to the left by one column
                                           //If Row 15 is left alone, it duplicates whatever nb next to it
        }
        if (random(0, 100) < 30 && groundRow[14] == 0) { //check any nb from 0 to 29, if any number between these 2 or any of those 2 are picked, spawn at obstacle(30% basically)
          groundRow[15] = 1;                             //Prevents two ** from appearing
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


      //Character position switch logic
      if(!isJumping && millis()-Charlastposition >= Charpositioninterval){
        Charlastposition = millis();
        currentSprite = 1 - currentSprite;
      }
        

      //CHARACTER JUMPING LOGIC
      if(isJumping && (millis() - TimePassedSinceJump) >= 1300){ //If the character is in the air and 1.3 sec has passed since the jump
        isJumping = false; //Set the jumping mechanic to false
      }

      if(isJumping){
        GameDisplay.setCursor(0, 0); 
        GameDisplay.write(currentSprite);

        GameDisplay.setCursor(0, 1); //Places cursor at row 0, column 0
        GameDisplay.print(" "); 
      }else{
        GameDisplay.setCursor(0, 0);
        GameDisplay.print(" ");
        
        GameDisplay.setCursor(0, 1); 
        GameDisplay.write(currentSprite); 
      }

      //BUTTON DEBOUNCING LOGIC
      int currentbuttonstate = digitalRead(button);//Read the button and store it
      if(currentbuttonstate != lastbuttonstate && (millis()-lastdebouncetime > DEBOUNCE_TIME)){ 
        lastdebouncetime = millis();
        lastbuttonstate = currentbuttonstate;

        if(currentbuttonstate == HIGH && !isJumping){
          isJumping = true; //Set the jumping mechanic to true
          TimePassedSinceJump = millis();
        }
      }      

      //LDR READING
      int lightlevel = analogRead(LDR);
      bool isghost = (lightlevel < GHOST_THRESHOLD);

  
      //GHOST MODE INDICATOR+SCORE SYSTEM LOGIC
      unsigned long currentTime = millis();
      if(isghost){
        GameDisplay.setCursor(8,0);
        GameDisplay.print("G");
        GameDisplay.setCursor(10,0);
        GameDisplay.print("      ");
        currentscore = 0;
        lastscoreupdate = currentTime;
        }else{
        GameDisplay.setCursor(8,0);
        GameDisplay.print(" ");
        if(currentTime - lastscoreupdate >= scoreupdateinterval){
         currentscore++; //Increments the score!!
         lastscoreupdate = currentTime; //Update the last score time

         GameDisplay.setCursor(10,0); //Set cursor column 10, row 0
         GameDisplay.print("Sc:");
         GameDisplay.print(currentscore);
      }
      }

        
      //CHARACTER COLLISION LOGIC+GAME OVER SCREEN
      if(!isJumping && groundRow[0] == 1 && !isghost){ 
        GameDisplay.clear(); //Clear the LCD before showing the GameOver
        GameDisplay.setCursor(3,0); //Sets the cursor to column 3, row 0
        GameDisplay.print("Game Over!");
        GameDisplay.setCursor(0,1); //Sets the cursor to column 0, row 1
        GameDisplay.print("Press to start");
        InitialState = ded;
      }else if (isghost && groundRow[0] == 1){
        groundRow[0] = 0;
      }
     }
     break;
    case ded:{
      if(readButton == HIGH){
        while(digitalRead(button) == HIGH){ // Wait for button release to avoid bouncing issues
        }
        for(int i = 15; i >= 0; i--){ //Clears the array completely
          groundRow[i] = 0;
        }
        previousObstacleMillis = millis(); //refresh the old obstacle timer 
        isJumping = false; //Sets the jump to false so character doesn't stay in the air


        currentscore = 0;
        GameDisplay.clear();
        InitialState = Playing;
      }

      break;
    }
  }
}
