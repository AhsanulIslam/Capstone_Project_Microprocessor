#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int adc_key_val[5] ={95,98,105,125,140};
bool gamestarted=false;
byte mySnake[9][8] = 
{
{ B00000,
  B00000,
  B00011,
  B00110,
  B01100,
  B11000,
  B00000,
},
{ B00000,
  B11000,
  B11110,
  B00011,
  B00001,
  B00000,
  B00000,
},
{ B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B01110,
},
{ B00000,
  B00000,
  B00011,
  B01111,
  B11000,
  B00000,
  B00000,
},
{ B00000,
  B11100,
  B11111,
  B00001,
  B00000,
  B00000,
  B00000,
},
{ B00000,
  B00000,
  B00000,
  B11000,
  B01101,
  B00111,
  B00000,
},
{ B01100,
  B11111,
  B01110,
  B11011,
  B11111,
  B01110,
  B00000,
},
{ B00000,
  B00000,
  B00000,
  B01000,
  B10000,
  B01000,
  B00000,
},
{ B10001,
  B01010,
  B00100,
  B01010,
  B10001,
  B01010,
  B00100,
}
};

boolean levelz[5][2][16] = {
{{true,false,true,false,true,false,false,false,false,false,false,false,false,false,false,false},
{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false}},

{{true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true},
{true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true}},

{{true,false,false,false,true,false,false,false,false,false,false,true,false,false,false,true},
{true,false,false,false,false,false,false,false,true,false,false,false,false,false,false,true}},

{{true,false,true,false,false,false,false,false,false,true,false,false,false,true,false,false},
{false,false,false,false,true,false,false,true,false,false,false,true,false,false,false,true}}
};

bool ageSelect=true;
bool printed=false;
int ageARR[5]={1,2,3,4,5};
int ageRange=60;
int selectedAge=15;
bool declareOnce=false;
int timeRange=160;
int selectedTime=1;

bool timesUp=false;
bool setCountDown=false;
int timer=1;




unsigned long time, timeNow; 
int gameSpeed;
boolean skip, gameOver, gameStarted;
int olddir;
int selectedLevel,levels;


int NUM_KEYS = 5;
int adc_key_in;
int key=-1;
int oldkey=-1;

boolean x[16][80];
byte myChar[8];
byte nullChar[8] = { 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0 };
boolean special;

struct partdef
{
  int row,column,dir; //0 - up, 1 - down, 2 - right, 3 - left
  struct partdef *next;
}; 
typedef partdef part;

part *head, *tail;
int i,j,collected;
long pc,pr;

void drawMatrix()
{
  int cc=0;
  if (!gameOver)
  {
  x[pr][pc] = true;
  //for (i=0;i<8;i++) lcd.createChar(i, nullChar);
  for(int r=0;r<2;r++)
  {
    for(int c=0;c<16;c++)
    {
      special = false;
      for(int i=0;i<8;i++)
      {
        byte b=B00000;
        if (x[r*8+i][c*5+0]) {b+=B10000; special = true;}
        if (x[r*8+i][c*5+1]) {b+=B01000; special = true;}
        if (x[r*8+i][c*5+2]) {b+=B00100; special = true;}
        if (x[r*8+i][c*5+3]) {b+=B00010; special = true;}
        if (x[r*8+i][c*5+4]) {b+=B00001; special = true;}
        myChar[i] = b;
      }
      if (special) 
      {
        lcd.createChar(cc, myChar);
        lcd.setCursor(c,r);
        lcd.write(byte(cc));
  cc++;
      }
      else 
      {
        lcd.setCursor(c,r);
        if (levelz[selectedLevel][r][c]) lcd.write(255);
        else lcd.write(254);
      }
    }
  } 
  }
}

void freeList()
{
  part *p,*q;
  p = tail;
  while (p!=NULL)
  {
    q = p;
    p = p->next;
    free(q);
  }
  head = tail = NULL;
}
int best=0;
void gameOverFunction()
{
  delay(1000);
  lcd.clear();
  freeList();
  lcd.setCursor(0,0);
  //lcd.print("playtime");
  //lcd.print(selectedTime);
  lcd.print(" AG");
  lcd.print(selectedAge);
  lcd.setCursor(0,1);
  lcd.print("Score:");
  lcd.print(collected);
  lcd.print(" Best:");
  if(best<collected)best=collected;
            
  lcd.print(best);
  delay(1000);
}

