
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "kernel_splatr.h"





uint8_t buf1_lsb;
uint8_t buf1_msb;

uint8_t buf2_lsb = 0;
uint8_t buf2_msb = 0;

uint16_t buf1_16 = 0;
uint16_t buf2_16 = 0;

uint16_t sprite_xy = 0; //packed value two bytes 

uint8_t cursor_x = 0;
uint8_t cursor_y = 0;


//------------------------------------
//these control widget positions   
volatile uint8_t fontsize      = 1;  

volatile uint8_t byte_width    = 100;
volatile uint8_t step_size_x   = 100/8; //byte_width/8;

volatile uint8_t byte_height   = 10; 
volatile uint8_t step_size_y   = 10;   //same as byte height

volatile uint8_t bb_begin      = 0;    //step_size_x - X coordinate for byte box 
volatile uint8_t hex_begin     = 100;  //X coordinate for hex text 

/*******************************/
//these control coloring  

volatile uint16_t CRSR_COLOR = 0x0ff0; 
 

//------------------------------------
//these are the bytes that are rendered 
volatile uint8_t byte1 = 0x0f;
volatile uint8_t byte2 = 0x0f;
volatile uint8_t byte3 = 0x0f;

volatile uint8_t byte4 = 0xff;
volatile uint8_t byte5 = 0xff;
volatile uint8_t byte6 = 0xff;



//nifty tool to see data on a bus 

void debug_parport(uint8_t passbyte)
{
    for(uint8_t x=0;x<255;x++)
    {
        //draw_byte_box(passbyte  , bb_begin ,        0,  byte_width , byte_height , bit_off_color);
        draw_byte_box(x, 
                      bb_begin, 0,  
                      byte_width, byte_height, 
                      bit_off_color,
                      or_off_color);
        render_8_hex(hex_begin  , 0        , fontsize   ,  x      , CRSR_COLOR);
        _delay_ms(100);
    }

}


/*******************************/

void draw_outline_box(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color){
   ST7735_LineRect(x  , y   ,w   , h  , grid_color);
   ST7735_FillRect(x+1, y+1 ,w-1 , h-1, color);
}

/*******************************/
void draw_byte_box(uint8_t byte, 
                   uint8_t x, uint8_t y, 
                   uint8_t w, uint8_t h, 
                   uint16_t oncolor,
                   uint16_t offcolor)
{
  /*
      draw a "byte" onscreen, represented as 8 squares with bits colored on or off 
      ARGS:
           byte     - buffer to hold the byte to be drawn 
           x,y      - corner to start drawing at, from top left 
           size     - scale factor for box (not much room on this small screen )
           color    - see adafruit lib for more info on colors 
  */  
    uint8_t num = 8;
    uint8_t a = 0;
    uint8_t total_w = x+w;
    uint8_t total_h = y+h;
    uint8_t bitsize = total_w/num;

    for (a=0;a<num;a++){
       if ( !!(byte & (1 << (7 - a))) ){  // MSB
           draw_outline_box(x+(a*bitsize), y, w/num, h, bit_on_color);
       }else{
           draw_outline_box(x+(a*bitsize), y, w/num, h, offcolor);
       }
    }

}

/*******************************/
void render_8_hex(uint8_t x, uint8_t y, uint8_t size, uint8_t accumulator, uint16_t color){
  /*
      draw a "byte" as a hex value, represented as 8 squares with bits colored on or off 
      ARGS:
           x,y         - corner to start drawing at, from top left 
           size        - scale factor for box (not much room on this small screen )
           accumulator - buffer to hold the byte to be drawn 
           color       - see adafruit lib for more info on colors 
  */

    uint8_t i = 0;

    uint8_t char_spacing = 6*size; //digit size in pixels
    uint8_t max = (char_spacing*2);//2 digits
    ST7735_FillRect(x, y, max+1, char_spacing+1, 0x0);    //clear rectangle in x*y space
    uint8_t cnt = 1;
    for (i=0;i<2;i++)
    {
        uint8_t digit = (accumulator & (0xf << (i*4))) >> (i*4);//little endian
        ST7735_DrawHexCharS((x+max)-(cnt*char_spacing), y+1, digit, color, 0x0, size);
        cnt++;
    }   
}

/*******************************/

void render_16_hex(uint8_t x, uint8_t y, uint8_t size, uint16_t accumulator, uint16_t color){
    uint8_t i = 0;

    uint8_t char_spacing = 6*size; //digit size in pixels
    uint8_t max = (char_spacing*4);//4 digits
    ST7735_FillRect(x, y, char_spacing*4, char_spacing, 0x0);    //clear rectangle in x*y space

    for (i=0; i<4; i++)
    {
        // Mask off one nibble at a time, shift it down to the
        // bottom of the integer, and use that as to index the font.
        // uint8_t digit = (accumulator & (0xf << (i*4))) >> (i*4);//little endian
        uint8_t digit = (accumulator & (0xf << (i*4))) >> (i*4);//little endian

        //view (nibble as working) as a graphic 
        //draw_byte_box(digit, 0,   1, byte_width, 20);

        //send (nibble as working) over serial port as human readble hex
        //USART_Transmit(hex_Font[digit]);//needs ascii encoding 

        //view (nibble as working) screen as human readable hex
        ST7735_DrawHexCharS((x+max)-(char_spacing*i), y, digit, color, 0x0, size);
     }   
}


/*******************************/
/*
void draw_cursor(uint16_t color){
    ST7735_LineRect(x  , y   , byte_width/8    , byte_height   ,color);
    ST7735_LineRect(x+1, y+1 ,(byte_width/8)-2 , byte_height-2 ,color);   
    //ST7735_LineRect(x+2, y+2 ,(byte_width/8)-3 , byte_height-3 ,color);  
}
*/
/*******************************/
//put here mostly because I forgot name of command
void clear_screen(uint16_t color){
    ST7735_FillScreen(color); 
}



