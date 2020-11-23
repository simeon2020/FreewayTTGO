
//240x135
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
#include "freeway.h"
#include "playerCar.h"
#include "EnemyCar_01.h"
#include "EnemyCar_02.h"
#include "EnemyCar_03.h"
#include "EnemyCar_04.h"
#define TFT_GREY 0x5AEB

float ys = 1;

float x = random(30, 100); //coordinates of ball
float y = 1;
int ny = y; //coordinates of previous position
int nx = x;

float px = 45; //start pos
int   pxn = px;

int score = 0;
int hiscore = 0;
int level = 1;

int gameState = 0;
int lane;
int lastLane;
int damage = 0;

void setup(void) {
  Serial.begin(115200);
  Serial.println();
  pinMode(0, INPUT);
  pinMode(35, INPUT);
  tft.init();
  tft.setRotation(0);

  tft.setSwapBytes(true);
  tft.pushImage(0, 0,  135, 240, freeway); //boot screen
}

int pom = 0;
int gameSpeed = 6000;

class Enemy {
  public:
    int image ;
    int elength;
    int posx;
    float posy;
    float espeed;
    bool behind = false;
    Enemy ( int image, int elength, int posx, float posy, float espeed, bool behind) {
      this->image = image;
      this->elength = elength; //enemy length , for collision , width assume ~19 :)
      this->posx = posx;
      this->posy = posy;
      this->espeed = espeed;
      this -> behind= behind;
    }
    void draw() {
      tft.fillRect(posx, posy - espeed - 1, 19, espeed + 2, TFT_BLACK);
      if (image == 1) {
        elength = 40;
        tft.pushImage(posx, posy, 19, elength, EnemyCar_01);
      }
      else if (image == 2) {
        elength = 42;
        tft.pushImage(posx, posy, 19, elength, EnemyCar_02);
      }
      else if (image == 3) {
        elength = 44;
        tft.pushImage(posx, posy, 19, elength, EnemyCar_03);
      }
      else if (image == 4) {
        elength = 48;
        tft.pushImage(posx, posy, 19, elength, EnemyCar_04);
      }
     
      // do AABB checkBehind
      if (!behind) {
        posy += espeed; //move enemy car if not behind else brake.
      } else {
        posy +=1.2;
        behind = false;
      }

      if (posy > 242) { //enemy passed, make new one
        score += 1;
        if (damage >0) { //cap damage recovery
          damage -= 1;
         }
        drawDamage(damage);
        // Serial.println(score);
        posy = -48;
        
        getLane (lastLane);
        lastLane = lane;
        posx = 5 + lane * 25;
        image = random (4) + 1;
        espeed = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + 1; //between 1 and 2
        //however
        if (image == 4) { //trucks are slow
          espeed = .6 ;
        }
        // Serial.println(elength);
      }
    }
    void getLane (int lastLane) {
      lane = random (5);
      if (lane == lastLane)
      {getLane(lastLane);}
      
    }
    void checkBehind(Enemy a,Enemy b,Enemy c) {
if (posy>-8) {
      if (  posx == a.posx && posy+elength >= a.posy ) { 
             // Serial.println(posx);
     // Serial.print("length ");
     // Serial.print(posy+elength);
     // Serial.println();
     // Serial.println(e2.posx);
     // Serial.println(a.posy);
        behind = true;
      
     //    Serial.println("TRUE");
      }
      else if (posx == b.posx && posy+elength >= b.posy) { 
        behind = true;

      }
      else if (posx == c.posx && posy+elength >= c.posy) { 
        behind = true;

      }
      else {
        behind = false;
      }

      }
      //check against player
      if (posy+elength >=195 && posy <= 235) {

        if (posx+19>pxn && posx<=pxn+19) {
          damage+=1;
          drawDamage(damage);
         // Serial.println(damage);
        }
      }
    }
  void reset (){
  posy = -40-random(200);
  }
  private:
};

