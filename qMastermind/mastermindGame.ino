/* LedTable
 *
 * Written by: Klaas De Craemer, Ing. David Hrbaty
 * 
 * Common code and defines for the Tetris game
 */

/*
 * Mastermind -> quantum decryption -> universe/epoxy
 * Tetris -> tunnel -> stone
 * TicTacToe -> quantum operators -> wood
 * Snake -> different measurements -> salt/moss
 * Cat -> different measurements/sensors -> glass
 * Pathways -> Find shortest ways -> borealis
 * 
 * pinball machine 1: quantum key exchange
 * pinball machine 2:
 * 
 * breadboard 1: classic computer
 * breadboard 2: quantum computer
 * 
 * FoL
 * 
 * 7 columns (1 center, obelisk)
 * 
 * 7 baseforms as mirror objects (kugel beschichten!?)
 */

#define  TEXTSPEED 100

//mastermind
// 6 color, 4 holes, 12 rounds, 4 holes for right color/right space (green) and right color, wrong space (blue)
// 1,296 different pattern, solvable in 5 steps or less
// groover: einfach die richtige kombination raten (rainbowcolor effekt??)

// color between 0...5 -> colorLib[6]
// 4 places for color, 4 for guesses -> 9 leds, 3 unused left/right
// 12 rows, 5 rows unused up/down

int numberOfColors=6;
#define numberOfPlaces 6

int maxGuessNumber=FIELD_HEIGHT-1;

int offsetWidth = 0;//(FIELD_WIDTH-(numberOfPlaces*2+1))/2;

boolean gameWon;

int storedGuesses[FIELD_WIDTH][FIELD_HEIGHT];
int storedMaster[numberOfPlaces];
boolean backupMaster[numberOfPlaces];
boolean backupGuess[numberOfPlaces];
int guessNumber;

boolean blinkPosition =true;
int blinkTime = 500;
long blinkUpdate = millis();

int position_Player[2];

void mastermindInit() {
  for (int i=0; i<FIELD_WIDTH; i++) {
    for (int j=0; j<FIELD_HEIGHT; j++) {
      storedGuesses[i][j]=numberOfColors;
    }
  }
  // create color code
  for (int i=0; i<numberOfPlaces; i++) {
    storedMaster[i] = random(0,numberOfColors-1);
  }
  guessNumber=0;
  gameWon =false;
  
  position_Player[0] = 0;
  position_Player[1] = 0;
}

boolean mastermindRunning = false;

void runMastermind(void) {
  mastermindInit();
  mastermindRunning = true;
  while(mastermindRunning) {
    // update player position depending on controls
    playerControlPosition();
    if (storedGuesses[position_Player[0]][guessNumber]==numberOfColors) storedGuesses[position_Player[0]][guessNumber] = random(0,numberOfColors-1);
    // draw Gamefield
    printField();
  }
  // show score
  if (gameWon) {
    scrollTextBlocked("Hurray",6,GREEN);
  } else {
      for(int j=1; j<FIELD_HEIGHT; j++) {
        for(int i=0; i<numberOfPlaces; i++) {
        storedGuesses[i][j] = storedMaster[i];
        }
        printField();
        delay(TEXTSPEED);
        j++;
      }
    mastermindRunning = true;
    while(mastermindRunning) {
      readInput();
      if (curControl != BTN_NONE) mastermindRunning = false;
    }
    //scrollTextBlocked("Try Again",9,RED);
  } 
  // end loop, back to plasma screen
}

