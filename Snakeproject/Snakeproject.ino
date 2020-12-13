#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


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
{{true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false}},

{{true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true},
{true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true}},

{{true,false,false,false,true,false,false,false,false,false,false,true,false,false,false,true},
{true,false,false,false,false,false,false,false,true,false,false,false,false,false,false,true}},

{{true,false,true,false,false,false,false,false,false,true,false,false,false,true,false,false},
{false,false,false,false,true,false,false,true,false,false,false,true,false,false,false,true}}
};


unsigned long time, timeNow; 
int gameSpeed;
boolean skip, gameOver, gameStarted;
int olddir;
int selectedLevel,levels;

int adc_key_val[5] ={95,98,105,125,140};
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
        byte blank[8]=
        { B10001,
        B01010,
      B00100,
      B01010,
      B10001,
        B01010,
        B00100,
      };
        lcd.setCursor(c,r);
        if (levelz[selectedLevel][r][c]) lcd.write(255);
        else 
        {
      lcd.write(254);
        }
      }
    }
  } 
  }
}

void startF(int speed)
{
  gameOver = false;
  gameStarted = false;
  selectedLevel = 1;
  collected = 0;
  gameSpeed = speed;
 
  time = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Select level: 1");
  for(i=0;i<8;i++)
  {
    lcd.createChar(i,mySnake[i]);
    lcd.setCursor(i+4,1);
    lcd.write(byte(i));
  }

}
void setup()
{
  levels = 5; //number of lvls
  lcd.begin(16, 2);
  startF(8);
}

void loop()
{
  
 
  
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
