/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using SPI to communicate
 4 or 5 pins are required to interface.

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <virtuabotixRTC.h>  
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   5 // SDA, D1,MOSI
#define OLED_CLK    6  // SCK, D0,SCL,CLK
#define OLED_DC    3  // DC, A0
#define OLED_CS    2  // CS, Chip Select
#define OLED_RESET 4  // RES, RST,RESET
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/* Comment out above, uncomment this block to use hardware SPI
#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);
*/

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
//#define SSD1306_LCDHEIGHT 64
//#if (SSD1306_LCDHEIGHT != 64)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
//#endif

#define DS1302_RESET 9  // RES, RST,RESET
#define DS1302_DAT   8  // DATA
#define DS1302_CLK   7  // SCK, CLK

virtuabotixRTC myRTC(DS1302_CLK, DS1302_DAT, DS1302_RESET);
char *day_s[] = {"", "CHU NHAT", "THU 2", "THU 3", "THU 4", "THU 5", "THU 6", "THU 7"};

// Mang is a working date - Ngay chi dinh lam viec - ngay lam bu. localNoteArray = 0
char *working_date[][4] = {
  {0, 0, 0, "Lam viec"},
};

// Mang is hour working date - Gio ngay chi dinh lam viec - gio ngay lam bu. localNoteArray = 1
char *time_working_date[][5] = {
  {0, 30, 5, "BT sang", 0},
  {0, 10, 6, "An sang", 2},
  {0, 0, 7, "Lam viec sang", 7},
  {0, 0, 11, "Het lam viec", 8},
  {0, 10, 11, "An trua", 3},
  {0, 15, 13, "BT chieu", 0},
  {0, 30, 13, "Lam viec chieu", 7},
  {0, 30, 16, "Het lam viec", 8},
  {0, 30, 17, "An chieu", 4},
  {0, 45, 20, "Diem danh", 5},
  {0, 30, 17, "Het the thao", 6},
  {0, 0, 18, "Ha Co", 7},
  {0, 0, 19, "Xem thoi su", 8},
  
  {30, 7, 9, "PHUONG ANH 1", 1},
  {30, 8, 9, "PHUONG ANH 2", 4},
  {30, 9, 9, "PHUONG ANH 3", 3},
};

// Mang is a date off - Ngay chi dinh nghi - ngay le. localNoteArray = 2
char *date_off[][4] = {
  {0, 0, 0, "NGAY NGHI"},
  {30, 4, 2021, "NGHI LE 30/4"},
  {3, 5, 2021, "NGHI BU 1/5"},
  {2, 9, 2021, "QUOC KHANH"},
  {3, 9, 2021, "QUOC KHANH"},
  {3, 1, 2022, "NGHI BU 1/1"},
};
// Mang is hour date off - Gio ngay chi dinh nghi - gio ngay le. localNoteArray = 3
char *time_date_off[][5] = {
  {0, 0, 6, "BT sang", 1},
  {0, 0, 14, "BT chieu", 1},
  {0, 30, 6, "An sang", 2},
  {0, 30, 10, "An trua", 3},
  {0, 30, 17, "An chieu", 4},
  {0, 45, 20, "Diem danh", 5},

  {10, 17, 8, "TEST1", 5},
  {20, 18, 8, "TEST2", 4},
  {30, 19, 8, "TEST3", 3}
};

// Functions
bool what_is_default_days(int today);
int is_a_working_date(int date, int mouth, int year);
int is_broadcast_time_working_date(int seconds, int minute, int hour);
int is_a_date_off(int date, int mouth, int year);
int is_broadcast_time_date_off(int seconds, int minute, int hour);
void displayTime(int seconds, int minutes, int hours, int dayofweek, int dayofmonth, int month, int year, int note, int localNoteArray);
// end functions

// CONST INDEX
size_t size_working_date = sizeof(working_date)/sizeof(working_date[0]);
size_t size_time_working_date = sizeof(time_working_date)/sizeof(time_working_date[0]);
size_t size_date_off = sizeof(date_off)/sizeof(date_off[0]);
size_t size_time_date_off = sizeof(time_date_off)/sizeof(time_date_off[0]);
// end

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }


  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

// Dat thoi gian ban dau
//   myRTC.setDS1302Time(50, 4, 9, 3, 27, 4, 2021);
  
}

