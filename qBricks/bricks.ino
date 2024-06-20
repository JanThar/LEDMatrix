/* LedTable
 *
 * Written by: Ing. David Hrbaty
 * 
 * Common code and defines for the Bricks game
 */
 
float ballX = 10;
float ballY = 6;
float xincrement = 1;
float yincrement = 1;
int rad = 0;
int scorePlayer = 0;
#define blockWidth 2
#define blockHeight 1
#define numberOfRows 4
int maxAttempt = 1;

int positionPlayer = FIELD_WIDTH/2;

#define numBlocks FIELD_WIDTH/blockWidth*numberOfRows
#define MAX_SCORE numBlocks
#define MAX_ATTEMPT 5
#define PLAYER_HEIGHT 5
boolean continueGame = true;

char bricks[numBlocks][3];

/* Block shape */
static uint8_t brick[] PROGMEM = {
  10,
  8,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

void bricksInit(){
  scorePlayer = 0;
  maxAttempt = 0;
  ballY = 6;
  ballX = 10;
  for (int i=0; i<FIELD_WIDTH/blockWidth; i++) {
    for (int j=0; j<numberOfRows; j++) {
      bricks[i+j*FIELD_WIDTH/blockWidth][0] = 1;
      bricks[i+j*FIELD_WIDTH/blockWidth][1] = i;
      bricks[i+j*FIELD_WIDTH/blockWidth][2] = j;
    }
  }
}

void runBricks(){
  bricksInit();
  unsigned long prevUpdateTime = 0;
  boolean bricksRunning = true;
  while(bricksRunning){    
    
    if (scorePlayer == MAX_SCORE || maxAttempt == MAX_ATTEMPT){
      bricksRunning = false;
      break;
    }
    
    checkBallHitByPaddle();
    checkBlockCollision();
    
    ballX += xincrement;
    ballY += yincrement;
    
    checkBallOutOfBoundsTable();
    clearTablePixels();
    

    // Draw ball
    setTablePixel(ballX,ballY,WHITE);
    
    // Draw player paddle
    for (int x=positionPlayer-PLAYER_HEIGHT/2; x<=positionPlayer+PLAYER_HEIGHT/2; ++x){
      setTablePixel(x, FIELD_HEIGHT-1, BLUE);
    }
    // Draw bricks
    for (int i=0; i<numBlocks; i++){
      if(bricks[i][0] == 1) {
        unsigned int brickColor = colorLib[i%6];
        for (int j=0; j<blockWidth; j++)
          for (int k=0; k<blockHeight; k++)
          setTablePixel(bricks[i][1]*blockWidth+j,bricks[i][2]*blockHeight+k, brickColor);
      }
    }
    showPixels();
    unsigned long curTime;
    boolean dirChanged = false;
    do {
      readInput();
      if (curControl == BUTTON_LB){
        bricksRunning = false;
        break;
      }
      if (curControl != BTN_NONE && !dirChanged){//Can only change direction once per loop
        dirChanged = true;
        setPositionPaddle();
      }
      curTime = millis();
    } 
    while ((curTime - prevUpdateTime) <250);//Once enough time  has passed, proceed. The lower this number, the faster the game is
    prevUpdateTime = curTime;
  }
  
  fadeOut();
  char buf[4];
  int len = sprintf(buf, "%i", scorePlayer);
  //scrollTextBlocked(buf,len,WHITE);
}

void setPositionPaddle(){
  switch(curControl) {
    case BTN_RIGHT:
      if(positionPlayer + (PLAYER_HEIGHT-1) / 2 < FIELD_WIDTH-1){
        ++positionPlayer;
      }
      break;
    case BTN_LEFT:
      if(positionPlayer - PLAYER_HEIGHT / 2 > 0) {
        --positionPlayer;
      }
      break;
  }
}

void checkBallHitByPaddle() {
  if(ballY == FIELD_HEIGHT-2) {
    if(ballX == positionPlayer) {
      yincrement = -1;
      ballY = FIELD_HEIGHT-2;
    } else if(ballX < positionPlayer && ballX >= positionPlayer - PLAYER_HEIGHT / 2) {
      yincrement = -1;
      if (xincrement-1 > -1) xincrement -= 1;
      else xincrement = -1;
      ballY = FIELD_HEIGHT-2;
      ballX = positionPlayer - PLAYER_HEIGHT / 2-1;
    } else if(ballX > positionPlayer && ballX <= positionPlayer + (PLAYER_HEIGHT-1) / 2) {
      yincrement = -1;
      if (xincrement+1 < 1) xincrement += 1;
      else xincrement = 1;
      ballY = FIELD_HEIGHT-2;
      ballX = positionPlayer + (PLAYER_HEIGHT-1) / 2+1;
    }    
  } 
}

void checkBallOutOfBoundsTable() {
  if(ballY < 0) {
    yincrement = - yincrement;
    ballY = 1;
  } else if(ballY > FIELD_HEIGHT-1) {
    yincrement = - yincrement;
    xincrement = 0;
    ballY = FIELD_HEIGHT/2;
    ballX = FIELD_WIDTH/2;
    maxAttempt++;   
  } 
  if(ballX < 0) {
    xincrement = - xincrement;
    ballX = 1;
  } else if(ballX > FIELD_WIDTH-1) {
    xincrement = - xincrement;
    ballX = FIELD_WIDTH-2;
  } 
}

boolean checkBlockCollision(){
    int ballTop = ballY-rad;                                            // Values for easy reference
    int ballBottom = ballY+rad;
    int ballLeft = ballX-rad;
    int ballRight = ballX+rad;
    
    for(int i=0;i<numBlocks;i++){                                       // Loop through the blocks
        if(bricks[i][0] == 1){                                          // If the block hasn't been eliminated
         int blockX = bricks[i][1]*blockWidth;                                     // Grab x and y location
         int blockY = bricks[i][2]*blockHeight;
         if(ballBottom >= blockY && ballTop <= blockY+blockHeight){     // If hitting BLOCK
           if(ballRight >= blockX && ballLeft <= blockX+blockWidth){       
             removeBlock(i);                                            // Mark the block as out of play
             return true;
           }
         }
      }
    }
  return false;                                                         // No collision detected
}
/* Removes a block from game play */
void removeBlock(int index){
      bricks[index][0] = 0;                                             // Mark it as out of play
      scorePlayer++;                                                          // Increment score
      yincrement = -yincrement;                                         // Flip the y increment
}
