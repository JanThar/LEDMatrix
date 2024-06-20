/* LedTable
 *
 * Written by: Klaas De Craemer, Ing. David Hrbaty
 * 
 * Common code and defines for the Tetris game
 */

 /* battleship
  *  random position for ships
  *  carrier     5
  *  battleships 4
  *  cruiser     3
  *  submarine   3
  *  destroyer   2
  *  
  *  each ship has a different base -
  *  bomb with correct base will hit 100%, otherwise 50% miss
  *  
  *  alternative: each ship 1*1, different hitnumber, hitprobability
  *  area hit for localization??
  */

#define  TEXTSPEED 100

#define GAMEFIELD_WIDTH 14
#define GAMEFIELD_HEIGHT 21

#define DIR_UP 0
#define DIR_RIGHT 1
#define DIR_DOWN 2
#define DIR_LEFT 3

#define MOVEMENTTIME 1200
long lastUpdateMovement = millis();


int position_Player[2] = {3,3};
int position_Cat[2] = {3,3};
int direction_Cat = 0; 

void catsInit() {
  position_Player[0] = random(0,GAMEFIELD_WIDTH);
  position_Player[1] = random(0,GAMEFIELD_HEIGHT);
  position_Cat[0] = random(0,GAMEFIELD_WIDTH);
  position_Cat[1] = random(0,GAMEFIELD_HEIGHT);
  direction_Cat = random(0,4); 
}

boolean catsRunning = false;

void runCats(void) {
  catsInit();
  catsRunning = true;
  while(catsRunning) {
    // update player position 
    playerInput();
    updateCatPosition();
    printField();
  }
  // show score
  scrollTextBlocked("Got it",6,GREEN);
  // end loop, back to plasma screen
}

void updateCatPosition() {
  if (millis() > MOVEMENTTIME+lastUpdateMovement) {
    lastUpdateMovement = millis();
    //create new direction
    int modDir = random(0,12);
    if (modDir>6) {
      ;//do nothing, keep direction;
    } else if (modDir>3 && modDir<7) {
      direction_Cat = (direction_Cat+1)%4;
    } else if (modDir>0 && modDir<4) {
      direction_Cat = (direction_Cat-1)%4;
    } else {
      direction_Cat = (direction_Cat+2)%4; //0 -> UTurn
    }
    
    switch (direction_Cat) {
      case DIR_UP:
        if(position_Cat[1]<GAMEFIELD_HEIGHT-1) {
          position_Cat[1]=position_Cat[1]+1;
        } else {
          direction_Cat = DIR_DOWN;
          position_Cat[1]-1;
        }
        break;
      case DIR_RIGHT:
        if(position_Cat[0]<GAMEFIELD_WIDTH-1) {
          position_Cat[0]=position_Cat[0]+1;  
        } else {
          direction_Cat = DIR_LEFT;
          position_Cat[0]-1;
        }
        break;
      case DIR_DOWN:
        if(position_Cat[1]>0) {
          position_Cat[1]=position_Cat[1]-1;
        } else {
          direction_Cat = DIR_UP;
          position_Cat[1]+1;
        }
        break;
      case DIR_LEFT:
        if(position_Cat[0]>0) {
          position_Cat[0]=position_Cat[0]-1;  
        } else {
          direction_Cat = DIR_RIGHT;
          position_Cat[0]+1;
        }
        break; 
    }
  }
}

void playerInput() {
  readInput();
  switch(curControl){
    case BTN_LEFT:
      //move player left
      if (position_Player[0] >0) { // otherwise leftmost position already reached
        position_Player[0] = position_Player[0]-1;
      }
      break;
    case BTN_RIGHT:
      //move player right
      if (position_Player[0] <GAMEFIELD_WIDTH-1) { // otherwise rightmost position already reached
        position_Player[0] = position_Player[0]+1;
      }
      break;
    case BTN_DOWN:
      //move player down
      if (position_Player[1] <GAMEFIELD_HEIGHT-1) { // otherwise lowermost position already reached
        position_Player[1] = position_Player[1]+1;
      }
      break;
    case BTN_UP:
      //move player up
      if (position_Player[1] >0) { // otherwise uppermost position already reached
        position_Player[1] = position_Player[1]-1;
      }
      break;
//      case BTN_RB:
//        // Set input
//      break;
      case BTN_LB:
        //checkPosition();
      break;
  }
}

void checkPosition() {
  if (position_Cat[0]==position_Player[0] && position_Cat[1]==position_Player[1]) {
    catsRunning = false;
  }
}

void printField() {
  int x,y;
  // show grid
  for (y=0;y<FIELD_HEIGHT;y++) { 
    for (x=0;x<FIELD_WIDTH;x++) {
      setTablePixel(x,y,CRGB::Black);
    }
  }
  //testing show cat -delete later
  //setTablePixel(position_Cat[0],position_Cat[1],CRGB::White);
  // show player
  // color depending on distance - ToDO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // calculate distance between cat and player
  float distance = sqrt(pow((position_Player[0]-position_Cat[0]),2)+pow((position_Player[1]-position_Cat[1]),2)); //0...27
  int  closeColor = 255-(int)(distance*10);
  int  awayColor = (int)(distance*10);
  Serial.println(distance);
  if (position_Cat[0]==position_Player[0] && position_Cat[1]==position_Player[1]) {
    setTablePixel(position_Player[0],position_Player[1],CRGB::Green);
    catsRunning = false;
  } else {
    //setTablePixel(position_Player[0],position_Player[1],WHITE);
    setTablePixelRGB(position_Player[0],position_Player[1], awayColor, closeColor, 0);
    //FastLED.setBrightness((int)(distance*10));
  }
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