void loop() {
  // This allows for the update of variables for time or accessing the individual elements.                //|
  myRTC.updateTime();
  int seconds = myRTC.seconds;
  int minutes = myRTC.minutes;
  int hours = myRTC.hours;
  int dayofmonth = myRTC.dayofmonth;
  int dayofweek = myRTC.dayofweek;
  int month = myRTC.month;
  int year = myRTC.year;
  int localNoteArray=0; // 0 | 1 | 2 | 3. default = 0 - vi tri mang ngay nghi
  int note=0;           // int. default = 0 - in ra thong bao so 0
  int indexPlay=-1;     // int. default = -1 - khong choi ban nhac nao. Chi so localNoteArray cua mang chua ma ban nhac

  // Xem ngay
  bool is_days = what_is_default_days(myRTC.dayofweek);

  // Neu la 2-6
  if(is_days){
    // Roi vao ngay nghi khong
    int index_off_1 = is_a_date_off(dayofmonth, month, year);
    // Nghi thoi !
    if(index_off_1 != -1){
      // Toi gio phat bao chua
      int is_broadcast_1 = is_broadcast_time_date_off(seconds, minutes, hours);
      // PHAT BAO
      if(is_broadcast_1 != -1){
        indexPlay = 3;
        localNoteArray=3;
        note = is_broadcast_1;
      }
      // Khong phat
      else{
        indexPlay = -1;
        note = index_off_1;
        localNoteArray=2;
      }
      
    }
    // Di lam thoi !
    else{
      // Toi gio phat bao chua
      int is_broadcast_2 = is_broadcast_time_working_date(seconds, minutes, hours);
      // PHAT BAO
      if(is_broadcast_2 != -1){
        indexPlay = 1;
        localNoteArray=1;
        note = is_broadcast_2;
      }
      // 
      else{
        indexPlay = -1;
        note = 0;
        localNoteArray=0;
      }
    }
  }
  // Neu la 7,CN
  else{
    // Roi vao ngay lam viec bu khong
    int index_work_1 = is_a_working_date(dayofmonth, month, year);
    // Di lam thoi !
    if(index_work_1 != -1){
      // Toi gio phat bao chua
      int is_broadcast_3 = is_broadcast_time_working_date(seconds, minutes, hours);
      // PHAT BAO
      if(is_broadcast_3 != -1){
        indexPlay = 1;
        localNoteArray=1;
        note = is_broadcast_3;
      }
      // Khong phat
      else{
        indexPlay = -1;
        note = index_work_1;
        localNoteArray=0;
      }
      
    }
    // Nghi thoi !
    else{
      // Toi gio phat bao chua
      int is_broadcast_4 = is_broadcast_time_date_off(seconds, minutes, hours);
      // PHAT BAO
      if(is_broadcast_4 != -1){
        indexPlay = 3;
        localNoteArray=3;
        note = is_broadcast_4;
      }
      // 
      else{
        indexPlay = -1;
        note = 0;
        localNoteArray=2;
      }
    }
  }

 // SHow thoi gian
displayTime(seconds, minutes, hours, dayofweek, dayofmonth, month, year, note, localNoteArray);

// TEST
  Serial.print("note: ");
  Serial.println(note);
  Serial.print("indexPlay: ");
  Serial.println(indexPlay);
  Serial.print("localNoteArray: ");
  Serial.println(localNoteArray);
  // Phat nhac khong
  if(indexPlay==1){
    int songLocal1 = time_working_date[note][4];
    Serial.print("note: ");
    Serial.println(note);
    Serial.print("Ban nhac so: ");
    Serial.println(songLocal1);
  }
  if(indexPlay==3){
    int songLocal2 = time_date_off[note][4];
    Serial.print("note: ");
    Serial.println(note);
    Serial.print("Ban nhac so: ");
    Serial.println(songLocal2);
  }
  
  
}

bool what_is_default_days(int today){
  switch (today)
  {
  case 2: case 3: case 4: case 5: case 6:
    return true;
  case 1: case 7:
    return false;
  default:
    return false;
  }
}

int is_a_working_date(int date, int mouth, int year){
  int note = -1;
  for (int i = 0; i < size_working_date; i++)
  {
    if(working_date[i][0]==date && working_date[i][1]==mouth && working_date[i][2]==year){
      note = i;
      break;
    }
  }
  return note;
}

int is_broadcast_time_working_date(int seconds, int minute, int hour){
  int note = -1;
  for (int i = 0; i < size_time_working_date; i++)
  {
    if(time_working_date[i][0]==seconds && time_working_date[i][1]==minute && time_working_date[i][2]==hour){
      note = i;
      break;
    }
  }
  return note;
}

int is_a_date_off(int date, int mouth, int year){
  int note3 = -1;
  for (int i = 0; i < size_date_off; i++)
  {
    if(date_off[i][0]==date && date_off[i][1]==mouth && date_off[i][2]==year){
      note3 = i;
      break;
    }
  }
  return note3;
}

int is_broadcast_time_date_off(int seconds, int minute, int hour){
  int note4 = -1;
  for (int i = 0; i < size_time_date_off; i++)
  {
    if(time_date_off[i][0]==seconds && time_date_off[i][1]==minute && time_date_off[i][2]==hour){
      note4 = i;
      break;
    }
  }
  return note4;
}

// Show time
void displayTime(int seconds, int minutes, int hours, int dayofweek, int dayofmonth, int month, int year, int note, int localNoteArray){
  String dot = ":";
  String soc = "/";
  String hour_string = String(hours);
  String minute_string = String(minutes);
  String year_string = String(year%100);
  String month_string = String(month);
  String dayofmonth_string = String(dayofmonth);

  hour_string = hours<10 ? "0"+hour_string : hour_string;
  minute_string = minutes<10 ? "0"+minute_string : minute_string;
  month_string = month<10 ? "0"+month_string : month_string;
  dayofmonth_string = dayofmonth<10 ? "0"+dayofmonth_string : dayofmonth_string;

  delay(100);    
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(day_s[dayofweek]);

  //  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,10);
  display.println(dayofmonth_string + soc + month_string + soc + year_string);

  display.setTextSize(2.8);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(52,9);
  display.println(hour_string + dot + minute_string); 

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(115,16);             // Start at top-left corner
  display.println(seconds<10 ? "0"+String(seconds) : seconds);

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,25);             // Start at top-left corner
  switch (localNoteArray)
  {
  case 0:
    display.println(working_date[note][3]);
    break;
  
  case 1:
    display.println(time_working_date[note][3]);
    break;
  
  case 2:
    display.println(date_off[note][3]);
    break;
  
  case 3:
    display.println(time_date_off[note][3]);
    break;

  default:
    break;
  }

  display.display();
  delay(500);
}
