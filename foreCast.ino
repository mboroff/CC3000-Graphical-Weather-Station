void foreCast() {
 titleCtr = 0;
 fahrenheitCtr = 0;
 fcttextCtr = 0;
 tft.graphicsMode();
 tft.fillScreen(BLUE);   
 tft.fillRect(0, 0, 800, 60, WHITE);
 tft.textMode();
 tft.textEnlarge(1);  
 tft.textColor(RED, WHITE);
 tft.textSetCursor(300, 10);
 tft.print("Getting INFO");
 tft.textColor(WHITE, BLUE);
 internetCtr++;  
  /* Try connecting to the website.
     Note: HTTP/1.1 protocol is used to keep the server from closing the connection before all data is read.
  */
  Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 80);
  if (www.connected()) {
    www.fastrprint(F("GET "));
    www.fastrprint(WEBPAGE3);
    www.fastrprint(F(" HTTP/1.1\r\n"));
    www.fastrprint(F("Host: ")); www.fastrprint(WEBSITE); www.fastrprint(F("\r\n"));
    www.fastrprint(F("\r\n"));
    www.println();
  } else {
    Serial.println(F("Connection failed"));    
                          Serial.flush();
                          www.close();
//                          cc3000.disconnect();    
                          cc3000.stop();
//                          startConnection();
                          void(* resetFunc) (void) = 0; //declare reset function @ address 0
                          resetFunc();  //call reset
                          Serial.println("RESET!");
                          digitalWrite(7, LOW); //Pulling the RESET pin LOW triggers the reset.

                          return;
                          }
  Serial.println();

  Serial.println(F("-------------------------------------"));
  /* Read data until either the connection is closed, or the idle timeout is reached. */ 
  unsigned long lastRead = millis();
 while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    while (www.available()) {
      char c = www.read();
      lastRead = millis();
      Serial.print(c);
      if (c == '"') {
          if (startQuote == false) {
              startQuote = true;
              buff = "";
          }
        else  {
                   if (startQuote == true) {
                       startQuote = false;

                     if (lookingForfahrenheit == true) {
                         lookingForfahrenheit = false; 
                         tft.textEnlarge(1);  
                         if (fahrenheitCtr == 1) {
                             tft.textSetCursor(200, 90);
                             tft.textColor(RED, BLUE);
                             tft.print(buff);
                             tft.textColor(WHITE, BLUE);
                             tft.print("/");                             
                           }                           
                         if (fahrenheitCtr == 2) {
                             tft.textSetCursor(250, 90);
                             tft.print(buff);         
                           }
                          
                         if (fahrenheitCtr == 3) {
                             tft.textSetCursor(200, 180);
                             tft.textColor(RED, BLUE);
                             tft.print(buff);
                             tft.textColor(WHITE, BLUE);
                             tft.print("/");
                           }
                         if (fahrenheitCtr == 4) {
                             tft.textSetCursor(250, 180);
                             tft.print(buff);
                           }
                         if (fahrenheitCtr == 5) {
                             tft.textSetCursor(200, 270);
                             tft.textColor(RED, BLUE);
                             tft.print(buff);
                             tft.textColor(WHITE, BLUE);
                              tft.print("/");
                           }
                         if (fahrenheitCtr == 6) {
                             tft.textSetCursor(250, 270);
                             tft.print(buff);
                           }
                         if (fahrenheitCtr == 7) {
                             tft.textSetCursor(200, 360);
                             tft.textColor(RED, BLUE);
                             tft.print(buff);
                             tft.textColor(WHITE, BLUE);
                             tft.print("/");
                           }
                         if (fahrenheitCtr == 8) {
                             tft.textSetCursor(250, 360);
                             tft.print(buff);
                           }
                           
  
//                         Serial.println(buff);
//                         Serial.print("buff length = "); Serial.println(l);
                         buff = "";                         
                         }
                     else
                     if (buff == String("fahrenheit")){
                         lookingForfahrenheit = true;
                          fahrenheitCtr++;
//                         Serial.print(buff);    Serial.print(F(" "));
                         buff = "";
                         }
                    else 
                        if (lookingForfcttext == true) {
                         lookingForfcttext = false; 
                         tft.textEnlarge(0);  
                         if (fcttextCtr == 0) {
                             tft.textSetCursor(0, 120);
                             tft.print(buff);
                         }
                         if (fcttextCtr == 2) {
                             tft.textSetCursor(0, 210);
                             tft.print(buff);
                         }
                         if (fcttextCtr == 4) {
                             tft.textSetCursor(0, 300);
                             tft.print(buff);
                         }
                         if (fcttextCtr == 6) {
                             tft.textSetCursor(0, 390);
                             tft.print(buff);
                         }
                         
//                         Serial.println(fcttext[fcttextCtr]);
//                         Serial.print("buff length = "); Serial.println(l);
                         fcttextCtr++;
                         buff = "";
                         }
                     else
                     if (buff == String("fcttext")){
                         lookingForfcttext = true;
 //                        Serial.print(buff);    Serial.print(F(" "));
                         buff = "";
                         }
                    else 

                               if (lookingFortitle == true) {
                                   lookingFortitle = false; 
                                   tft.textEnlarge(1);  
                                   if (titleCtr == 1) {
                                       tft.textSetCursor(0, 90);
                                       tft.print(buff);
                                       }
                                   if (titleCtr == 3) {
                                       tft.textSetCursor(0, 180);
                                       tft.print(buff);
                                       }
                                   if (titleCtr == 5) {
                                       tft.textSetCursor(0, 270);
                                       tft.print(buff);
                                       }
                                   if (titleCtr == 7) {
                                       tft.textSetCursor(0, 360);
                                       tft.print(buff);
                                       }
                                   buff = "";
                                   }
                               else
                               if (buff == String("title")){
                                   lookingFortitle = true;
                                   titleCtr++;
//          Serial.print(buff);    Serial.print(F(" "));
                                   buff = "";
                                   }
                   }
        }
      } else
       if (startQuote == true) buff += c;
  }   
 }
  Serial.println();

  www.close();
  buff = "";
  delay(1000);
 // Serial.println(F("-------------------------------------"));
  
  /* You need to make sure to clean up after yourself or the CC3000 can freak out */
  /* the next time your try to connect ... */
 // Serial.println(F("\n\nDisconnecting"));
