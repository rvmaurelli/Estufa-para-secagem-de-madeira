#include <Wire.h>                                                //Biblioteca para comutação de dados reduzida.
#include <LCD.h>                                                 //Biblioteca para funcionamento do LCD com módulo I2C.
#include <LiquidCrystal_I2C.h>                                   //Biblioteca para funcionamento do módulo I2C.
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);   //SetUp de comunicação do LCD com módulo I2C.
unsigned long int tempo = 0;                                     //Varivel para referencia de tempo na estrutura switch\case.
unsigned long int tempo_millis = 0;                              //variavel para leitura e manipulação da contagem millis.
unsigned char estado = 'A';                                      //Variavel para estrutura switch\case inicialmente no case 'A'.
unsigned int linha = 0;                                          //Contagem de linhas para o monitor serial e importação de dados ao excel.
int IMP_LCD = 0;                                                 //Variavel para impressão no LCD.
int IMP_SERIAL = 0;                                              //Variavel para impressão no serial.
float TempEst;                                                   //Leitura do sensor de temperatura da estufa.
#define secador 11                                               //Pino que define o controle do secador.
#define bomba 12                                                 //Pino que define o controle da bomba d'água.
#define botao 13                                                 //Pino para leitura do botão.
#define TEMPO_DEF 20000                                          //Referencial de tempo para estado especifico na estrutura switch\case.
#define TEMPO_LCD 2000                                           //Referencial de tempo para impressão no lcd.
#define TEMPO_SERIAL 100                                         //Referencial de tempo para impressão no serial.
#define LM A0                                                    //Define o pino A3 para efetuar a leitura do LM35.

void setup() {
  pinMode(secador, OUTPUT);
  pinMode(bomba, OUTPUT);
  pinMode(botao, INPUT);
  lcd.begin(16, 2);
  lcd.clear();
  Serial.begin(9600);
  digitalWrite(bomba, HIGH);                                     //Inicia bomba d'água desligada.
  digitalWrite(secador, HIGH);                                   //Inicia secador desligado.
}

void loop() {
  switch (estado){

   case 'A':                                                     //Caso inicial que levara ao proximo caso "S" se o botão for pressionado.
        if (digitalRead(botao) == 0){                            //Verifica se o botão foi pressionado.
            estado = 'S';                                        //Passa para o caso "S".
            while (digitalRead(botao) == 0) {}
        }
        break;

   case 'S':                                                     //Caso que ligará o secador.
        digitalWrite(secador, HIGH);                             //Secador ligado.
        estado = 'T';                                            //Passa para o caso "T".
        tempo = millis() + TEMPO_DEF;                            //Deixa a variavel "tempo", "TEMPO_DEF" milésimos de segundo à frente da contagem millis.
        break;

   case 'T':                                                     //Caso que manterá o efeito do caso "S" funcionando pelo tempo determinado por "TEMPO_DEF".
        tempo_millis = millis();                                 //Atribui o valor da contagem millis neste instante à variavel "tempo_millis".
        if (tempo < tempo_millis){                               //Compara as variaveis "tempo" e "tempo_millis".
          digitalWrite(secador, LOW);                            //Desliga o secador se "tempo_millis" for maior que "tempo".
          estado = 'B';                                          //Passa para o caso "B".
        }
        else {                                                   //Interrompe o secador, desligando-o se clicado novamente o botão.
          if (digitalRead(botao) == 0) {                         //Verifica se o botão foi pressionado.
            digitalWrite(secador, LOW);                          //Desliga o secador.
            estado = 'B';                                        //Passa para o caso "B".
            while (digitalRead(botao) == 0){}                    //Evita que o código continue rodando enquanto o botão está pressionado.
          }
        }
        break;

   case 'B':                                                     //Mantem a bomba d'àgua ligada e o secador desligado.
        digitalWrite(bomba, HIGH);                               //Liga bomba.
        if (digitalRead(botao) == 0) {                           //Se o botão for pressionado, passa para o caso "S".
          digitalWrite(bomba, LOW);                              //Desliga bomba.
          estado = 'S';                                          //Passa para o caso "S".
          while (digitalRead(botao) == 0){}                      //Evita que o código continue rodando enquanto o botão está pressionado.
        }
        break;

   default:                                                      //Caso a variável "estado" assuma um valor inválido, faz com que retorne ao estado inicial "A".
        estado = 'A';                                            //Retorna ao estado "A".
        break;
  }
  if (IMP_LCD < millis()) {                                      //Intervalo de tempo para impressão no LCD.
    IMP_LCD = millis() + TEMPO_LCD;                              //Recondiciona o intervalo de tempo para impressão.
    TempEst = analogRead(LM);                                    //lê o LM e armazena na variavel "TempEst".
    TempEst = ((TempEst * 5) / (1023) / (0.01));                 //Lê o valor de tensão do sensor LM35 e converte para graus celsius.
    lcd.setCursor (0, 0);                                        //Inicia impressão no LCD em coluna 0, linha0.
    lcd.print("Temperatura:");                                   //Imprime "Temperatura" no LCD.
    lcd.setCursor (0, 1);                                        //Inicia impressão no LCD em coluna 0, linha1.
    lcd.print(TempEst);                                          //Imprime o valor da temperatura da estufa logo após a ultima impressão.
    lcd.print("C");                                              //Imprime "C" no LCD. Referente a Celsius.
  }


  if (IMP_SERIAL < millis()) {                                   //Intervalo de tempo para impressão no monitor serial.
    IMP_SERIAL = millis() + TEMPO_SERIAL;                        //Recondiciona o intervalo de tempo para impressão.
    TempEst = analogRead(LM);                                    //lê o LM e armazena na variavel "TempEst".
    TempEst = ((TempEst * 5) / (1023) / (0.01));                 //Lê o valor de tensão do sensor LM35 e converte para graus celsius.
    Serial.print("Temperata da Estufa: ");                       //Imprime "Temperatura da Estufa" no monitor serial.
    Serial.println(TempEst);                                     //Imprime o valor da temperatura da estufa logo após a frase acima.
  }  
}
