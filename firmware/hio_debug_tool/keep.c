/***********************************************/
/*
void bitmap_test(void){
   //DRAW AN IMAGE FROM AVR'S RAM 
   //IMAGE WILL BE UPSIDE DOWN (TOP LEFT ORIGIN)  

  const uint16_t imagedata[] = {   
      0x42CD, 0x42CD, 0x3A8C, 0x324C, 0x324B, 0x3A8C, 0x42ED, 0x4B0E, 0x42ED, 0x42ED,
      0x4B0E, 0x4B2E, 0x42CD, 0x42AC, 0x42CD, 0x3AAC, 0x3AAC, 0x3AAC, 0x3AAC, 0x3AAC,
      0x4AED, 0x4B0D, 0x4AED, 0x42CD, 0x42CD, 0x3ACD, 0x42CD, 0x42CD, 0x4A4B, 0x5907,
      0x4ACD, 0x4AED, 0x4B0E, 0x4B0D, 0x42ED, 0x42CD, 0x42CD, 0x42CD, 0x42CD, 0x4AED,
      0x3ACD, 0x3A8C, 0x3A8C, 0x3AAC, 0x42CD, 0x42CD, 0x42CD, 0x4AED, 0x42ED, 0x42ED,
      0x4B0D, 0x4ACC, 0x428B, 0x3A6B, 0x3A4A, 0x3A4A, 0x3A4A, 0x3A4A, 0x42AC, 0x326B,
      0x3A6C, 0x326C, 0x326B, 0x324B, 0x2A09, 0x29C9, 0x21A8, 0x29C8, 0x2167, 0x1946,
      0x1945, 0x10C4, 0x2146, 0x2145, 0x2145, 0x2145, 0x2166, 0x2146, 0x42ED, 0x42ED,
      0x42ED, 0x4B0E, 0x4B0D, 0x42ED, 0x4B0E, 0x4B0E, 0x532E, 0x534F, 0x4B0E, 0x4B0D,
      0x3AAD, 0x3AAD, 0x42CD, 0x42ED, 0x3ACD, 0x3A8C, 0x3A8C, 0x3AAD, 0x430D, 0x4B0D
   };
   
   const uint16_t *img = &imagedata;
   
   ST7735_DrawBitmap(20, 20, img, 10, 10);
}
*/

/***********************************************/
/*
void blit_test(void){
   //COPY SRAM DATA FROM FROM AVR'S RAM , THEN DRAW FROM SRAM 
   //THIS IS THE FIRST HALF OF TRANSMIT BMP FUNCTION OVER UART
   //IMAGE WILL BE UPSIDE DOWN (TOP LEFT ORIGIN)  
   const uint16_t imagedata[] = {   
                              0x00aa,0x00ff,0x00aa,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ff,0x00ea 
                             ,0x00ff,0x00ff,0x00ff,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ff,0x00ea 
                             ,0x00ff,0xffff,0x00ff,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ff,0x00ea 
                             ,0x00ff,0x00ff,0x00ff,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ea,0x00ff 
                             ,0x00ff,0x00ff,0xffff,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ea,0x00ff 
                             ,0x00ff,0x00ff,0x00ff,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ea,0x00ff 
                             ,0xffff,0x00ff,0x00ff,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ff,0x00ea 
                             ,0x00ff,0xffff,0x00ff,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ff,0x00ea 
                             ,0x00ff,0x00ff,0xffff,0xffff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ff,0x00ea 
                             ,0x00aa,0x00ff,0x00ff,0x00aa,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ff,0x00ea 
                          };
   const uint16_t *img = &imagedata;
   
   blit_SRAM( 0, 0, 0, img, 10, 10);
   ST7735_DrawBitmapSRAM(10, 10, 0, 10, 10);
}
*/

/***********************************************/
/*
void ST7735_DrawBitmapSRAM(uint8_t x, uint8_t y, uint16_t start, uint8_t w, uint8_t h){  
  int8_t skipC = 0;                      // non-zero if columns need to be skipped due to clipping
  int8_t originalWidth = w;              // save this value; even if not all columns fit on the screen, the image is still this width in ROM
  uint8_t i = w*(h - 1);

  uint8_t _height = 160;
  uint8_t _width  = 128;


  if((x >= _width) || ((y - h + 1) >= _height) || ((x + w) <= 0) || (y < 0)){
    return;                             // image is totally off the screen, do nothing
  }
  if((w > _width) || (h > _height)){    // image is too wide for the screen, do nothing
    //***This isn't necessarily a fatal error, but it makes the
    //following logic much more complicated, since you can have
    //an image that exceeds multiple boundaries and needs to be
    //clipped on more than one side.
    return;
  }
  if((x + w - 1) >= _width){            // image exceeds right of screen
    skipC = (x + w) - _width;           // skip cut off columns
    w = _width - x;
  }
  if((y - h + 1) < 0){                  // image exceeds top of screen
    i = i - (h - y - 1)*originalWidth;  // skip the last cut off rows
    h = y + 1;
  }
  if(x < 0){                            // image exceeds left of screen
    w = w + x;
    skipC = -1*x;                       // skip cut off columns
    i = i - x;                          // skip the first cut off columns
    x = 0;
  }
  if(y >= _height){                     // image exceeds bottom of screen
    h = h - (y - _height + 1);
    y = _height - 1;
  }

  setAddrWindow(x, y-h+1, x+w-1, y);
  
  uint16_t pixel = 0;
  uint16_t idx    = start;

  for(y=0; y<h; y=y+1){
    for(x=0; x<w; x=x+1){

        pixel = read_ram(idx);
        writedata( pixel ); //msb
        writedata( pixel ); //lsb
        idx++;

        i = i + 1;// go to the next pixel
    }
    i = i + skipC;
    i = i - 2*originalWidth;
  }
}
*/