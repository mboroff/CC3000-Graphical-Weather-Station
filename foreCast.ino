void foreCast() {
 titleCtr = 0;
 fahrenheitCtr = 0;
 fcttextCtr = 0;
 tft.graphicsMode();
 tft.fillScreen(BLUE);   
 tft.fillRect(0, 0, 800, 60, WHITE);
 tft.textMode();
 tft.textEnlarge(1);  
 tft.textColor(BLACK, WHITE);
 tft.textSetCursor(35, 10);
 tft.print(observationtime);
 tft.textSetCursor(310, 10);
 tft.print("FORECAST ");
 tft.textColor(WHITE, BLUE);
 tft.textSetCursor(0, 90);
 tft.print(title1); 
 tft.print(" "); 
 tft.print(fahrenheit0);
 tft.print("/"); 
 tft.print(fahrenheit1); 
 tft.print(" "); 
 tft.print(conditions1);
 tft.textSetCursor(0, 125);
 tft.textEnlarge(0);  
 tft.print(fcttext1);
 tft.textEnlarge(1);   
 tft.textSetCursor(0, 180);
 tft.print(title3); 
 tft.print(" "); 
 tft.print(fahrenheit2); 
 tft.print("/"); 
 tft.print(fahrenheit3); 
 tft.print(" "); 
 tft.print(conditions2);
 tft.textSetCursor(0, 215);
 tft.textEnlarge(0);  
 tft.print(fcttext2);
 tft.textEnlarge(1);   
 tft.textSetCursor(0, 270);
 tft.print(title5);  
 tft.print(" "); 
 tft.print(fahrenheit4); 
 tft.print("/"); 
 tft.print(fahrenheit5); 
 tft.print(" "); 
 tft.print(conditions3);
 tft.textSetCursor(0, 305);
 tft.textEnlarge(0);  
 tft.print(fcttext3);
 tft.textEnlarge(1);   
 tft.textSetCursor(0, 360);
 tft.print(title7); 
 tft.print(" "); 
 tft.print(fahrenheit6); 
 tft.print("/"); 
 tft.print(fahrenheit7); 
 tft.print(" "); 
 tft.print(conditions4);
 tft.textSetCursor(0, 395);
 tft.textEnlarge(0);  
 tft.print(fcttext4);

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
