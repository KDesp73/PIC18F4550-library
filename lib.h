#ifndef LIB_H
#define LIB_H

// :020000040000FA

/** -[TOC]-
 * 1. SEVEN_SEGMENT
 * 2. LCD_DISPLAY
 * 3. MISC
 * 4. KEYPAD
*/

/** Timer values
 * Prescaler    Init Value      Result Interval  
 * --------------------------------------------
 * 1            5536            5ms
 * 32           63661           5ms
 * 64           63661           10ms
 * 128          63661           20ms
 * 64           59911           30ms
 * 64           56161           50ms
 * 128          56161           100ms
 * 128          18661           500ms
 * 
*/

#use standard_io (A)
#use standard_io (B)
#use standard_io (C)

#byte PORTA = 0xF80
#byte PORTB = 0xF81
#byte PORTC = 0xF82
#byte PORTD = 0xF83
#byte PORTE = 0xF84

#define MAX_TIMER_VALUE_16 65536
#define MAX_TIMER_VALUE_8 256


#ifdef SEVEN_SEGMENT
int8 hex_table[16] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110111, // A
    0b01111100, // b
    0b00111001, // C
    0b01011110, // d
    0b01111001, // E
    0b01110001  // F
};

void display_number(int8* display_port, int8* handle_port, int16 number, int8* digit, int8 table[16], int8 initial_timer_value){ 
    if(initial_timer_value == -1) initial_timer_value = 5536;

    if(number > 999) return;


    int8 digits[] = {1, 2, 4};

    int16 mon, dec, eka;

    eka = (int8) (number / 100);
    dec = (int8) ((number - (100 * eka)) / 10);
    mon = (int8) (number - (100 * eka) - (10 * dec));


    set_timer0(initial_timer_value);
    *digit = ++*digit % 3;
    *handle_port = digits[*digit];

    if(*digit == 0){
        *display_port = table[mon];
    }

    if(*digit == 1){
        *display_port = table[dec];
    }

    if(*digit == 2){
        *display_port = table[eka];
    }
}

#endif // SEVEN_SEGMENT

#ifdef LCD_DISPLAY

#define LCD_DB4   PIN_D4
#define LCD_DB5   PIN_D5
#define LCD_DB6   PIN_D6
#define LCD_DB7   PIN_D7

#define LCD_E      PIN_D3
#define LCD_RS     PIN_D2
#define LCD_RW     PIN_D1
#define lcd_type 2        // 0=5x7, 1=5x10, 2=2 lines
#define lcd_line_two 0x40 // LCD RAM address for the 2nd line

int8 const LCD_INIT_STRING[4] = {
    0x20 | (lcd_type << 2), // Func set: 4-bit, 2 lines, 5x8 dots
    0xc,                    // Display on
    1,                      // Clear display
    6                       // Increment cursor
};

void show_seconds(int x, int y, int seconds) {
   lcd_gotoxy(x, y);
   if (seconds < 10) {
      lcd_putc("0");
      lcd_gotoxy(x + 1, y);
      printf(lcd_putc, "%d", seconds);
   } else printf(lcd_putc, "%2d", seconds);
}

void show_minutes(int x, int y, int minutes) {
   lcd_gotoxy(x, y);
   if (minutes < 10) {
      lcd_putc("0");
      lcd_gotoxy(x + 1, y);
      printf(lcd_putc, "%d", minutes);
   } else printf(lcd_putc, "%2d", minutes);
}

void show_hours(int x, int y, int hours) {
   lcd_gotoxy(x, y);
   if (hours < 10) {
        lcd_putc("0");
        lcd_gotoxy(x + 1, y);
        printf(lcd_putc, "%d", hours);
   } else printf(lcd_putc, "%2d", hours);
}

void show_separators(int x, int y) {
   lcd_gotoxy(x, y);
   lcd_putc(":");
}

void show_clock(int x, int y, int seconds, int minutes, int hours) {
    lcd_gotoxy(x, y);
    printf(lcd_putc, "%2d:%2d:%2d", hours, minutes, seconds);
}

void lcd_send_nibble(int8 nibble);
void lcd_send_byte(int8 address, int8 n);
void lcd_init(void);
void lcd_gotoxy(int8 x, int8 y);
void lcd_putc(char c);

void lcd_send_nibble(int8 nibble){
    // Note:  !! converts an integer expression
    // to a boolean (1 or 0).
    output_bit(LCD_DB4, !!(nibble & 1));
    output_bit(LCD_DB5, !!(nibble & 2)); 
    output_bit(LCD_DB6, !!(nibble & 4));   
    output_bit(LCD_DB7, !!(nibble & 8));   

    delay_cycles(20);
    output_high(LCD_E);
    delay_us(50);
    output_low(LCD_E);
}

//-----------------------------------
// This sub-routine is only called by lcd_read_byte().
// It's not a stand-alone routine.  For example, the
// R/W signal is set high by lcd_read_byte() before
// this routine is called.     


