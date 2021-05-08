/**************************************************************************
 This is an application in plance AUTOMATIC BROADCAST DEVICE

 Pick four main modules:
 1. Arduino nano v3.0
 2. RTC DS1307 | DS1302 | DS3231
 3. OLED 0.96 inch 128x64 pixcel
 4. DFPlayer mini

 and eight dependencies modules
 1. Amplyfier PAM-8403
 2. Loudspeack 4omh-5w
 3. 1 MicroSD(512 Mb) maximum 32Gb
 4. 12V DC --> 3.3v 5v 12v converter
 5. Adapter 12v DC
 6. Wire
 7. Bearboard
 8. 1 register 1K ohms. brown-black-red

 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <virtuabotixRTC.h>  
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Khoi tao danh cho DFPLAYER MINI
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI  5 // SDA, D1,MOSI
#define OLED_CLK   6  // SCK, D0,SCL,CLK
#define OLED_DC    3  // DC, A0
#define OLED_CS    2  // CS, Chip Select
#define OLED_RESET 4  // RES, RST,RESET
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
#define NUMFLAKES     10 // Number of snowflakes in the animation example
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

// DS1302 REAL TIEM CLOCK
#define DS1302_RESET 9  // RES, RST,RESET
#define DS1302_DAT   8  // DATA
#define DS1302_CLK   7  // SCK, CLK
virtuabotixRTC myRTC(DS1302_CLK, DS1302_DAT, DS1302_RESET);
char *day_s[] = {"", "CHU NHAT", "THU 2", "THU 3", "THU 4", "THU 5", "THU 6", "THU 7"};

// Mang is a working date - Ngay chi dinh lam viec - ngay lam bu. localNoteArray = 0
char *working_date[][4] = {
  {0, 0, 0, "LAM VIEC"},
};

// Mang is hour working date - Gio ngay chi dinh lam viec - gio ngay lam bu. localNoteArray = 1
char *time_working_date[][5] = {
  {0, 30, 5, "BT SANG", 1},
  {0, 10, 6, "AN SANG", 2},
  {0, 0, 7, "LAM VIEC SANG", 3},
  {0, 0, 11, "HET LAM VIEC", 4},
  {0, 10, 11, "AN TRUA", 2},
  {0, 15, 13, "BT CHIEU", 1},
  {0, 30, 13, "LAM VIEC CHIEU", 3},
  {0, 30, 17, "HET LAM VIEC", 4},
  {0, 10, 18, "AN CHIEU", 2},
  {0, 45, 20, "DIEM DANH", 5},
};

// Mang is a date off - Ngay chi dinh nghi - ngay le. localNoteArray = 2
char *date_off[][4] = {
  {0, 0, 0, "NGAY NGHI"},
  /*  NAM 2021 */
  // NGHI LE GIAI PHONG MN & QUOC TET LAO DONG
  {30, 4, 2021, "NGHI LE"},
  {3, 5, 2021, "NGHI BU 1/5"},
  // NGAY QUOC KHANH
  {2, 9, 2021, "QUOC KHANH"},
  {3, 9, 2021, "QUOC KHANH"},

  /*  NAM 2022 */
  // NGHI TET DUONG
  {3, 1, 2022, "NGHI BU 1/1"},
  // NGHI TET NGUYEN DAN
  {30, 1, 2022, "NGHI TET"},
  {31, 1, 2022, "NGHI TET"},
  {1, 2, 2022, "MONG 1 TET"},
  {2, 2, 2022, "MONG 2 TET"},
  {3, 2, 2022, "MONG 3 TET"},
  {4, 2, 2022, "NGHI TET"},
  {5, 2, 2022, "NGHI TET"},
  {6, 2, 2022, "NGHI TET"},
  // NGHI GIO TO HV
  {10, 4, 2022, "NGAY GIO TO"},
  // NGHI LE GIAI PHONG MN & QUOC TET LAO DONG
  {2, 5, 2022, "NGHI BU 30/4"},
  {3, 5, 2022, "NGHI BU 1/5"}, 
  // NGAY QUOC KHANH
  {1, 9, 2022, "NGHI LE"},
  {2, 9, 2022, "QUOC KHANH"},
};
// Mang is hour date off - Gio ngay chi dinh nghi - gio ngay le. localNoteArray = 3
char *time_date_off[][5] = {
  {0, 0, 6, "BT SANG", 1},
  {0, 0, 14, "BT CHIEU", 1},
  {0, 30, 6, "AN SANG", 2},
  {0, 30, 10, "AN TRUA", 2},
  {0, 30, 17, "AN CHIEU", 2},
  {0, 45, 20, "DIEM DANH", 5},
  //TEST
  // {20, 30, 20, "PHUONG ANH 7", 7},
};