//  tft.setCursor(texTx, texTy);
//  tft.print("Disconnecting");
//  texTy = texTy + 10;
 // cc3000.disconnect();
 // cc3000.stop();
   tft.textEnlarge(1);  
  tft.textColor(BLACK, WHITE);
  tft.textSetCursor(300, 10);
  tft.print("FORECAST    ");

int free = 0;
    while (free == 0) {
          if (prevSec != second()) {          // of it is a different second than the previous do the clock thing
          prevSec = second();
          tft.textMode();
          tft.textEnlarge(1);
          tft.textColor(BLACK, WHITE);
        /* time of day */
         tft.textSetCursor(550, 10);
         int tempHr;
         if (hour() > 12) {
             tempHr = hour() - 12;
             }
          else {
                tempHr = hour();
                }
         if (hour() == 0) {
             tempHr = 12;
             }    
         if (tempHr < 10) {
             tft.print(F("0"));
             }
         tft.print(tempHr);
         tft.print(F(":"));
         if (minute() < 10) {
            tft.print(F("0"));
            }
         tft.print(minute());
         tft.print(F(":"));
         if (second() < 10) {
             tft.print(F("0"));
             }
         tft.print(second());
         if (hour() > 11) {
             tft.print(F(" P.M. "));
             }
         else {
               tft.print(F(" A.M. "));
               }
        /* alarm time */
        if (alarmSet == true) {
            tft.graphicsMode();
            tft.fillCircle(755, 20, 5, RED);
            tft.textMode();
            tft.textColor(RA8875_WHITE, RA8875_BLUE);     
            }
          }
      if (minute() ==  0 || minute() == 10 || minute() == 20 || 
          minute() == 30 || minute() == 40 || minute() == 50 ) {
           free = 1;  //  trigger return on next while compare
          } 
      get_touch();
      if (tsz > 0) {
          free = 1;
           }
    }
  tsz = 0;
  
}
