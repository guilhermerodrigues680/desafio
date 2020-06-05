#include <avr/io.h>
#include <util/delay.h>
#include <LiquidCrystal.h>
#include <avr/interrupt.h>
#include <Arduino.h>

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

#define LINHA     PIND
#define COLUNA    PORTD

#define SAIDA     PORTB
#define Q1        PORTB0
#define Q2        PORTB3
#define QUARTO_1  'A'
#define QUARTO_2  'B'
#define SALA_TV   'C'
#define SALA_JTR  'D'
#define LIGA      '*'
#define DESLIGA   '#'
#define OFF       0
#define ON        1

char LeTecla(void);
/*******    PARA USO DO DISPLAY    ***********************/
void init_dsp(int l, int c);
void putmessage(int l, int c, char *msg);
void putnumber_i(int l, int c, long ni, int nd);
void putnumber_f(int l, int c, float ni, int nd);

int main(void)
{
  char tecla;
  init_dsp(2, 16);
  putmessage(0, 0, "  Smart  Light  ");
  
  DDRD = 0x0F;
  PORTD = 0xFF;
  
  for (;;)
  {
    tecla = LeTecla();
    if (tecla != 0)
    {
      putnumber_i(1,0, tecla, 3);
    }

    _delay_ms(10);
  }
}

char LeTecla(void)
{
  unsigned char n, i, tecla = 0, linha;
  unsigned char teclado[4][4] = {{'1', '2', '3', 'A'},
                                 {'4', '5', '6', 'B'},
                                 {'7', '8', '9', 'C'},
                                 {'*', '0', '#', 'D'}};

  // DDRD = 0x0F;
  // PORTD = 0xFF;
  for (n = 0; n < 4; n++)
  {
    COLUNA &= ~_BV(n);
    linha = LINHA >> 4;
    for (i = 0; i < 4; i++)
    {
      if (!(linha & _BV(i)))
      {
        tecla = teclado[i][n];
        while (!((LINHA >> 4) & _BV(i)))
          ;
      }
    }
    COLUNA |= _BV(n);
  }
  return (tecla);
}

/*******    PARA USO DO DISPLAY    ***********************/
void init_dsp(int l, int c)
{
  lcd.begin(c, l);
}

void putmessage(int l, int c, char *msg)
{
  lcd.setCursor(c, l);
  lcd.print(msg);
}

void putnumber_i(int l, int c, long ni, int nd)
{
  char zero[] = {"0000000000000000"};
  long int nx;
  int i, j;

  nx = ni;
  for (i = 10, j = 1; nx > 9; i *= 10, j++)
    nx = ni / i;
  lcd.setCursor(c, l);
  lcd.print(&zero[16 - nd + j]);
  lcd.print(ni);
}

void putnumber_f(int l, int c, float ni, int nd)
{
  lcd.setCursor(c, l);
  lcd.print(ni, nd);
}