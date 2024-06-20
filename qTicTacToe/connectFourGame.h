// Quantum TicTacToe https://medium.com/@toohonlin/develop-quantum-mechanics-intuition-through-quantum-game-qubit-tic-tac-toe-d9814bc927dc
// Either two unitary moves or one measurement 
// |0> , |1> or undefined
// q put a pure state qbit (undefined)
// h Hadamerd gate -> superposition, undefined again
// x pauli gate -> change from |0> to |1> or the other way around, undefined stays undefined
// m measurement -> ste value fixed
// 3*3 field for qbits (either off, up, down, horizontal -> alternative 3 colors)
// 1 row in between off -> 3|3|3|3|3 (19) * 3|3|3|3|3|3 (23) 5*6

#define AREA_WIDTH  3
#define AREA_HEIGHT 3
int gameStates[AREA_WIDTH][AREA_HEIGHT];
int gameStates1[AREA_WIDTH][AREA_HEIGHT];

int computerThoughtTime = 200;

#define spinUndefined 1
#define fixedSpinUp   4
#define fixedSpinDown 5

#define  TEXTSPEED 100

#define xQ       0
#define hQ       1
#define initQ    2
#define measureQ 3

int white = 0xFFFFFF; //CRGB::White;
int green = 0x005000; //CRGB::Green;
int black = 0x000000;
int blue  = 0x0000FF; // player one
int red   = 0xFF00FF;  // player two

int cursorX = AREA_WIDTH/2;
int cursorY = AREA_HEIGHT/2;
int cursorColor;

int score1,score2;

int operationCounter = 0;
int maxOperations = 20;

boolean firstPlayer, computerPlayerFirst;

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

void initConnectFour() {
  for (int i=0; i<AREA_WIDTH; i++) {
    for (int j=0; j<AREA_HEIGHT; j++) {
      gameStates[i][j] = spinUndefined;
    }
  }
  score1 = 0;
  score2 = 0;
  firstPlayer      = true;
  computerPlayerFirst = random(0,2);
  operationCounter = 0;
}

void clearConnectFour() {
  for (int i=0; i<FIELD_WIDTH; i++) {
    for (int j=0; j<FIELD_HEIGHT; j++) {
      setTablePixel(i,j,black);
    }
  }
}

int offsetX = 2;
int offsetY = 5;

void drawConnectFour() {
  clearConnectFour();
  // draw basic field
  for (int i=0; i<AREA_WIDTH; i++) {
    for (int j=0; j<AREA_HEIGHT; j++) {
      if (gameStates[i][j] == spinUndefined) {
        setTablePixel(i*4+0+offsetX,j*4+2+offsetY, 0xFF00FF); //red
        setTablePixel(i*4+1+offsetX,j*4+2+offsetY, 0xFF00FF);
        setTablePixel(i*4+2+offsetX,j*4+2+offsetY, 0xFF00FF);
      } else if (gameStates[i][j] == fixedSpinUp) { //blue
        setTablePixel(i*4+1+offsetX,j*4+1+offsetY, 0x0000FF);
        setTablePixel(i*4+1+offsetX,j*4+2+offsetY, 0x000050);
        setTablePixel(i*4+1+offsetX,j*4+3+offsetY, 0x000010);
      } else if (gameStates[i][j] == fixedSpinDown) { //green
        setTablePixel(i*4+1+offsetX,j*4+1+offsetY, 0x001000);
        setTablePixel(i*4+1+offsetX,j*4+2+offsetY, 0x005000);
        setTablePixel(i*4+1+offsetX,j*4+3+offsetY, 0x00FF00); 
        //red 0xFF00FF
      }
    }
  }
  // draw cursor positon
  //Serial.println(firstPlayer);
  if (firstPlayer) {
    cursorColor = green;
    setTablePixel(cursorX*4+offsetX,   cursorY*4+2+1+offsetY, cursorColor);
    setTablePixel(cursorX*4+2+offsetX, cursorY*4+2+1+offsetY, cursorColor);
  } else {
    cursorColor = blue;
    setTablePixel(cursorX*4+offsetX,   cursorY*4+1+offsetY,   cursorColor);
    setTablePixel(cursorX*4+2+offsetX, cursorY*4+1+offsetY,   cursorColor);
  }
//  Serial.print(cursorX*4);
//  Serial.print(", ");
//  Serial.println(cursorY*4);
  showPixels();
}

