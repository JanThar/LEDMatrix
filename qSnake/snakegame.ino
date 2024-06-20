// see https://kirais.itch.io/keep-drogon-alive
// "When Drogon is red, its state is |0>; when it's blue, its state is |+> = |0>+|1>. 
//  When the arrow is read, it's a Z-basis measurement; when it's blue, it's a X-basis measurement. 
//  Whether Drogon gets hit is determined by the measurement outcome of |1>. 
//  When Drogon and arrow have the same color, the measurement outcome is 100% |0>, 0% |1> and therefore, it never gets hit. 
//  When they are not the same color, the measurement outcome is 50% |0> and 50% |1> and therefore, Drogon gets hit 50% of the time.

#define GAMEDELAY 500
long timestamp;
#define SPAWNDELAY 5000
long spawntime;
#define  TEXTSPEED 100

int qCollectorStartingValues = 4;
int qCollectorLength;//Current length of collector
int qCollectorDir;//Current Direction of collector
int qCollectorVal;//Current Direction of collector
int qScore;
boolean snakeRunning = false;

int newPositionCollectorX = 0;
int newPositionCollectorY = 0;

struct matrixPosition {
  int x;
  int y;
  int v;
};
typedef struct matrixPosition MatrixPosition;

MatrixPosition qCollector[NUM_PIXELS]; // positions of collector parts

void runSnake() {
  initSnake();
  snakeRunning = true;
  while (snakeRunning) {
    readInput();
    runGame();
    FastLED.show();
    delay(50);
  }
  // show score
  //char str[3];
  //sprintf(str, "%d", qCollectorLength);
  if (qCollectorLength>5) {
    scrollTextBlocked("Enough",6,GREEN);
  } else {
    scrollTextBlocked("GetMore",7,RED);
  }
}

void runGame() {
  if (curControl == BTN_UP)    qCollectorDir = DIR_RIGHT;
  if (curControl == BTN_DOWN)  qCollectorDir = DIR_LEFT;
  if (curControl == BTN_LEFT)  qCollectorDir = DIR_UP;
  if (curControl == BTN_RIGHT) qCollectorDir = DIR_DOWN;
  if (curControl == BTN_RB)     qCollectorVal = (qCollectorVal+1)%2;
  if (curControl == BTN_LB)     qCollectorVal = (qCollectorVal+1)%2;
  //Serial.println(qCollectorVal);

  newPositionCollectorX = qCollector[0].x;
  newPositionCollectorY = qCollector[0].y;


  if (millis() > timestamp+GAMEDELAY) {
    timestamp=millis();
    if (qCollectorDir == DIR_UP)    newPositionCollectorX--;
    if (qCollectorDir == DIR_DOWN)  newPositionCollectorX++;
    if (qCollectorDir == DIR_RIGHT) newPositionCollectorY--;
    if (qCollectorDir == DIR_LEFT)  newPositionCollectorY++;

    
    if (newPositionCollectorX < 0)              newPositionCollectorX = FIELD_WIDTH-1;
    if (newPositionCollectorX > FIELD_WIDTH-1) newPositionCollectorX = 0;
    if (newPositionCollectorY < 0)              newPositionCollectorY = FIELD_HEIGHT-1;
    if (newPositionCollectorY > FIELD_HEIGHT-1)  newPositionCollectorY = 0;
  
    //check for collision
    if (leds[newPositionCollectorX][newPositionCollectorY] == BLACK) { // everything fine, continue
      updateCollectorPosition();
    } else if (checkSelfCollision()) {
      gameover();
    } else if (leds[newPositionCollectorX][newPositionCollectorY] == colorLib[qCollectorVal+3]) { // same color, measuremant and resource aligned, 100%
      collectResource();
      createResource();
    } else if (leds[newPositionCollectorX][newPositionCollectorY] != colorLib[qCollectorVal+3]) { // not aligned, 50% chance
      if(random(0,2)) {
        gameover();
      }
      else {
        collectResource();
        createResource();
      }
    }
  }
  if(millis() > spawntime+SPAWNDELAY) {
    createResource();
    spawntime = millis();
  }
}

boolean checkSelfCollision() {
  if(qCollectorLength>1) {
    for(int i=0; i<qCollectorLength; i++) {
      if(qCollector[i].x == newPositionCollectorX && qCollector[i].y == newPositionCollectorY) return true;
    }
  }
  return false;
}

void updateCollectorPosition() {
  for(int i=0; i<qCollectorLength; i++) { //all off
    setTablePixel(qCollector[i].x, qCollector[i].y, BLACK);
  }
  
  for(int i=qCollectorLength-1; i>0; i--) {
    qCollector[i].x = qCollector[i-1].x;
    qCollector[i].y = qCollector[i-1].y;
  }
  //new first position
  qCollector[0].x = newPositionCollectorX;
  qCollector[0].y = newPositionCollectorY;
  
  for(int i=0; i<qCollectorLength; i++) { //all on
    setTablePixel(qCollector[i].x, qCollector[i].y, colorLib[qCollectorVal+3]);
  }
}

void collectResource() {
  qScore++;
  //Serial.println(qScore);
    
  for(int i=qCollectorLength; i>0; i--) {
    qCollector[i].x = qCollector[i-1].x;
    qCollector[i].y = qCollector[i-1].y;
  }
  //new first position
  qCollector[0].x = newPositionCollectorX;
  qCollector[0].y = newPositionCollectorY;
  qCollectorLength++;
}

void gameover() {
 snakeRunning =false;
}

void initSnake() {
  Serial.println("RunSnake");
  timestamp=millis();
  spawntime=millis();
  //qCollectorStartingValues = 4;
  qScore=0;
  qCollectorLength = 1;
  qCollectorDir = random(0,5);
  qScore = 0;
  for(int i=0; i<FIELD_WIDTH; i++) {
    for(int j=0; j<FIELD_HEIGHT; j++) {
      setTablePixel(i,j,BLACK);
    }
  }
  qCollector[0].x = random(0,FIELD_WIDTH);
  qCollector[0].y = random(0,FIELD_HEIGHT);
  qCollectorVal = 0;
  setTablePixel(qCollector[0].x, qCollector[0].y, colorLib[qCollectorVal+3]);
  for (int i=0; i<4; i++) {
    createResource();
  }
  Serial.println("StartSnakeComplete");
}

void createResource() {
  int randomX = random(0,FIELD_WIDTH);
  int randomY = random(0,FIELD_HEIGHT);
  boolean positionFound = false;
  while (!positionFound) {
    if(leds[randomX][randomY] == BLACK) {
      setTablePixel(randomX, randomY,colorLib[random(3,5)]);
//      Serial.print("Recource ");
//      Serial.print(matrixpixel[randomX][randomY]);
//      Serial.print(" at x: ");
//      Serial.print(randomX);
//      Serial.print(" ,y: ");
//      Serial.print(randomY);
      positionFound = true;
    } else {
      randomX = random(0,FIELD_WIDTH);
      randomY = random(0,FIELD_HEIGHT);
    }
  }
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
