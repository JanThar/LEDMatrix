/* LedTable
 *
 * Written by: Klaas De Craemer, Ing. David Hrbaty
 * 
 * Common code and defines for the Tetris game
 */

/* pacman: blau wall, gelb cookies, powerup, pacman
 * monster 4 farben, blau fressbar
 */

#define  TEXTSPEED 100
int moveSpeed = 100;

boolean pathways[21][14] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,1,0,0,1,0,0,0,0,1},
  {1,0,1,1,1,1,0,0,1,1,1,1,0,1},
  {1,0,1,0,0,0,0,0,0,0,0,1,0,1},
  {1,0,1,1,1,1,0,0,1,1,1,1,0,1},
  {1,0,0,0,0,1,0,0,1,0,0,0,0,1},
  {1,1,1,1,1,1,0,0,1,1,1,1,1,1},
  {1,0,1,0,0,1,1,1,1,0,0,1,0,1},
  {1,0,1,0,0,1,0,0,1,0,0,1,0,1},
  {1,0,1,0,1,1,1,1,1,1,0,1,0,1},
  {1,0,1,0,1,0,0,0,0,1,0,1,0,1},
  {1,0,1,0,1,1,1,1,1,1,0,1,0,1},
  {1,0,1,0,0,1,0,0,1,0,0,1,0,1},
  {1,0,1,0,0,1,1,1,1,0,0,1,0,1},
  {1,1,1,1,1,1,0,0,1,1,1,1,1,1},
  {1,0,0,0,0,1,0,0,1,0,0,0,0,1},
  {1,0,1,1,1,1,0,0,1,1,1,1,0,1},
  {1,0,1,0,0,0,0,0,0,0,0,1,0,1},
  {1,0,1,1,1,1,0,0,1,1,1,1,0,1},
  {1,0,0,0,0,1,0,0,1,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int position_Player[2] = {1,6};

#define numberOfTargets 15

int position_Targets[numberOfTargets][2] = {
  {3,0},  {5,2},   {5,5},   {13,5},  {0,8},
  {2,12}, {11,10}, {13,14}, {6,9},   {11,12},
  {5,14}, {0,18},  {2,17},  {8,19}, {11,20}
};

boolean targetActive[numberOfTargets];

int targetsActive;
int score;

int bestScore = 9999;

#define GAMEFIELDOFFSET (FIELD_WIDTH-GAMEFIELDWIDTH)/2

void pathwaysInit() {
  //FastLED.setBrightness(150);
  for(int i=0; i<numberOfTargets; i++) {
    targetActive[i] = true;
  }
  position_Player[0] = 1;
  position_Player[1] = 6;
  targetsActive = 15;
  score=0;
}

boolean pathwaysRunning = false;

void checkTargets() {
  for(int i=0; i<numberOfTargets; i++) {
    if (targetActive[i]) {
      if ((position_Player[0] == position_Targets[i][0]) && (position_Player[1] == position_Targets[i][1])) {
        targetActive[i] = false;
        targetsActive--;
      }
    }
  }
}

void runPathways(void) {
  pathwaysInit();
  unsigned long prevUpdateTime = 0;
  pathwaysRunning = true;
  while(pathwaysRunning) {
    unsigned long curTime;
    //do{
      delay(moveSpeed);
      // update player position depending on controls
      playerControlPosition();
      // check collision with targets
      checkTargets();
      // draw Gamefield
      printField();
      // if all targets are collected end game
      if (targetsActive==0) {
        pathwaysRunning = false;
        break;
      }
    //  curTime = millis();
    //} while ((curTime - prevUpdateTime) < moveSpeed);
  }
  // show score
  char str[3];
  sprintf(str, "%d", score);
  if (score<bestScore) {
    bestScore=score;
    scrollTextBlocked(str,3,GREEN);
  } else {
    scrollTextBlocked(str,3,RED);
  }
  // end loop, back to plasma screen
}

void playerControlPosition() {
  // if possible:
  //Serial.println(curControl);
  readInput();
  switch(curControl){
    case BTN_LEFT:
      //move player left
      if (position_Player[0] >0) { // otherwise leftmost position allready reached
        if(pathways[position_Player[1]][position_Player[0]-1]) {
          position_Player[0] = position_Player[0]-1;
          score++;
        }
      }
      break;
    case BTN_RIGHT:
      //move player right
      if (position_Player[0] <14) { // otherwise rightmost position allready reached
        if(pathways[position_Player[1]][position_Player[0]+1]) {
          position_Player[0] = position_Player[0]+1;
          score++;
        }
      }
      break;
    case BTN_DOWN:
      //move player down
      if (position_Player[1] <21) { // otherwise lowermost position allready reached
        if(pathways[position_Player[1]+1][position_Player[0]]) {
          position_Player[1] = position_Player[1]+1;
          score++;
        }
      }
      break;
    case BTN_UP:
      //move player up
      if (position_Player[1] >0) { // otherwise uppermost position allready reached
        Serial.println("Up");
        if(pathways[position_Player[1]-1][position_Player[0]]) {
          position_Player[1] = position_Player[1]-1;
          score++;
        }
      }
      break;
  }
}

void printField() {
  int x,y;
  // show grid
  for (y=0;y<FIELD_HEIGHT;y++) { 
    for (x=0;x<FIELD_WIDTH;x++) {
      if(pathways[y][x]) {
        setTablePixel(x,y,CRGB::Blue);
      } else {
        setTablePixel(x,y,CRGB::Black);
      }
    }
  }
  // show targets
  for(int i=0; i<numberOfTargets; i++) {
    if (targetActive[i]) {
      setTablePixel(position_Targets[i][0],position_Targets[i][1],CRGB::Red);
    }
  }
  // show player
  setTablePixel(position_Player[0],position_Player[1],CRGB::Green);

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
