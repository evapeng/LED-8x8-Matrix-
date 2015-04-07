//setup pins
int pins[16]={12,11,10,9,8,7,6,5,4,3,2,A0,A1,A2,A3,A4};
int cols[8]={pins[0],pins[1],pins[2],pins[3],pins[4],pins[5],pins[6],pins[7]};
int rows[8]={pins[8],pins[9],pins[10],pins[11],pins[12],pins[13],pins[14],pins[15]};
//setup initialize letter displays ----------------------------------
//numbers of letters we have so far currently 
const int numLetters=14; 
int letterHexArrays[14][8]={{0x00,0x40,0x40,0x40,0x40,0x40,0x7e,0x00},{0x7e,0x40,0x40,0x78,0x78,0x40,0x40,0x7e},{0xff,0x18,0x18,0x18,0x18,0x18,0x18,0x18},{0x7e,0x40,0x40,0x7e,0x7e,0x2,0x2,0x7e},{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},{0x7e,0x62,0x62,0x7e,0x60,0x60,0x60,0x60},{0x00,0x40,0x40,0x40,0x40,0x40,0x7e,0x00},{0x7e,0x42,0x42,0x7e,0x42,0x42,0x42,0x42},{0x42,0x42,0x42,0x7e,0x18,0x18,0x18,0x18},{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},{0x7e,0x62,0x62,0x7e,0x60,0x60,0x60,0x60},{0x7e,0xE7,0xC3,0xC3,0xC3,0xC3,0xE7,0x7e},{0xc1,0xA1,0x91,0x89,0x85,0x83,0x81,0x81},{0xff,0x80,0x80,0x9f,0x81,0x81,0x81,0xff}};
byte leds[numLetters][8][8];
byte returnArray[numLetters][8][8];
byte row = 0;
int delayTime=1000;
int displayLet=-1;
//game controls---------------------------------------
int startGame=0;
const int control1 = A5;
const int control2 = A6;
int totalPoints=10; //limit number of points 
int helpThreshold = 2; //difference in points before help is initialized 
//player controls-------------------------------------
int player1Pos = 0; 
int player2Pos = 0;
int increment = 10; //threshold for analogRead difference 
//initialize players leds
byte player1Leds[8][8]; 
byte player2Leds[8][8];
//initialize player columns
int defaultCol1 = 0;
int defaultCol2 = 7;
//initialize player rows
int player1TopRow=3;
int player2TopRow=3;
int topRow=-1;
//initialize paddle information 
int paddleSize1=2;
int paddleSize2=2;
int paddleSize=-1;
int count=1;
//player points
int player1Points=0;
int player2Points=0;
//initialize ball controls-----------------------------------
int ballRow=4;
int ballCol=4;
byte ballLeds[8][8];
/*[0 1 2
   3 x 4
   5 6 7]
*/
int dirCoord[4][2]={{-1,-1}       ,{-1,1},
                        
                    {1,-1}        ,{1,1}};

//choose a random direction 
int dir = initializeBall();
int ballCoord[2];
int gameOverBool=0;

//BELOW IS REFERENCE CODE (NOT MY OWN) FOR FORMATTING 
#ifndef ARDPRINTF
#define ARDPRINTF
#define ARDBUFFER 16
#include <stdarg.h>
#include <Arduino.h>

int ardprintf(char *str, ...)
{
  int i, count=0, j=0, flag=0;
  char temp[ARDBUFFER+1];
  for(i=0; str[i]!='\0';i++)  if(str[i]=='%')  count++;

  va_list argv;
  va_start(argv, count);
  for(i=0,j=0; str[i]!='\0';i++)
  {
    if(str[i]=='%')
    {
      temp[j] = '\0';
      Serial.print(temp);
      j=0;
      temp[0] = '\0';

      switch(str[++i])
      {
        case 'd': Serial.print(va_arg(argv, int));
                  break;
        case 'l': Serial.print(va_arg(argv, long));
                  break;
        case 'f': Serial.print(va_arg(argv, double));
                  break;
        case 'c': Serial.print((char)va_arg(argv, int));
                  break;
        case 's': Serial.print(va_arg(argv, char *));
                  break;
        default:  ;
      };
    }
    else 
    {
      temp[j] = str[i];
      j = (j+1)%ARDBUFFER;
      if(j==0) 
      {
        temp[ARDBUFFER] = '\0';
        Serial.print(temp);
        temp[0]='\0';
      }
    }
  };
  Serial.println();
  return count + 1;
}
#undef ARDBUFFER
#endif
// END OF REFERENCE CODE 

