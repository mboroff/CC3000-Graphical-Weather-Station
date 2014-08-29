/**********************************************
            get_touch
***********************************************/

void get_touch() 
{

// clear the coordinates
tsx = 0;
tsy = 0;
tsz = 0;
px = 0;
py = 0;
pz = 0;

 // while (pz > 0)
  if (ts.touched()) {
   
    // read px & py & pz;
    while (! ts.bufferEmpty()) {
      Serial.print(ts.bufferSize());
      ts.readData(&px, &py, &pz);
      while(ts.bufferSize()>0){
       ts.readData(&px, &py, &pz);
        //           ts.getPoint();  
             }
/*
      Serial.print(F("->(")); 
      Serial.print(px); Serial.print(F(", ")); 
      Serial.print(py); Serial.print(F(", ")); 
      Serial.print(pz);
      Serial.print(F(")"));
*/
              delay(50);

    ts.writeRegister8(STMPE_INT_STA, 0xFF); // reset all ints
    }
  //   delay(400);
      if (pz > MINPRESSURE) {
          px = map(px, TS_MINX, TS_MAXX, 0, tft.width());
          py = map(py, TS_MINY, TS_MAXY, 0, tft.height());
 /*
          Serial.print(F("->(")); 
          Serial.print(px); Serial.print(F(", ")); 
          Serial.print(py); Serial.print(F(", ")); 
          Serial.print(pz);
          Serial.println(F(")"));
*/
          if (pz > tsz) {
             tsx = px;   // get max
             tsy = py;
             tsz = pz;
          }
           while(ts.bufferSize()>0){
            ts.getPoint();  
             }
           px = 0;
           py = 0;
           pz = 0;
/*
  tft.textMode();
  tft.textEnlarge(0);
  tft.textTransparent(RA8875_WHITE);
  tft.textColor(RA8875_WHITE, RA8875_BLUE);
  texTx = 0;
  texTy = 460;
  tft.textSetCursor(texTx, texTy);
  tft.print(F("ts values ->(")); 
  tft.print(tsx); tft.print(F(", ")); 
  tft.print(tsy); tft.print(F(", ")); 
  tft.print(tsz);
  tft.print(F(")"));

          Serial.print(F("ts values ->(")); 
          Serial.print(tsx); Serial.print(F(", ")); 
          Serial.print(tsy); Serial.print(F(", ")); 
          Serial.print(tsz);
          Serial.println(F(")"));
*/
      }
  }  
}