Enemy e1 = Enemy(1, 40, 35, -40, 1,0);
Enemy e2 = Enemy(2, 42, 80, -40, 1.5,0);
Enemy e3 = Enemy(3, 44, 50, -40, 1.25,0);
Enemy e4 = Enemy(4, 48, 5, -40, .7,0);

void drawDamage(int damage){
tft.fillRect(133, 40, 2, 240, TFT_RED);  
tft.fillRect(133, 40, 2, damage*2, TFT_BLACK);
}

void loop() {

  if (gameState == 0)
  {
    if (digitalRead(0) == 0 || digitalRead(35) == 0)
    {
      if (pom == 0) { 
        score=0;
        damage = 0;

        tft.fillScreen(TFT_BLACK);
        tft.fillRect(133, 40, 2, 240, TFT_RED);               //energy bar
          
        e1.reset();
        e2.reset();
        e3.reset();
        e4.reset();

        tft.drawLine(0, 0, 0, 240, TFT_GREY);
        tft.drawLine(132, 0, 132, 240, TFT_GREY);

        gameState = gameState + 1;
        pom = 1;
      }

    } else {
      pom = 0;
    }
  }

  if (gameState == 1)         //start game
  {
    // road markings
    {
      for (int i = 0; i < 7; i++) {
        tft.drawLine(26, -20 + i * 40 - 1 + y, 26, -20 + i * 40 - 1 + 20 + y, TFT_BLACK);
        tft.drawLine(26, -20 + i * 40 + y, 26, -20 + i * 40 + 20 + y, TFT_GREY);
        tft.drawLine(52, -20 + i * 40 - 1 + y, 52, -20 + i * 40 - 1 + 20 + y, TFT_BLACK);
        tft.drawLine(52, -20 + i * 40 + y, 52, -20 + i * 40 + 20 + y, TFT_GREY);
        tft.drawLine(78, -20 + i * 40 - 1 + y, 78, -20 + i * 40 - 1 + 20 + y, TFT_BLACK);
        tft.drawLine(78, -20 + i * 40 + y, 78, -20 + i * 40 + 20 + y, TFT_GREY);
        tft.drawLine(104, -20 + i * 40 - 1 + y, 104, -20 + i * 40 - 1 + 20 + y, TFT_BLACK);
        tft.drawLine(104, -20 + i * 40 + y, 104, -20 + i * 40 + 20 + y, TFT_GREY);
      }
      y += .5;
      if (y >= 40) {
        y = 0;
      }

    }
    e1.checkBehind(e2,e3,e4);
    e1.draw();
    
    e2.checkBehind(e1,e3,e4);
    e2.draw();
    e3.checkBehind(e2,e1,e4);
    e3.draw();
    e4.checkBehind(e2,e3,e1);
    e4.draw();
    // player movement
    if (px >= 2 && px <= 109) {
      if (digitalRead(0) == 0)
        px = px - 1;
      if (digitalRead(35) == 0)
        px = px + 1;
    }
    //constraints
    if (px <= 3)
      px = 4;

    if (px >= 108)
      px = 107;
    if (int(px) != pxn) {

      //delete player
      tft.fillRect(pxn, 195, 19, 40, TFT_BLACK);
      pxn = px;
    }

    if (damage >= 100) {
      gameState = 2;
    }
    tft.pushImage(px, 195,  19, 40, playerCar);
    delayMicroseconds(gameSpeed);
  }

  if (gameState == 2)             //game over
  {
    if (score > hiscore) 
    { 
      hiscore = score;            //do something fancy?
      }
    
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 40, 1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);  tft.setTextSize(2);
    tft.println("GAME OVER");
    tft.setCursor(13, 90, 1);
    tft.println("SCORE:" );
    tft.setTextSize(1);
    tft.println();
    tft.setTextSize(3);
    tft.println("   " + String(score));
    tft.setTextSize(2);
    tft.setCursor(13, 170, 1);
    tft.println("HiScore:");
    tft.setTextSize(1);
    tft.println();
    tft.setTextSize(3);
    tft.println("   " + String(hiscore));

    delay(1000);
    gameState = 0;
  }
}
