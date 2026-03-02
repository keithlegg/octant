
#ifndef _KRNLSPLTR_
#define _KRNLSPLTR_


/*******************************/
//screen properties

#define X_RES 128
#define Y_RES 160

/*******************************/

#define bit_off_color 0x03be //light blue 
#define or_off_color 0xd440  //ORange - or
#define and_off_color 0xd750 //TAN    - and 

#define bit_on_color 0x0ee0  //lime green
#define grid_color 0x2220    //dark grey

#define BIT_ON 0x30 //ascii 1
#define BIT_OFF 0x31 //ascii 0

/*******************************/


#define CMD_NES_DRAW 0x6e        //n 
#define CMD_CLR_SCRN 0x4e        //N 
#define CMD_BITMAP 0x42          //B  //DRAW SRAM DIRECTLY
#define CMD_BITMAP2 0x43         //C  //copy AVR-SRAM DRAW SRAM
#define CMD_BITMAP3 0x44         //D  //draw AVR BITMAP


//gfx routines - consider merging with gfx lib?
void draw_outline_box(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);

void draw_byte_box(uint8_t byte, 
                   uint8_t x, uint8_t y, 
                   uint8_t w, uint8_t h, 
                   uint16_t oncolor,
                   uint16_t offcolor);

void render_8_hex(uint8_t x, uint8_t y, uint8_t size, uint8_t accumulator, uint16_t color);
void render_16_hex(uint8_t x, uint8_t y, uint8_t size, uint16_t accumulator, uint16_t color);

void latch_outputs(void);

//void listen_serial(void);

#endif