void growSnake()
{
  part *p;
  p = (part*)malloc(sizeof(part));
  p->row = tail->row;
  p->column = tail->column;
  p->dir = tail->dir;
  p->next = tail;
  tail = p;
}

void newPoint()
{

  part *p;
  p = tail;
  boolean newp = true;
  while (newp)
  {
    pr = random(16);
    pc = random(80);
    newp = false;
    if (levelz[selectedLevel][pr / 8][pc / 5]) newp=true;
    while (p->next != NULL && !newp) 
    {
      if (p->row == pr && p->column == pc) newp = true;
      p = p->next;
    }
  }
  
  if (collected < 13 && gameStarted) growSnake();
}

void moveHead()
{
  switch(head->dir) // 1 step in direction
  {
    case 0: head->row--; break;
    case 1: head->row++; break;
    case 2: head->column++; break;
    case 3: head->column--; break;
    default : break;
  }
  if (head->column >= 80) head->column = 0;
  if (head->column < 0) head->column = 79;
  if (head->row >= 16) head->row = 0;
  if (head->row < 0) head->row = 15;
  
  if (levelz[selectedLevel][head->row / 8][head->column / 5]) gameOver = true; // wall collision check
  
  part *p;
  p = tail;
  while (p != head && !gameOver) // self collision 
  {
    if (p->row == head->row && p->column == head->column) gameOver = true;
    p = p->next;
  }
  if (gameOver)
    gameOverFunction();
  else
  {
  x[head->row][head->column] = true;
  
  if (head->row == pr && head->column == pc) // point pickup check
  {
    collected++;
    if (gameSpeed < 25) gameSpeed+=3;
    newPoint();
  }
  }
}

void moveAll()
{
  part *p;
  p = tail;
  x[p->row][p->column] = false;
  while (p->next != NULL) 
  {
    p->row = p->next->row;
    p->column = p->next->column;
    p->dir = p->next->dir;
    p = p->next;
  }
  moveHead();
}

void createSnake(int n) // n = size of snake
{
  for (i=0;i<16;i++)
    for (j=0;j<80;j++)
      x[i][j] = false;
    
  part *p, *q;
  tail = (part*)malloc(sizeof(part));
  tail->row = 7;
  tail->column = 39 + n/2;
  tail->dir = 3;
  q = tail;
  x[tail->row][tail->column] = true;
  for (i = 0; i < n-1; i++) // build snake from tail to head
  {
    p = (part*)malloc(sizeof(part));
    p->row = q->row;
    p->column = q->column - 1; //initial snake id placed horizoltally
    x[p->row][p->column] = true;
    p->dir = q->dir;
    q->next = p;
    q = p;
  }
  if (n>1)
  {
    p->next = NULL;
    head  = p;
  }
  else 
  {
    tail->next = NULL;
    head = tail;
  }
}

void startF()
{
  gameOver = false;
  gameStarted = false;
  selectedLevel = 1;
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Select level: 1");
  for(i=0;i<8;i++)
  {
    lcd.createChar(i,mySnake[i]);
    lcd.setCursor(i+4,1);
    lcd.write(byte(i));
  }
  collected = 0;
  gameSpeed = 8;
  createSnake(3);
  time = 0;
}
void setup()
{
  levels = 5;
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Select Age:");
  lcd.setCursor(0,1);
  //lcd.print("Set time:");

  pinMode(7,OUTPUT);
  
 
}