// Mang chua ngay toi da trong nam mac dinh: 1 --> 12. Rieng thang 2 can phai kiem tra nam nhuan: nhuan=29, ko nhuan=28
int *date_of_mouth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Functions
bool what_is_default_days(int today);
int is_a_working_date(int date, int mouth, int year);
int is_broadcast_time_working_date(int seconds, int minute, int hour);
int is_a_date_off(int date, int mouth, int year);
int is_broadcast_time_date_off(int seconds, int minute, int hour);
void displayTime(int seconds, int minutes, int hours, int dayofweek, int dayofmonth, int month, int year, int note, int localNoteArray);
bool is_tomorrow_off(int dayofweek, int dayofmonth, int month, int year);
// end functions

// CONST INDEX
size_t size_working_date = sizeof(working_date)/sizeof(working_date[0]);
size_t size_time_working_date = sizeof(time_working_date)/sizeof(time_working_date[0]);
size_t size_date_off = sizeof(date_off)/sizeof(date_off[0]);
size_t size_time_date_off = sizeof(time_date_off)/sizeof(time_date_off[0]);
// end

// VI TRI CAC BAN
// const int getup_song    = 1; // NHAC BAO THUC
// const int eat_song      = 2; // NHAC AN COM
// const int working_song  = 3; // NHAC LAM VIEC
// const int end_work_song = 4; // NHAC HET LAM VIEC
// const int check_song    = 5; // NHAC DIEM DANH
const int sleep_song    = 6; // NHAC DI NGU

void setup() {
  mySoftwareSerial.begin(9600);
  // Serial.begin(115200);
  delay(3000);
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    while(true);
  }
  // Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  delay(500);
  
  //----Set volume----
  myDFPlayer.volume(25);  //Set volume value (0~30).
  delay(500);
//  myDFPlayer.volumeUp(); //Volume Up
//  myDFPlayer.volumeDown(); //Volume Down
  
  //----Set different EQ----
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  delay(500);
  
  //----Set device we use SD as default----
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_U_DISK);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  delay(500); 
    
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    // Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
  // Clear the buffer
  display.clearDisplay();
  // Dat thoi gian ban dau
  // NAP TRUOC 24s so voi thoi gian thuc
  // myRTC.setDS1302Time(44, 57, 11, 7, 1, 5, 2021);
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

  // Phat nhac khong
  if(indexPlay==1){
    int songLocal1 = time_working_date[note][4];
    myDFPlayer.play(songLocal1);
    delay(800);
  }
  if(indexPlay==3){
    int songLocal2 = time_date_off[note][4];
    myDFPlayer.play(songLocal2);
    delay(800);
  }

  // Kiem tra gio di ngu. Kiem tra 2 thoi diem trong ngay. 21:30:0 & 22:0:0
  // Di ngu neu mai la ngay LAM VIEC
  if(seconds == 0 && minutes == 30 && hours== 21){
    bool is_sleep1 = is_tomorrow_off(dayofweek, dayofmonth, month, year);
    if(!is_sleep1){
      myDFPlayer.play(sleep_song);
      delay(800);
    }
  }
  // Di ngu neu mai la ngay NGHI
  if(seconds == 0 && minutes == 0 && hours== 22){
    bool is_sleep2 = is_tomorrow_off(dayofweek, dayofmonth, month, year);
    if(is_sleep2){
      myDFPlayer.play(sleep_song);
      delay(800);
    }
  }

}

bool what_is_default_days(int today){
  switch (today)
  {
  case 2: case 3: case 4: case 5: case 6:
    return true;
  case 1: case 7: case 8:
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
  delay(100);
}

// Ngay mai co phai la ngay nghi? NGHI = TRUE | LAM = FALSE
bool is_tomorrow_off(int dayofweek, int dayofmonth, int month, int year){
  // Cap nhat ngay Thang 2
  if (year % 4){
    date_of_mouth[1] = 28;
  }
  else{
    date_of_mouth[1] = 29;
  }

  // Tim ngay mai
  int new_date = dayofmonth + 1;
  int new_month = month;
  int new_year = year;
  // Neu ngay mai qua thang nay roi
  if(new_date > date_of_mouth[month-1]){
    new_date = 1;
    new_month++;
    // Van trong nam nay
    new_year = new_month <= 12 ? new_year : new_year+1;      
  }

  // Neu la 2-6
  if(what_is_default_days(dayofweek+1)){
    return is_a_date_off(new_date, new_month, new_year) != -1 ? true : false;
  }
  // Neu 7, CN
  else{
    return is_a_working_date(new_date, new_month, new_year) != -1 ? false : true;
  }
}