void setup() {
    Serial.begin(9600);
     initializePlayers(player1Leds, player2Leds, defaultCol1, defaultCol2, player1TopRow, player2TopRow, paddleSize1);
    //initialize pins 
    pinMode(control1, INPUT);
    for (int i=0;i<16; i++){
    pinMode(pins[i],OUTPUT);
    }
    for (int i=0; i<8;i++){
      digitalWrite(cols[i],LOW);
    }
   for (int i=0; i<8; i++){
    digitalWrite(rows[i],HIGH);
   } 
   //turn hex digits into binary digits 
   for (int let=0; let<numLetters; let++){
      for (int i=0; i<8 ; i++){
        String tempString=String(letterHexArrays[let][i], BIN);
        while (tempString.length()<8){
        tempString= "0" + tempString;
        }
          for (int n=0;n<8;n++){
            leds[let][i][n]=tempString.charAt(n)-'0';
          }
        }
    }
    player1Pos = analogRead(control1);
    player2Pos = analogRead(control2);
 }

  
void loop(){
   //initial displays
   if (startGame==0){
      displayLet=(displayLet+1)%numLetters;
      for (int innerLoop=0; innerLoop<10000;innerLoop++){
          displayLetters(displayLet);
      }
     if ((displayLet+1)==numLetters){
       startGame=1;
     }
   }  
   //game starts  
   if (startGame==1){
      count++;
      if (count==8000){
        loop2();
        count=0;
      }
      displayGame();
      }
  }
  
//second, slower loop
void loop2(){ 
  if (gameOverBool==0){
    createPlayer1();
    createPlayer2();
    createBall();
  }
}
  
void displayGame(){
  digitalWrite(rows[row],HIGH);
  row=(row+1)%8;
  for (int col=0; col<8;col++){
    if ((player1Leds[row][col]==1) || (player2Leds[row][col]==1) || (ballLeds[row][col]==1)){
      digitalWrite(cols[col],HIGH);
    }
    else{
      digitalWrite(cols[col],LOW);
    }
  }
  digitalWrite(rows[row],LOW);
}

void displayLetters(int let){
  //turn previous row off
  digitalWrite(rows[row],HIGH);
  row=(row+1)%8;
  for (int col=0; col<8; col++){
    if (leds[let][row][col]==1){
      digitalWrite(cols[col],HIGH);
    }
    else {
      digitalWrite(cols[col],LOW);
    }
  }
   digitalWrite(rows[row],LOW);
}

void getNegative(){
  for (int let=0; let<numLetters; let++){
    for (int row=0; row<8; row++){
      for (int col=0; col<8; col++){
        leds[let][row][col]=(leds[let][row][col]+1)%2;
      }
    }
  }
}
//FANCY STUFF I DIDN'T SHOW IN DEMO ------------------------
//void scroll(int letter){
//  for (int row=0; row<8; row++){
//    int oldTemp=leds[letter][row][7];
//    for (int col=0; col<8; col++){
//      int tempLet=leds[letter][row][col];
//      leds[letter][row][col]=oldTemp;
//      oldTemp=tempLet;
//    }
//  }
//}


//void copyArray (){
//  for (int elem=0; elem<numLetters; elem++){
//    for (int row=0; row<8; row++){
//      for (int col=0; col<8; col++){
//        returnArray[elem][row][col]=leds[elem][row][col];
//      }
//    }
//  }
//}

//void scrollEnd(int letter){
//  for (int row=0; row<8; row++){
//    for (int col=7; col>0; col++){
//      returnArray[letter][row][col]=returnArray[letter][row][col-1];
//      returnArray[letter][row][0]=0;
//    }
//  }
//}
//END ----------------------------------------------------

