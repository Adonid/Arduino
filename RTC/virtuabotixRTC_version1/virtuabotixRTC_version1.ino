//=======================================================================================================//|
//                                                                                                       //|
//                                        Program Description Begin                                      //|
//                                                                                                       //|
//=======================================================================================================//|
//                                                                                                       //|
//  This example executes a print out example that accesses individual data elements.                    //|
//                                                                                                       //|
//=======================================================================================================//|
//                                                                                                       //|
//                                        Program Description End                                        //|
//                                                                                                       //|
//=======================================================================================================//|


//=======================================================================================================//|
//                                                                                                       //|
//                                 This is the Revision Section Begin                                    //|
//                                                                                                       //|
//=======================================================================================================//|
//                                                                                                       //|
//    Date                  Programmer                  Revisions Made                                   //|
//                                                                                                       //|
//    XX June 2012          Krodal                      Original                                         //|
//    XX March 2013         Krodal                      Added bcd2bin, bin2bcd_h, and bin2bcd_l functions//|
//    07 January 2014       Joseph Datillo              Converted file into library format.              //|
//                          T. Daniel Murphy            Added functions to allow for user implentation   //|
//                                                        as a library, vice a stand alone program.      //|
//                                                        This also adds the functions required to access//|
//                                                        individual data elements, and updating time.   //|
//                                                                                                       //|
//=======================================================================================================//|
//                                                                                                       //|
//                                        Revision Section End                                           //|
//                                                                                                       //|
//=======================================================================================================//|


//=======================================================================================================//|
//                                                                                                       //|
//                                        Acknowledgements Begin                                         //|
//                                                                                                       //|
//=======================================================================================================//|
//                                                                                                       //|
//  The original program was provided by Arduino.cc user Kodal on URL:                                   //|
//  http://playground.arduino.cc/Main/DS1302                                                             //|
//                                                                                                       //|
//=======================================================================================================//|
//                                                                                                       //|
//                                          Acknowledgements End                                         //|
//                                                                                                       //|
//=======================================================================================================//|


//=======================================================================================================//|
//                                                                                                       //|
//                                            Reference Links Begin                                      //|
//                                                                                                       //|
//=======================================================================================================//|
                                                                                                         //|
//  Datasheet:  http://www.hobbytronics.co.uk/datasheets/DS1302.pdf                                      //|
//  GET UPDATES: https://www.virtuabotix.com/resources.php?product=versalino                             //|
                                                                                                         //|
//=======================================================================================================//|
//                                                                                                       //|
//                                            Reference Links End                                        //|
//                                                                                                       //|
//=======================================================================================================//|


//=======================================================================================================//|
//                                                                                                       //|
//                          Include files required for Program Function Begin                            //|
//                                                                                                       //|
//=======================================================================================================//|
                                                                                                         //|
#include <virtuabotixRTC.h>                                                                              //|
                                                                                                         //|
//=======================================================================================================//|
//                                                                                                       //|
//                          Include files required for Program Function End                              //|
//                                                                                                       //|
//=======================================================================================================//|

//virtuabotixRTC(uint8_t inSCLK, uint8_t inIO, uint8_t inC_E); 
// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(6, 5, 4);
char *day_s[] = {"", "Chủ nhật", "Thứ 2", "Thứ 3", "Thứ 4", "Thứ 5", "Thứ 6", "Thứ 7"};
void setup()  {      
  Serial.begin(9600);

// Set the current date, and time in the following format:
// seconds, minutes, hours, day of the week, day of the month, month, year
  myRTC.setDS1302Time(28, 59, 9, 7, 10, 4, 2021);
}

//=======================================================================================================//|    
//                                                                                                       //|   
//                         Printout by accessing Single Element objects BEGIN                            //|   
//                                                                                                       //|   
//=======================================================================================================//|   
//                                                                                                       //|   
//  This example utilizes the Serial.print function to access individual data elements, this allows for  //|
//  user defined output format.                                                                          //|
//                                                                                                       //|   
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//|  
void loop()  {                                                                                           //| 
// This allows for the update of variables for time or accessing the individual elements.                //|
  myRTC.updateTime();                                                                                    //|                                                                                                        //| 
// Start printing elements as individuals                                                                //|   
  Serial.print("Current Date / Time: ");
  Serial.print(day_s[myRTC.dayofweek]);
  Serial.print(", ngày ");                                                                                     //| 
  Serial.print(myRTC.dayofmonth);
  Serial.print(", tháng ");                                                                                     //| 
  Serial.print(myRTC.month);                                                                             //| 
  Serial.print(", năm ");                                                                                     //| 
  Serial.print(myRTC.year);                                                                              //| 
  Serial.print("| ");                                                                                    //| 
  Serial.print(myRTC.hours);                                                                             //| 
  Serial.print(":");                                                                                     //| 
  Serial.print(myRTC.minutes);                                                                           //| 
  Serial.print(":");                                                                                     //| 
  Serial.println(myRTC.seconds);                                                                         //| 
                                                                                                         //| 
// Delay so the program doesn't print non-stop                                                           //| 
  delay(1000);                                                                                          //| 
}                                                                                                        //| 
                                                                                                         //|    
//=======================================================================================================//|    
//                                                                                                       //|    
//                                   Printout using BUFFER objects BEGIN                                 //|    
//                                                                                                       //|    
//=======================================================================================================//|    


  
