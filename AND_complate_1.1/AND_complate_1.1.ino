  /**************************************************************************
 This is an application in plance AUTOMATIC BROADCAST DEVICE

 Pick four main modules:
 1. Arduino nano v3.0
 2. RTC DS3231
 3. DFPlayer mini

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

#include <string.h>
#include <Arduino.h>
#include "RTClib.h"
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Khoi tao danh cho DFPLAYER MINI
SoftwareSerial mySoftwareSerial(2, 3); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// Khoi tao DS3231
RTC_DS3231 rtc;

char *day_s[] = {"CHU NHAT", "THU 2", "THU 3", "THU 4", "THU 5", "THU 6", "THU 7"};

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
  {0, 0, 18, "AN CHIEU", 2},
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
  {0, 45, 20, "DIEM DANH", 5}
};

// Mang chua ngay toi da trong nam mac dinh: 1 --> 12. Rieng thang 2 can phai kiem tra nam nhuan: nhuan=29, ko nhuan=28
int *date_of_mouth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Functions
void printDetail(uint8_t type, int value);
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
const int working_song  = 3; // NHAC LAM VIEC
// const int end_work_song = 4; // NHAC HET LAM VIEC
// const int check_song    = 5; // NHAC DIEM DANH
const int sleep_song    = 6; // NHAC DI NGU

void setup() {
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif

  // RTC INIT
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    // Chu y: Bi cham lai 10s khi nap voi phuong phap nay
//   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // END RTC
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  delay(3000);

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  
  //----Set volume----//
  myDFPlayer.volume(25);  //Set volume value (0~30).
}

void loop() {
  delay(200);
  DateTime now = rtc.now();
  int seconds = now.second();
  int minutes = now.minute();
  int hours = now.hour();
  int dayofmonth = now.day();
  int dayofweek = now.dayOfTheWeek();
  int month = now.month();
  int year = now.year();
  int localNoteArray=0; // 0 | 1 | 2 | 3. default = 0 - Chi so cua mang thong bao
  int note=0;           // int. default = 0 - in ra thong bao so 0
  int indexPlay=-1;     // int. default = -1 - khong choi ban nhac nao. Chi so localNoteArray cua mang chua ma ban nhac
  
  // Xem ngay
  bool is_days = what_is_default_days(dayofweek);
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

  // Di ngu neu mai la ngay NGHI THI NGHI LA, VIEC TOI
  if(seconds == 0 && minutes == 0 && hours== 19){
    bool is_working_tonight = is_tomorrow_off(dayofweek, dayofmonth, month, year);
    if(!is_working_tonight){
      myDFPlayer.play(working_song);
      delay(800);
    }
  }

  // Kiem tra gio di ngu. Kiem tra 2 thoi diem trong ngay. 21:30:0 & 22:0:0
  // Di ngu neu mai la ngay LAM VIEC
  if(seconds == 0 && minutes == 50 && hours== 21){
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

  // SHow thoi gian
  displayTime(seconds, minutes, hours, dayofweek, dayofmonth, month, year, note, localNoteArray);

  // SHOW tinh trang loi
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}

bool what_is_default_days(int today){
  /* doan code nay co la do chuong trinh CHUNG TOI LA CHIEN SY thay doi gio phat song toi 11h ngay T7 moi nghi roi moi duoc an com*/
  if(today==7){
    // 11:00:00
    time_date_off[3][1]=0;
    time_date_off[3][2]=11;
  }
  else
  {
    // 10:30:00
    time_date_off[3][1]=30;
    time_date_off[3][2]=10;
  }
  
  /* Het code ep buoc !!!*/
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
  Serial.print(day_s[dayofweek]);
  Serial.print(" ngày ");
  Serial.print(dayofmonth);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/");
  Serial.print(year);
  Serial.print(" ");
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print(":");
  Serial.print(seconds);

  Serial.println("Vi tri che de trong mang: ");
  Serial.print(note);
  Serial.println("Chi so cua mang phat ra: ");
  Serial.print(localNoteArray);
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

// PRINT ERROR
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  
}
