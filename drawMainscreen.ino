void drawMainscreen() {   
  tft.graphicsMode();
  tft.fillScreen(RA8875_BLACK);   
  tft.fillRect(0, 0, 800, 60, WHITE);
  tft.fillRoundRect(5, 70, 200, 400, 10, BLUE);
  tft.drawCircle(160, 143, 5, BLACK);
  tft.fillRect(15, 270, 170, 5, WHITE);
  tft.fillRoundRect(600, 70, 200, 400, 10, YELLOW);
  tft.drawCircle(760, 143, 5, BLACK);
  tft.fillRoundRect(220, 70, 365, 220, 10, CYAN);
  tft.fillRoundRect(220, 270, 365, 200, 10, GREEN);
 // tft.fillRoundRect(280, 415, 260, 50, 10, BLACK);  
  tft.fillRect(615, 270, 170, 5, WHITE);
//  bmpDraw("suny64.bmp", 330, 90);
 // outdoor temp
 // char c = '7';
 // tft.drawChar(30, 140, c, BLACK, BLUE, 10);  
 // c = '8';
 // tft.drawChar(90, 140, c, BLACK, BLUE, 10);  
// outdoor humidity
//  c = '3';
//  tft.drawChar(30, 320, c, BLACK, BLUE, 10);  
//  c = '9';
//  tft.drawChar(90, 320, c, BLACK, BLUE, 10);  

 // indoor temp
 char c = '7';
  tft.drawChar(630, 140, c, BLACK, YELLOW, 10);  
  c = '8';
  tft.drawChar(690, 140, c, BLACK, YELLOW, 10);  
// indoor humidity
  c = '3';
  tft.drawChar(630, 320, c, BLACK, YELLOW, 10);  
  c = '9';
  tft.drawChar(690, 320, c, BLACK, YELLOW, 10);  

  tft.textMode();
  tft.textEnlarge(1);  
  tft.textColor(BLACK, BLUE);
  tft.textSetCursor(35, 85);
  tft.print(F("OUTDOOR")); 
  tft.textSetCursor(15, 220);
  tft.print(F("Temperature")); 
  tft.textSetCursor(170, 130);
  tft.print(F("F")); 
  tft.textSetCursor(150, 310);
  tft.print(F("%"));
  tft.textSetCursor(35, 420);
  tft.print(F("Humidity"));
  tft.textSetCursor(260, 320);
  tft.textColor(BLACK, GREEN);
//  tft.print("Wind string 4567890");
   tft.textSetCursor(360, 420);
  tft.textColor(BLACK, GREEN);   
  tft.print(F("Wind"));
  tft.textSetCursor(640, 85);
  tft.textColor(RED, YELLOW);   
  tft.print(F("INDOOR"));
  tft.textSetCursor(610, 220);
  tft.textColor(BLACK, YELLOW);   
  tft.print(F("Temperature"));
  tft.textSetCursor(770, 130);
  tft.print(F("F")); 
  tft.textSetCursor(755, 305);
  tft.print(F("%"));
  tft.textSetCursor(640, 420);
  tft.print(F("Humidity"));
}