void checkFours() {
  // update matrix
  for (int i=0; i<AREA_WIDTH; i++) {
    for (int j=0; j<AREA_HEIGHT; j++) {
      if(gameStates[i][j] == spinUndefined) {
        int randomSpin = random(0,2);
        if (randomSpin == 0) {
          gameStates[i][j] = fixedSpinDown;
        } else {
          gameStates[i][j] = fixedSpinUp;
        }
      } 
    }
  }
  // score2 red
  //check columns
  for (int i=0; i<AREA_WIDTH; i++) {
    if ((gameStates[i][0] == gameStates[i][1]) && (gameStates[i][0] == gameStates[i][2])) {
      if (gameStates[i][0]==fixedSpinUp) score1++;
      else score2++;
    }
  }
  //check rows
  for (int i=0; i<AREA_HEIGHT; i++) {
    if ((gameStates[0][i] == gameStates[1][i]) && (gameStates[0][i] == gameStates[2][i])) {
      if (gameStates[0][i]==fixedSpinUp) score1++;
      else score2++;
    }
  }
  //check diagonals
  if ((gameStates[0][0] == gameStates[1][1]) && (gameStates[0][0] == gameStates[2][2])) {
    if (gameStates[0][0]==fixedSpinUp) score1++;
    else score2++;
  }
  if ((gameStates[0][2] == gameStates[1][1]) && (gameStates[0][2] == gameStates[2][0])) {
    if (gameStates[0][2]==fixedSpinUp) score1++;
    else score2++;
  }
}

void changePlayer() {
  firstPlayer = !firstPlayer;
}

void updatePosition(int function) {
  if (function == hQ) {
    Serial.println("hQ");
    if (gameStates[cursorX][cursorY] == fixedSpinUp || gameStates[cursorX][cursorY] == fixedSpinDown) {
      gameStates[cursorX][cursorY] = spinUndefined;
      operationCounter++;
      changePlayer();
    }
  } else if (function == xQ) {
    Serial.println("initQ");
    if (gameStates[cursorX][cursorY] == spinUndefined) {
      if (firstPlayer) gameStates[cursorX][cursorY] = fixedSpinDown;
      else gameStates[cursorX][cursorY] = fixedSpinUp;
      operationCounter++;
      Serial.print("Init SpinUndefined, ");
      changePlayer();
      Serial.println("change player");
    }
  }
}

void computerControl() {
  delay(computerThoughtTime);
  boolean findPlace = false;
  while (!findPlace) {
    int i = random(0,3);
    int j = random(0,3);
    if (gameStates[i][j]==spinUndefined) {
      if (computerPlayerFirst) gameStates[i][j] = fixedSpinDown;
      else gameStates[i][j] = fixedSpinUp;
      findPlace = true;
      operationCounter++;
      changePlayer();
    } else if (gameStates[i][j]==fixedSpinUp)  {
      if (computerPlayerFirst) {
        gameStates[i][j] = spinUndefined;
        findPlace = true;
        operationCounter++;
        changePlayer();
      }
      //already got this part, try again
    } else if (gameStates[i][j]==fixedSpinDown)  {
      if (!computerPlayerFirst) {
        gameStates[i][j] = spinUndefined;
        findPlace = true;
        operationCounter++;
        changePlayer();
      }
      //already got this part, try again
    }
  } 
  operationCounter++;
}

void playerControl() {
  switch(curControl){
    case BTN_RIGHT:
      cursorX++;
      if (cursorX>AREA_WIDTH-1) cursorX=0;
      break;
    case BTN_LEFT:
      cursorX--;
      if (cursorX<0) cursorX=AREA_WIDTH-1;
      break;
    case BTN_UP:
      cursorY--;
      if (cursorY<0) cursorY=AREA_HEIGHT-1;
      break;
    case BTN_DOWN:
      cursorY++;
      if (cursorY>AREA_HEIGHT-1) cursorY=0;
      break;
    case BTN_LB:
      updatePosition(xQ);
      break;
    case BTN_RB:
      updatePosition(hQ);
      break;
  }
}

void runConnectFour(void) {
  Serial.println("InitGame");
  initConnectFour();
  boolean gameRunning = true;
  while(gameRunning) {
    //Serial.println("gameRunning");
    if (firstPlayer!=computerPlayerFirst) {
      readInput();
      playerControl();
    } else {
      computerControl();
    }
    drawConnectFour();
    //gameRunning = checkFreeSpaces();
    if(operationCounter > maxOperations) gameRunning = false;
  }
  checkFours();

  drawConnectFour();
  delay(1000);
  curControl = BTN_NONE;
  while (curControl == BTN_NONE) {
    readInput();
    drawConnectFour();
  }

  if (score2 > score1) {
    scrollTextBlocked("Congrats",8,GREEN);
  } else if (score1 == score2) {
    scrollTextBlocked("Draw",4,YELLOW);
  } else {
    scrollTextBlocked("Try Again",9,RED);
  }
  
//  char buf[4];
//  int len = sprintf(buf, "%i", score2);
//  scrollTextBlocked(buf, len, green); 
//
//  scrollTextBlocked(":", 1, red); 
//
//  len = sprintf(buf, "%i", score1);
//  scrollTextBlocked(buf, len, blue);
}
