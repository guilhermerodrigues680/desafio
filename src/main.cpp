/*
 * Laboratório de Sistemas Embutidos Microprocessados – 2º Desafio
 * ***************************
 * 
 * Alunos:
 * - Arthur Siqueira - Graduando, Eng. Controle e Automação - PUC Minas
 * - Guilherme Rodrigues - Graduando, Eng. Controle e Automação - PUC Minas
 * 
 * ***************************
 * 
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h>
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
#define TIMER1_MODULE 0xFFFF

#define _ltrans(x, xi, xm, yi, ym) (long)((long)(x - xi) * (long)(ym - yi)) / (long)(xm - xi) + yi

void quarto1(void);
void quarto2(void);
void salajantar(void);
void salatv(void);
/*** TECLADO*/
char LeTecla(void);
/*******    PARA USO DO DISPLAY    ***********************/
void init_dsp(int l, int c);
void putmessage(int l, int c, char *msg);
void putnumber_i(int l, int c, long ni, int nd);
void putnumber_f(int l, int c, float ni, int nd);
void clearl1(void);


// Colocada a funcao 'void setup' pois o tinkerkad dá bug ao usar o TIMER1 sem ela
void setup()
// int main(void)
{
  char tecla;
  unsigned int result = 0;
  init_dsp(2, 16);
  putmessage(0, 0, "  Smart  Light  ");

  ICR1 = TIMER1_MODULE;                         // define o periodo do PWM 4.096ms ((1/16e6)*1*65536 * 1000)
  TCCR1A = _BV(COM1B1) | _BV(COM1A1) | _BV(WGM11);  // PWM em OC1A(PB1) modo fast PWM com OCR1A
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);     // FPS de 1
  DDRB = _BV(Q2) | _BV(DDB2) | _BV(DDB1) | _BV(Q1); // Configura como saida Q1 e Q2, e tambem os pinos OC1A(DDB1)
  OCR1A = 0;

  for (;;)
  {
    tecla = LeTecla();
    if (tecla != 0)
    {
      // putnumber_i(1, 0, tecla, 3); // Imprime a tecla pressionada
      switch (tecla)
      {
      case QUARTO_1:
        quarto1(); // Chama a rotina do quarto 1
        break;
      case QUARTO_2:
        quarto2(); // Chama a rotina do quarto 2
        break;
      case SALA_TV:
        salatv(); // Chama a rotina sala de tv
        break;
      case SALA_JTR:
        salajantar(); // Chama a rotina sala de jantar
        break;
      default: // As teclas nao configuradas passam por aqui
        break;
      }
    }
  }
}

// Colocada a funcao 'void loop' pois o tinkerkad dá bug ao usar o TIMER1 sem ela
void loop(){}


// ==> Rotina do QUARTO 1
void quarto1(void) {
  char tecla = LeTecla();
  
  clearl1();
  putmessage(1,0, "Press ON ou OFF");

  while (tecla == 0)  // Fica preso aqui até apertar uma tecla
  {
    tecla = LeTecla();
  }
  if (tecla == LIGA)
  {
    SAIDA |= _BV(Q1);
    clearl1();
    putmessage(1, 0, "QUARTO 1 : ON");
  }
  else if (tecla == DESLIGA)
  {
    SAIDA &= ~_BV(Q1);
    clearl1();
    putmessage(1, 0, "QUARTO 1 : OFF");
  } 
  else  // tecla diferente apertada
  {
    clearl1();
    putmessage(1, 0, "NADA EXECUTADO");
  }
}


// ==> Rotina do QUARTO 2
void quarto2(void) {
  char tecla = LeTecla();
  clearl1();
  putmessage(1,0, "Press ON ou OFF");

  while (tecla == 0) { // Fica preso aqui até apertar uma tecla
    tecla = LeTecla();
  }
  if (tecla == LIGA)
  {
    SAIDA |= _BV(Q2);
    clearl1();
    putmessage(1, 0, "QUARTO 2 : ON");
  } else if (tecla == DESLIGA) {
    SAIDA &= ~_BV(Q2);
    clearl1();
    putmessage(1, 0, "QUARTO 2 : OFF");
  } else {
    clearl1();
    putmessage(1, 0, "NADA EXECUTADO");
  }
}

void salatv(void)
{
  unsigned int valor;
  char tecla = LeTecla();
  clearl1();
  putmessage(1,0, "Pres. ON,OFF,0a9");

  while (tecla == 0) { // Fica preso aqui até apertar uma tecla
    tecla = LeTecla();
  }

  if (tecla >= '1' && tecla <= '9') // Se é um numero
  {
    // Converte o caracter em valor numerico de 1 a 9 em seguida de 0 a 100 %
    valor = _ltrans(tecla - '0', 0, 10, 0, TIMER1_MODULE);
    OCR1B = valor;
    clearl1();
    putmessage(1, 0, "SALA TV:   %");
    putnumber_i(1, 9, (tecla - '0') * 10, 2);
  } 
  else if (tecla == '0')
  {
    valor = _ltrans(10, 0, 10, 0, TIMER1_MODULE);
    OCR1B = valor;
    clearl1();
    putmessage(1, 0, "SALA TV: 100%");
  }
  else if (tecla == DESLIGA)
  {
    valor = _ltrans(0, 0, 10, 0, TIMER1_MODULE);
    OCR1B = valor;
    clearl1();
    putmessage(1, 0, "SALA TV: OFF");
    
  }
  else
  {
    clearl1();
    putmessage(1, 0, "NADA EXECUTADO");
  }
}


void salajantar(void)
{
  unsigned int valor;
  char tecla = LeTecla();
  
  clearl1();
  putmessage(1,0, "Pres. ON,OFF,0a9");

  while (tecla == 0)  // Fica preso aqui até apertar uma tecla
  {
    tecla = LeTecla();
  }

  if (tecla >= '1' && tecla <= '9') // Se é um numero
  {
    // Converte o caracter em valor numerico de 1 a 9 em seguida de 0 a 100 %
    valor = _ltrans(tecla - '0', 0, 10, 0, TIMER1_MODULE);
    OCR1A = valor;
    clearl1();
    putmessage(1, 0, "SALA JANTAR:   %");
    putnumber_i(1, 13, (tecla - '0') * 10, 2);
  } 
  else if (tecla == '0')
  {
    valor = _ltrans(10, 0, 10, 0, TIMER1_MODULE);
    OCR1A = valor;
    clearl1();
    putmessage(1, 0, "SALA JANTAR:100%");
  }
  else if (tecla == DESLIGA)
  {
    valor = _ltrans(0, 0, 10, 0, TIMER1_MODULE);
    OCR1A = valor;
    clearl1();
    putmessage(1, 0, "SALA JANTAR: OFF");
  }
  else // tecla diferente apertada
  {
    clearl1();
    putmessage(1, 0, "NADA EXECUTADO");
  }
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

void clearl1(void){
  putmessage(1,0, "                "); // Limpa a linha 1 do display
}