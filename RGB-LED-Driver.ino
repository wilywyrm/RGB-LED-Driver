#include <SPI.h>
#include "font_table.h"

// needed for LED output/SPI communication
#define SHIFT_DAT P1_7
#define SHIFT_CLK P1_5
#define SHIFT_CLR P1_3
#define SHIFT_LAT P1_4
#define SHIFT_EN  P1_0
#define DRIVE_RST P2_0

// non-essential
#define SHIFT_OUT P1_6

// some arbitrary constants
#define FONT_TABLE_OFFSET 32
#define FONT_CHAR_WIDTH 5
#define DISPLAY_WIDTH 8

// the setup routine runs once when you press reset:
void setup() {
  // initialize the interface pins as outputs.
  pinMode(SHIFT_CLR, OUTPUT);
  pinMode(SHIFT_LAT, OUTPUT);
  pinMode(SHIFT_DAT, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_EN,  OUTPUT);
  pinMode(DRIVE_RST, OUTPUT);

  // set up SPI protocol module to talk to shift registers
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(LSBFIRST);

  // disable shift registers outputs
  digitalWrite(SHIFT_EN, HIGH);

  // clear registers and anode driver
  digitalWrite(SHIFT_CLR, LOW);
  digitalWrite(DRIVE_RST, LOW);

  // disable loading of data into register outputs
  digitalWrite(SHIFT_LAT, LOW);

  // enable and stop clearing registers
  digitalWrite(SHIFT_EN, LOW);
  digitalWrite(SHIFT_CLR, HIGH);
}

// the loop routine runs over and over again forever:
void loop() {
  prints("WINNER WINNER CHICKEN DINNER");
}

// print string
void prints(char* str){
  int a;
  // while the string has not ended
  while(*str){
    a = 0;
    char c = *str;
    for(a = 0; a < 50; a++){
      int i = 0;
      for(i = 0; i < 8; i++){
        // disable shift register loading data until transmission done
        digitalWrite(SHIFT_LAT, LOW);
        // transfer anode (row)
        SPI.transfer(1 << i);
        // transfer inverted font data for the current row
            // note that font data must be inverted since the first 3 (rgb)
            // shift registers control the cathodes (ground sides) of the LEDs
        SPI.transfer(~(font_table[c - FONT_TABLE_OFFSET][i]));


        //digitalWrite(SHIFT_EN, HIGH);
        // disable outputs momentarily
        digitalWrite(DRIVE_RST, LOW);
        // signal to anode driver it is safe to pull in new data
        digitalWrite(SHIFT_LAT, HIGH);
        // extra clock cycle because data gets shifted to registers, then another for registers to LED driver, then enable outputs

        delay(1);
      }
    }
    str++;
  }

  // print out blank character
  for(a = 0; a < 50*8; a++){
  // disable shift register loading data until transmission done
    digitalWrite(SHIFT_LAT, LOW);
    // transfer anode (row)
    SPI.transfer(0x00);
    // transfer inverted font data
    SPI.transfer(0xFF);
    SPI.transfer(0xFF);
    SPI.transfer(0xFF);
    delay(1);
    digitalWrite(SHIFT_LAT, HIGH);
  }
}


// scrolling print string (CHALLENGE MODE)
void sprints(char* str, int loops){
  int a, b;
  unsigned char char1, char2;
  const int SPACING = 1;

  // while the string is not empty
  while(*str){
      char c1, c2;
      c1 = *str;
      c2 = *(str+1);

      for(a = 0; a < loops; a++){
        int i = 0;
        for(b = 0; b < FONT_CHAR_WIDTH; b++){
          for(i = 0; i < 8; i++){
            // disable shift register loading data until transmission done
            digitalWrite(SHIFT_LAT, LOW);
            // transfer anode (row)
            SPI.transfer(1 << i);
            // transfer inverted font data for the current row
                // note that font data must be inverted since the first 3 (rgb)
                // shift registers control the cathodes (ground sides) of the LEDs
            char c1_data = font_table[c1 - FONT_TABLE_OFFSET][i];
            char c2_data = font_table[c2 - FONT_TABLE_OFFSET][i];
            char send_data = ~(c1_data << b | FONT_CHAR_WIDTH+SPACING-b >> c2_data);
            SPI.transfer(send_data);

            //digitalWrite(SHIFT_EN, HIGH);
            // disable outputs momentarily
            digitalWrite(DRIVE_RST, LOW);
            // signal to anode driver it is safe to pull in new data
            digitalWrite(SHIFT_LAT, HIGH);
            // extra clock cycle because data gets shifted to registers, then another for registers to LED driver, then enable outputs

            delay(1);
          }
        }
      }
      str++;
  }
}
