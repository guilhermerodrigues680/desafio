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

#define TIMER1_MODULE 0xFFFF

// Delay no tinkercad em ms, usado em loops para aumentar a fluidez da simulacao
#define DELAY_SIMULACAO_TINKERCAD 10
#define _DELAY_TINKERCAD() (_delay_ms(DELAY_SIMULACAO_TINKERCAD))
// #define _DELAY_TINKERCAD()

#define _ltrans(x, xi, xm, yi, ym) (long)((long)(x - xi) * (long)(ym - yi)) / (long)(xm - xi) + yi

void quarto1(void);
void quarto2(void);
void salajantar(void);
/*** TECLADO*/
char LeTecla(void);
/*******    PARA USO DO DISPLAY    ***********************/
void init_dsp(int l, int c);
void putmessage(int l, int c, char *msg);
void putnumber_i(int l, int c, long ni, int nd);
void putnumber_f(int l, int c, float ni, int nd);


// Colocada a funcao 'void setup' pois o tinkerkad dá bug ao usar o TIMER1 sem ela
void setup()
// int main(void)
{
  char tecla;
  unsigned int result=0;
  init_dsp(2, 16);
  putmessage(0, 0, "  Smart  Light  ");
  
  

  // ICR1 = 0xFFFF;                           // Seta o TOP do timer1 de 16bit
  // OCR1A = 0x3FFF;                          // set PWM for 25% duty cycle @ 16bit
  // OCR1B = 0xBFFF;                          // set PWM for 75% duty cycle @ 16bit
  // TCCR1A |= (1 << COM1A1) | (1 << COM1B1); // set none-inverting mode

  // TCCR1A |= (1 << WGM11); // set Fast PWM mode using ICR1 as TOP
  // TCCR1B |= (1 << WGM12) | (1 << WGM13);

  // TCCR1B |= (1 << CS10); // START the timer with no prescaler

  // // Configura como saida Q1 e Q2, e tambem os pinos OC1A(PB1) e OC1B(PB2)
  // DDRB = _BV(Q2) | _BV(PORTB2) | _BV(PORTB1) | _BV(Q1);

  ICR1    = TIMER1_MODULE-1; // define o periodo do PWM 4.096ms ((1/16e6)*1*65536 * 1000) 
  TCCR1A  = _BV(COM1A1) | _BV(WGM11);// PWM em OC1A(PB1) modo fast PWM com OCR1A
  // TCCR1B  = _BV(WGM13) | _BV(WGM12) | _BV(CS11); // FPS de 8
  TCCR1B  = _BV(WGM13) | _BV(WGM12) | _BV(CS10); // FPS de 1
	// DDRB    = _BV(DDB1);  // Configura OC1B como saida
  DDRB = _BV(Q2) | _BV(DDB1) | _BV(Q1); // Configura como saida Q1 e Q2, e tambem os pinos OC1A(DDB1)
  // result = _ltrans(100, 0, 100, 0, TIMER1_MODULE-1);
  // OCR1A = result;
  // OCR1A = (int)(_ltrans(50, 0, 100, 0, TIMER1_MODULE-1));
  OCR1A = 0;


  for (;;)
  {
    tecla = LeTecla();
    if (tecla != 0)
    {
      putnumber_i(1, 0, tecla, 3);
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
        salajantar();
        break;
      default: // As teclas nao configuradas passam por aqui
        break;
      }
    }

    _DELAY_TINKERCAD(); // Delay para melhorar a fluidez na simulacao do tinkercad
  }
}


// ==> Rotina do QUARTO 1
void quarto1(void) {
  char tecla = LeTecla();
  putmessage(1,0, "Press ON ou OFF");
  while (tecla == 0) { // Fica preso aqui até apertar uma tecla
    tecla = LeTecla();
    _DELAY_TINKERCAD(); // Delay para melhorar a fluidez na simulacao do tinkercad
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
  putmessage(0, 0, "  Smart  Light  ");
}


// Colocada a funcao 'void loop' pois o tinkerkad dá bug ao usar o TIMER1 sem ela
void loop(){}


// ==> Rotina do QUARTO 2
void quarto2(void) {
  char tecla = LeTecla();
  putmessage(1,0, "Press ON ou OFF");
  while (tecla == 0) { // Fica preso aqui até apertar uma tecla
    tecla = LeTecla();
    _DELAY_TINKERCAD(); // Delay para melhorar a fluidez na simulacao do tinkercad
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
  putmessage(0, 0, "  Smart  Light  ");
}


void salajantar(void)
{
  unsigned int valor;
  char tecla = LeTecla();
  
  putmessage(1,0, "Pres. ON,OFF,0a9");

  while (tecla == 0) { // Fica preso aqui até apertar uma tecla
    tecla = LeTecla();
    _DELAY_TINKERCAD(); // Delay para melhorar a fluidez na simulacao do tinkercad
  }

  if (tecla >= '0' && tecla <= '9') // Se é um numero
  {
    // Converte o caracter em valor numerico de 0 a 9 em seguida de 0 a 100 %
    valor = _ltrans(tecla - '0', 0, 10, 0, TIMER1_MODULE-1);
    OCR1A = valor;
  } 
  else if (tecla == LIGA)
  {
    valor = _ltrans(10, 0, 10, 0, TIMER1_MODULE-1);
    OCR1A = valor;
  }
  else if (tecla == DESLIGA)
  {
    valor = _ltrans(0, 0, 10, 0, TIMER1_MODULE-1);
    OCR1A = valor;
  }
  else
  {
    // tecla diferente apertada
    // Nao faz nada
  }

  // Limpa o display
  lcd.clear();
  putmessage(0, 0, "  Smart  Light  ");
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