void playerControlPosition() {
  readInput();
  switch(curControl){
    case BTN_LEFT:
      //move guess color position left
      if (position_Player[0]>0) {
        position_Player[0]--;
      } else {
        position_Player[0] = numberOfPlaces-1;
      }
      break;
    case BTN_RIGHT:
      //move guess color position right
      if (position_Player[0]<numberOfPlaces-1) {
        position_Player[0]++;
      } else {
        position_Player[0] = 0;
      }
      break;
    case BTN_RB:
      // color --
      if(storedGuesses[position_Player[0]][guessNumber]>0) {
        storedGuesses[position_Player[0]][guessNumber]--;
      } else {
        storedGuesses[position_Player[0]][guessNumber]=numberOfColors-2;
      }
      break;
    case BTN_LB:
      // check if all colors are set
      boolean allGuessesSet = true;
      for(int i=0; i<numberOfPlaces; i++) {
       if (storedGuesses[i][guessNumber] == numberOfColors) allGuessesSet = false;
      }
      if(allGuessesSet) {
        compareGuesses();
        // next round
        guessNumber++;
        guessNumber++;
        // loose:
        if (guessNumber>maxGuessNumber) {
          mastermindRunning = false;
        }
        // winning:
        if (gameWon) {
          mastermindRunning = false;
        }
      }
      break;
  }
}

void compareGuesses() {
  int positionGuess=0;
  Serial.print("Master: ");
  for(int i=0; i<numberOfPlaces; i++) {
    backupMaster[i] = true;
    backupGuess[i] = true;
    Serial.print(storedMaster[i]);
    Serial.print(", ");
  }
  Serial.println();
  Serial.print("Guess: ");
    for(int i=0; i<numberOfPlaces; i++) {
    Serial.print(storedGuesses[i][guessNumber]);
    Serial.print(", ");
  }
  Serial.println();
  // check for a correct guess and position
  for(int i=0; i<numberOfPlaces; i++) {
    if (storedGuesses[i][guessNumber] == storedMaster[i]) {
      // insert color/position guess into storedGuesses
      storedGuesses[numberOfPlaces+2+positionGuess][guessNumber] = 3;
      Serial.print("Correct Guess at ");
      Serial.println(i);
      // set backupMaster false at this position
      backupMaster[i] = false;
      backupGuess[i] = false;
      positionGuess++;
      if(positionGuess==numberOfPlaces) gameWon = true;
    } 
  }
  for(int i=0; i<numberOfPlaces; i++) {
    if(backupGuess[i]) { // if still true not correct guessed
      for(int j=0; j<numberOfPlaces; j++) {
        if (storedGuesses[i][guessNumber] == storedMaster[j] && backupMaster[j]) {
          Serial.print("Wrong Position for ");
          Serial.print(i);
          Serial.print(" but get the color from ");
          Serial.println(j);
          // insert color guess into storedGuesses
          storedGuesses[numberOfPlaces+2+positionGuess][guessNumber] = 2;
          // set backupMaster false at this position
          backupMaster[j] = false;
          backupGuess[i] = false;
          positionGuess++;
          j=numberOfPlaces;
        }
      }   
    }
  }
}

void printField() {
  int x,y;
  // clear field
  for (y=0;y<FIELD_HEIGHT;y++) { 
    for (x=0;x<FIELD_WIDTH;x++) {
      setTablePixel(x,y,CRGB::Black);
    }
  }
  for (y=0;y<FIELD_HEIGHT;y++) { 
    for (x=0;x<FIELD_WIDTH-offsetWidth;x++) {
      if (storedGuesses[x][y]!=numberOfColors) {
        setTablePixel(x+offsetWidth,y,colorLib[storedGuesses[x][y]]);
      }
    }
  }
  // show player
  if (millis() > blinkUpdate+blinkTime) {
    blinkPosition = !blinkPosition;
    blinkUpdate = millis();
  }
  if (blinkPosition)
    setTablePixel(position_Player[0]+offsetWidth,guessNumber,CRGB::Orange);

  showPixels();
}

void scrollTextBlocked(char* text, int textLength, int color){
  unsigned long prevUpdateTime = 0;
  
  for (int x=FIELD_WIDTH; x>-(textLength*8); x--){
    clearTablePixels();
    printText(text, textLength, x, 2, color, FONT_8X8);
    showPixels();
    //Read buttons
    unsigned long curTime;
    do{
      readInput();
      curTime = millis();
    } while (((curTime - prevUpdateTime) < TEXTSPEED) && (curControl == BTN_NONE));//Once enough time  has passed, proceed
    
    prevUpdateTime = curTime;
  }
}