/*******************************/

void latch_outputs(void){
    PORTA = byte1;
    PORTC = byte2;
    //PORTC = byte4;    
}



/***********************************************/
//I never did figure out char mapping but it uses this:
//https://en.wikipedia.org/wiki/Code_page_437 
void scribe_str(char *data) 
{ 
   //uint8_t cursor_x = 0;
   //uint8_t cursor_y = 0;
  
   cursor_y+=10;
   if(cursor_y>=160){
       cursor_y=0;
       ST7735_FillScreen(1);
   }

   uint8_t ct = 0;
   while(*data){ 
      //ST7735_DrawCharS(8*ct, 10, *data++, 0x0ff0, 0x0000, 1);
      ST7735_DrawCharS(ct*7, cursor_y, *data++, 0x0ff0, 0x0000, 1);
      
      ct++; 
   }

}


/***********************************************/

/* COM buffer is for command args - Serial buffer is for stiching and tearing,
   helper to grab arguments for commands over serial port 
*/


/* 
uint16_t rx_two_bytes(void){
    uint8_t c1 =  USART_receive();//lsb addr
    uint8_t c2 =  USART_receive();//msb addr
    //send_txt_2bytes(buf1_16, 1, 0);
    scribe_str("2 bytes received.");
    return cvt_8x2_to_16(c1, c2); //fills buffer buf1_16

}
*/

/***********************************************/
/*
 //helper to grab arguments for commands over serial port 
 void rx_three_bytes(void){
    uint8_t c1 =  USART_receive();//lsb addr
    uint8_t c2 =  USART_receive();//msb addr 
    buf2_lsb =  USART_receive();//data byte 

    buf1_16 = cvt_8x2_to_16(c1, c2); //fills buffer buf1_16
    scribe_str("3 bytes received.");
 }
*/

/***********************************************/
/*
//helper to grab arguments for commands over serial port 
void rx_four_bytes(void){
    // uint8_t c1 =  UART_receive();//lsb addr
    // uint8_t c2 =  UART_receive();//msb addr 
    // uint8_t d1 =  UART_receive();//lsb addr
    // uint8_t d2 =  UART_receive();//msb addr 
    // cvt_8x2_to_16(d1, d2); //fills buffer buf1_16
    // buf2_16 = buf1_16;     //copy buffer 
    // cvt_8x2_to_16(c1, c2); //fills buffer buf1_16
    // send_txt_2bytes(buf1_16, 1, 0);
    // send_txt_2bytes(buf2_16, 1, 0);
    
    buf1_lsb =  USART_receive();//lsb addr
    buf1_msb =  USART_receive();//msb addr 
    buf2_lsb =  USART_receive();//lsb addr
    buf2_msb =  USART_receive();//msb addr 
    scribe_str("4 bytes received.");
}
*/

/***********************************************/
/*
//for printing internal 16 bit numbers - all serial related I/O is bytesX2
void send_txt_2bytes( uint16_t data, uint8_t use_newline,  uint8_t use_space){
   uint8_t i = 0;

   for (i=0; i<=15; i++) {
       //if (i==8){  USART_Transmit(0x20); }//middle space 

       if ( !!(data & (1 << (15 - i))) ){  // MSB
           USART_Transmit( BIT_OFF );
       }else{
           USART_Transmit( BIT_ON );
       }
    }
    
    if(use_space!=0){
        USART_Transmit(0x20);    //SPACE 
    }

    if(use_newline!=0){
        USART_Transmit( 0xa ); //CHAR_TERM = new line  
        USART_Transmit( 0xd ); //0xd = carriage return
    }
}
*/

/*******************************/
/*
void app_one_redraw(void){
    // ST7735_FillScreen(1);
    //////////////////////////////////
    //calculate values for boolean logic display   
    // uint8_t *pByte = 0;
    // pByte = &byte4;
    // or_bits(pByte, byte1, byte2);
    // pByte = &byte5;
    // and_bits(pByte, byte1, byte2);
    
    //////////////////////////////////

    draw_byte_box(byte1     , bb_begin        ,                0,  byte_width , byte_height, 
                  bit_on_color, bit_off_color);
    render_8_hex(hex_begin  , 0               , fontsize   ,  byte1      , CRSR_COLOR);
    //--------  
    draw_byte_box(byte2     , bb_begin        , byte_height , byte_width  , byte_height,
                  bit_on_color, bit_off_color);

    render_8_hex(hex_begin  , byte_height     , fontsize  , byte2       , CRSR_COLOR); 
    //--------  
    draw_byte_box(byte3     , bb_begin        , (byte_height*2) , byte_width  , byte_height, 
                  bit_on_color, bit_off_color);

    render_8_hex(hex_begin  , (byte_height*2) , fontsize  , byte3       , CRSR_COLOR); 

    //--------    
    //--------
    draw_cursor(CRSR_COLOR); //draw new cursor

    //show the memory address that we are navigating  
    //render_16_hex(0 , 100 , 2  , mem_addr    , CRSR_COLOR);
    //ST7735_DrawCharS(0 , 100 , mem_addr,    CRSR_COLOR, 0x00, 2);//16 NOT 8 bit debug

    //show the memory address that we are navigating  
    //render_8_hex(0  , 120 , 2  , byte3       , CRSR_COLOR);
    ST7735_DrawCharS(0 , 100 , mem_addr,    CRSR_COLOR, 0x00, 2);

    //--------
    latch_outputs();//set the ports to reflect the changes
}
*/