//----------------------------------------
// Send a byte to the LCD.
void lcd_send_byte(int8 address, int8 n){
    output_low(LCD_RS);

    if(address)
        output_high(LCD_RS);
    else
        output_low(LCD_RS);

    delay_cycles(10);

    output_low(LCD_E);

    lcd_send_nibble(n >> 4);
    lcd_send_nibble(n & 0xf);
}

void lcd_init(void){
    int8 i;

    output_low(LCD_RS);

    output_low(LCD_E);

    delay_ms(600);

    for(i=0 ;i < 3; i++){
        lcd_send_nibble(0x03);
        delay_ms(10);
    }

    lcd_send_nibble(0x02);

    for(i=0; i < sizeof(LCD_INIT_STRING); i++){
        lcd_send_byte(0, LCD_INIT_STRING[i]);

        // If the R/W signal is not used, then
        // the busy bit can't be polled.  One of
        // the init commands takes longer than
        // the hard-coded delay of 60 us, so in
        // that case, lets just do a 5 ms delay
        // after all four of them.

        delay_ms(10);
    }
}

//----------------------------

void lcd_gotoxy(int8 x, int8 y){
    int8 address;

    if(y != 1)
        address = lcd_line_two;
    else
        address=0;

    address += x-1;
    lcd_send_byte(0, 0x80 | address);
}

//-----------------------------
void lcd_putc(char c){
    switch(c) {
        case '\f':
            lcd_send_byte(0,1);
            delay_ms(4);
            break;

        case '\n':
            lcd_gotoxy(1,2);
            break;

        case '\b':
            lcd_send_byte(0,0x10);
            break;

        default:
            lcd_send_byte(1,c);
            break;
    }
}
#endif // LCD_DISPLAY

#ifdef MISC
int8 four_lsb(int8 port){
    int8 temp;
    delay_ms(30);
    temp = port & 0x0F;
    return temp;
}

int8 four_msb(int8 port){
    int8 temp;
    delay_ms(30);
    temp = port & 0xF0;
    return temp;
}

int calculate_initial_value(int max_timer_value, int time_ms, int prescaler){
    double mc = 83.333; // nano

    return (int) max_timer_value - ((time_ms * 1000000) / (mc * prescaler));
}
#endif // MISC

#ifdef KEYPAD

//Keypad connection:
#define row0 PIN_B4
#define row1 PIN_B5
#define row2 PIN_B6
#define row3 PIN_B7
#define col0 PIN_B0
#define col1 PIN_B1
#define col2 PIN_B2
#define col3 PIN_B3

//------------------- Variable definition----------------------------
char const KEYS[4][4] =
{{'1','2','3','A'},
 {'4','5','6','B'},
 {'7','8','9','C'},
 {'*','0','#','D'}};


#define KBD_DEBOUNCE_FACTOR 33 // Set this number to apx n/333 where
// n is the number of times you expect
// to call kbd_getc each second

//------------------Function Definition------------------------------
short int ALL_ROWS (void);
void kbd_init(void);
char kbd_getc(void);
//-------------------------------------------------------------------
void kbd_init(void)
{
set_tris_b(0xF0);
output_b(0x0F);
}
//-----------------------------------------------------------
short int ALL_ROWS (void)
{
if(input (row0) & input (row1) & input (row2) & input (row3))
return (0);
else
return (1);
}

//-----------------------------------------------------------

char kbd_getc(void)
{
static byte kbd_call_count;
static short int kbd_down;
static char last_key;
static byte col;

byte kchar;
byte row;

kchar='\0';

if(++kbd_call_count>KBD_DEBOUNCE_FACTOR)
  {
      switch (col)
              {
               case 0:
                output_low(col0);
              output_high(col1);
              output_high(col2);
              output_high(col3);
              break;
   
              case 1:
              output_high(col0);
              output_low(col1);
              output_high(col2);
              output_high(col3);
              break;

               case 2:
              output_high(col0);
                output_high(col1);
              output_low(col2);
              output_high(col3);
              break;

               case 3:
              output_high(col0);
              output_high(col1);
              output_high(col2);
              output_low(col3);
              break;
               }

            if(kbd_down)
                 {
                  if(!ALL_ROWS())
                       {
                       kbd_down=false;
                       kchar=last_key;
                        last_key='\0';
                          }
                 }
         else
              {
               if(ALL_ROWS())
                 {
                  if(!input (row0))
                        row=0;
                  else if(!input (row1))
                        row=1;
                  else if(!input (row2))
                       row=2;
                  else if(!input (row3))
                        row=3;

                  last_key =KEYS[row][col];
                  kbd_down = true;
                 }
            else
                    {
               ++col;
               if(col==4)
                     col=0;
                    }
             }
            kbd_call_count=0;
     }
         return(kchar);
}

int getNumber(int8* number_of_digits, int8 MAX_DIGITS, int16* number, char* current_char, char k)
{
    if (*number_of_digits == MAX_DIGITS)
    {
        *number_of_digits = 0;
        return 1;
    }
    else
    {
        *current_char = (k & 0b00001111)
        *number = *number + ((int)(k & 0b00001111)) * pow(10, *number_of_digits);
        *number_of_digits++;
        return 0;
    }
}


#endif // KEYPAD


#endif // LIB_H