void initializePlayers(byte player1Matrix[8][8], byte player2Matrix[8][8], int col1, int col2, int row1, int row2, int paddleSize){
  //set size of paddle
  for (int i=0; i<paddleSize; i++){    
    if ((row1+i)<8){
      player1Leds[row1+i][col1]=1;
      player2Leds[row2+i][col2]=1;
    }
  }
}
  
void createPlayer2(){
  int diff = player2Pos - analogRead(control2);
  player2Pos=analogRead(control2);
  if (abs(diff)>increment){
    if (diff>0){
      shiftUp(player2Leds,player2TopRow, defaultCol2, 2);
    }
    else if (diff<0){
     shiftDown(player2Leds,player2TopRow,defaultCol2, 2);
    } 
  }
}

void createPlayer1(){
  int diff = player1Pos - analogRead(control1);
  player1Pos = analogRead(control1);
  if (abs(diff)>increment){
    if (diff>0){
      shiftDown(player1Leds, player1TopRow, defaultCol1, 1);
    }
    else if (diff<0){
      shiftUp(player1Leds,player1TopRow,defaultCol1, 1);
    }
  }
}

void findBall(){
  for (int i=0; i<8; i++){
    for (int n=0; n<8; n++){
      if (ballLeds[i][n]==1){
         ballCoord[0]=i;
         ballCoord[1]=n;
      }
    }
  }
}

void createBall(){
  findBall();
  int row = ballCoord[0];
  int col = ballCoord[1];
  if (col==6){
    //if it is hit by player
    if (player2Leds[row][col+1]==1){
      //move in opposite direction
      moveBall();
      return;
    }
    else {
      //give point to other player;
      ardprintf("Player 1 gained a point! The current score is: %d:%d", player1Points, player2Points);
      player1Points++;
      fade(2);
      if (player1Points>=totalPoints){
        gameOver();
      }
      else if (player1Points<totalPoints){
        if (((player1Points-player2Points) > helpThreshold) && paddleSize2==2){
          changePaddleSize(2,1);
        }
        else if (((player2Points-player1Points) < helpThreshold) && paddleSize1==3){
          changePaddleSize(1,-1);
        }
        restart();
      }
    }
  }
  else if (col==1){
    if (player1Leds[row][col-1]==1){
      moveBall();
      return;
    }
    else{
      ardprintf("Player 2 gained a point! The current score is: %d:%d", player1Points, player2Points);
      player2Points++;
      fade(1);
      if (player2Points>=totalPoints){
        gameOver();
      }
      else if (player2Points<totalPoints){
        if (((player2Points-player1Points) > helpThreshold) && paddleSize1==2){
          changePaddleSize(1,1);
        }
        else if (((player1Points-player2Points) < helpThreshold) && paddleSize2==3){
          changePaddleSize(2,-1);
        }
        restart();
      }
    }
  }
  else{
    moveBall();
  }
}
      
void moveBall(){
   int row = ballCoord[0];
   int col = ballCoord[1];
  if (dir==0){
    if (row>0 && col>1){
      ballLeds[row-1][col-1]=1;
      ballLeds[row][col]=0;
    }
    else if (row>0 && col==1){dir=1;} //touches wall 
    else if (row==0 && col>1){dir=2;} //touches ceiling
    else if (row==0 && col==1){dir=3;} //touches corner
    }
  if (dir==1){//[-1,1]
   if (row>0 && col<6){
     ballLeds[row-1][col+1]=1;
     ballLeds[row][col]=0;
   }
   else if (row>0 && col==6){dir=0;} //hits wall
   else if (row==0 && col<6){dir=3;}//hits ceiling 
   else if (row==0 && col==6){dir=2;} //hits corner 
   }
  if (dir==2){//[1,-1]
    if (row<7 && col>1){
      ballLeds[row+1][col-1]=1;
      ballLeds[row][col]=0;
    }
  else if (row<7 && col==1){dir=3;}//hits wall
  else if (row==7 && col>1){dir=0;}//hits floor
  else if (row==7 && col==1){dir=1;} //hits corner
  }
  if (dir==3){//[1,1]
    if (row<7 && col<6){
      ballLeds[row+1][col+1]=1;
      ballLeds[row][col]=0;
    }
    else if (row<7 && col==6){dir=2;}//hits wall
    else if (row==7 && col<6){dir=1;}//hits floor
    else if (row==7 && col==6){dir=0;}//hits corner
  }
}
  