void loop()
{
  
  if(ageSelect)
  {
   
    
    
   adc_key_in = analogRead(0); 
   key = get_key(adc_key_in);
    if (key != oldkey)   // if keypress is detected
   {
      delay(1);  // wait for debounce time
     adc_key_in = analogRead(0);    // read the value from the sensor 
     key = get_key(adc_key_in);
      
       if (key != oldkey)    
     {   
       oldkey = key;
       if (key >=0)
       {
         olddir = head->dir;
         if (key==1 && selectedAge<ageRange) selectedAge++;
         if (key==2 && selectedAge>15) selectedAge--;
         //if (key==0&& selectedTime<timeRange) selectedTime++;
         //if (key==3 && selectedTime>1) selectedTime--;
         if (key==4) 
         {
          ageSelect=false;
         }
         else
         {
           lcd.setCursor(13,0);
           lcd.print(selectedAge);
           //lcd.setCursor(13,1);
           //lcd.print(selectedTime);
         }
       }
     }
   }
    
   
    
  }
   
  
  else
  {
    
    if(!declareOnce)
    {
       
     delay(1000);
    startF();
    delay(500);
    key=-1;
    oldkey=-1;
      declareOnce=true;
    }
   
   if(!timesUp)
   {
     delay(1000);
     timer++;
   } 
   if(timer==selectedTime)
   {//timesUp=gameOver=true;
    // gameOverFunction();
   }
   
  if (!gameOver && !gameStarted)
  {
   adc_key_in = analogRead(0);    // read the value from the sensor 
   key = get_key(adc_key_in);  // convert into key press
   if (key != oldkey)   // if keypress is detected
   {
     delay(50);  // wait for debounce time
     adc_key_in = analogRead(0);    // read the value from the sensor 
     key = get_key(adc_key_in);    // convert into key press
     if (key != oldkey)    
     {   
       oldkey = key;
       if (key >=0)
       {
         olddir = head->dir;
         if (key==1 && selectedLevel<levels) selectedLevel++;
         if (key==2 && selectedLevel>1) selectedLevel--;
         if (key==4) 
         {
           lcd.clear();
           selectedLevel--;
           newPoint();
           gameStarted = true;
         }
         else
         {
           lcd.setCursor(14,0);
           lcd.print(selectedLevel);
         }
       }
     }
   }
  }
  if (!gameOver && gameStarted)
  {
    digitalWrite(7,HIGH);
   skip = false; //skip the second moveAll() function call if the first was made
   
   adc_key_in = analogRead(0);    // read the value from the sensor 
   key = get_key(adc_key_in);
    // convert into key press
   if (key != oldkey)   // if keypress is detected
   {
     delay(50);  // wait for debounce time
     adc_key_in = analogRead(0);    // read the value from the sensor 
     key = get_key(adc_key_in);    // convert into key press
     if (key != oldkey)    
     {   
       oldkey = key;
       if (key >=0)
       {
         olddir = head->dir;
         if (key==0 && head->dir!=3) head->dir = 2;
         if (key==1 && head->dir!=1) head->dir = 0;
         if (key==2 && head->dir!=0) head->dir = 1;
         if (key==3 && head->dir!=2) head->dir = 3;
         
         if (olddir != head->dir)
         {
           skip = true;
           delay(1000/gameSpeed);
           moveAll();
           drawMatrix();
         }
       }
     }
   }
   if (!skip)
   {
     timeNow = millis();
     if (timeNow - time > 1000 / gameSpeed)
     {
       moveAll();
       drawMatrix();
       time = millis();
     }
   }
  }
  if(gameOver)
  {
    digitalWrite(7,LOW);
   adc_key_in = analogRead(0);    // read the value from the sensor 
   key = get_key(adc_key_in);  // convert into key press
   if (key != oldkey)   // if keypress is detected
   {
     delay(50);  // wait for debounce time
     adc_key_in = analogRead(0);    // read the value from the sensor 
     key = get_key(adc_key_in);    // convert into key press
     if (key != oldkey)    
     {   
       oldkey = key;
       if (key >=0)
       {
          startF();
       }
     }
   }
   
  }
    
   }
  
}

int get_key(unsigned int input)
{
  
    int k;
    for (k = 0; k < NUM_KEYS; k++)
    {
      if (input < adc_key_val[k])
      {
        return k;
      }
    }   
    if (k >= NUM_KEYS)k = -1;  // No valid key pressed
    return k;
}
