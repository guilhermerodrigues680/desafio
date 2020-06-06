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

void quarto1(void);
void quarto2(void);
/*** TECLADO*/
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
  
  DDRB = _BV(Q2) | _BV(Q1); // Configura como saida Q1 e Q2
  
  for (;;)
  {
    tecla = LeTecla();
    if (tecla != 0)
    {
      putnumber_i(1,0, tecla, 3);
      switch (tecla)
      {
      case QUARTO_1:
        quarto1(); // Chama a rotina do quarto 1
        break;
      case QUARTO_2:
        quarto2(); // Chama a rotina do quarto 2
        break;
      case SALA_TV:
        /* code */
        break;
      case SALA_JTR:
        /* code */
        break;
      default: // As teclas nao configuradas passam por aqui
        break;
      }
    }

    _delay_ms(10);
  }
}


// ==> Rotina do QUARTO 1
void quarto1(void) {
  char tecla = LeTecla();
  putmessage(1,0, "Press ON ou OFF");
  while (tecla == 0) { // Fica preso aqui até apertar uma tecla
    tecla = LeTecla();
  }
  if (tecla == LIGA)
  {
    SAIDA |= _BV(Q1);
  } else if (tecla == DESLIGA) {
    SAIDA &= ~_BV(Q1);
  } else {
    // tecla diferente apertada
    // Nao faz nada
  }

  // Limpa o display
  lcd.clear();
}


// ==> Rotina do QUARTO 2
void quarto2(void) {
  char tecla = LeTecla();
  putmessage(1,0, "Press ON ou OFF");
  while (tecla == 0) { // Fica preso aqui até apertar uma tecla
    tecla = LeTecla();
  }
  if (tecla == LIGA)
  {
    SAIDA |= _BV(Q2);
  } else if (tecla == DESLIGA) {
    SAIDA &= ~_BV(Q2);
  } else {
    // tecla diferente apertada
    // Nao faz nada
  }

  // Limpa o display
  lcd.clear();
}



/*******    TECLADO                ***********************/
char LeTecla(void)
{
  unsigned char n, i, tecla = 0, linha;
  unsigned char teclado[4][4] = {{'1', '2', '3', 'A'},
                                 {'4', '5', '6', 'B'},
                                 {'7', '8', '9', 'C'},
                                 {'*', '0', '#', 'D'}};

  DDRD = 0x0F;
  PORTD = 0xFF;
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