void shiftUp(byte matrix[8][8], int playerRow, int defaultCol,int player){
  if (player==1){ paddleSize = paddleSize1;} else { paddleSize = paddleSize2;}
  int deleteRow = paddleSize-1;
  if (playerRow>0){
    //creating new row
    matrix[playerRow-1][defaultCol]=1;
    //delete bottom row
    matrix[playerRow+deleteRow][defaultCol]=0;
    //set new top row
    if (player==1){
      player1TopRow-=1;
    }
    else{
      player2TopRow-=1;
    }
  }
}
void shiftDown(byte matrix[8][8], int playerRow, int defaultCol, int player){
  if (player==1){ paddleSize = paddleSize1;} else { paddleSize = paddleSize2;}
  int deleteRow = paddleSize-1;
  if ((playerRow+deleteRow)<7){
    //creating new row
    matrix[playerRow+paddleSize][defaultCol]=1;
    //delete bottom row
    matrix[playerRow][defaultCol]=0;
    //set new top row
    if (player==1){
      player1TopRow+=1;
    }
    else{
      player2TopRow+=1;
    }
  }
}

int initializeBall(){
  ballLeds[ballRow][ballCol]=1;
  //initialize a direction
  return random(4);
}

void gameOver(){
  gameOverBool=1;
  delay(5000);
}

void restart(){
  reinitialize();
  dir=initializeBall();
  gameOverBool=0;
}

void reinitialize(){
  for (int i=0; i<8; i++){
    for (int n=0; n<8; n++){
      ballLeds[i][n]=0;
    }
  }
}
  
void changePaddleSize(int player, int change){
  if (player==1){paddleSize=paddleSize1;} else {paddleSize=paddleSize2;}
  if (player==1){topRow=player1TopRow;} else {topRow=player2TopRow;}
  if (change==1){
      if (paddleSize==2){
        //situation 1 where there is space to grow paddle 
        if (topRow<6){
          if (player==1){
            paddleSize1=3;
            player1Leds[topRow+2][defaultCol1]=1;
          }
          else {
            paddleSize2=3;
            player2Leds[topRow+2][defaultCol2]=1;
          }
        }
        //situation where there is now more space
        else if (topRow==6){
          if (player==1){
            paddleSize1=3;
            player1Leds[topRow-1][defaultCol1]=1;
            player1TopRow-=1;
          }
          else {
            paddleSize2=3;
            player2Leds[topRow-1][defaultCol2]=1;
            player2TopRow-=1;
          }
        }
      }
  }
  else if (change==-1){
    //delete the extra
    if (paddleSize==3){
      if (player==1){
        paddleSize1=2;
        player1Leds[topRow+2][defaultCol1]=0;
      }
      else if (player ==2) {
        paddleSize2=2;
        player2Leds[topRow+2][defaultCol2]=0;
      }
    }
  }
}

void fade(int player){
  for (int i=0; i<60; i++){
    if (player==1){
      for (int n=0; n<50; n++){
        digitalWrite(rows[row],HIGH);
        row=(row+1)%8;
        for (int col=0; col<8;col++){
          if ((player1Leds[row][col]==1)){
            digitalWrite(cols[col],HIGH);
          }
          else{
            digitalWrite(cols[col],LOW);
          }
        }
        if (row%8==0){
          delay(i*i/500.0);}
        digitalWrite(rows[row],LOW);
        if (row%8==0){
          delay(i*.005);}
      }
    }
    else if (player==2){
      for (int n=0; n<50; n++){
        digitalWrite(rows[row],HIGH);
        row=(row+1)%8;
        for (int col=0; col<8;col++){
          if ((player2Leds[row][col]==1)){
            digitalWrite(cols[col],HIGH);
          }
          else{
            digitalWrite(cols[col],LOW);
          }
        }
        if (row%8==0){
          delay(i*i/500.0);}
        digitalWrite(rows[row],LOW);
        if (row%8==0){
          delay(i*.005);}
      }
    }
  }
}




