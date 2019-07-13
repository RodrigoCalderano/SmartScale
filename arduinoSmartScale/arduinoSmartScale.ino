#include <LiquidCrystal.h>

int pinGasSensor = A0;
int pinRequest = 3;

int gasValue = 0;
int weightValue = 18;

int filter = 0;
int gasAux = 0;
int request = 0;

LiquidCrystal lcd(12, 11, 7, 6, 5, 4);


void setup() {
  Serial.begin(9600);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(pinGasSensor, INPUT);
  pinMode(pinRequest, INPUT);
  lcd.begin(16, 2);
}

void loop() {
  request = digitalRead(pinRequest);
  delay(100);
  if (request){
      gasValue = analogRead(A0);
      gasAux = gasAux + gasValue;
      filter = filter + 1;
      if (filter == 10){
        gasValue = gasAux/10;

        // START SENDING DATA
        // GAS
        if (gasValue < 1000){
          Serial.print(0);
        }
        if (gasValue < 100){
          Serial.print(0);
        }
        if (gasValue < 10){
          Serial.print(0);
        }
        Serial.print(gasValue);
        
        // WEIGHT

        if (weightValue < 1000){
          Serial.print(0);
        }
        if (weightValue < 100){
          Serial.print(0);
        }
        if (weightValue < 10){
          Serial.print(0);
        }
        Serial.print(weightValue);
        
        gasAux = 0;
        filter = 0;
      }
  }

  

  if (digitalRead(8) == LOW) { // Botão Pressionado;
    weightValue = weightValue + 1;
  }
  if (digitalRead(9) == LOW) { // Botão Pressionado;
    weightValue = weightValue - 1;
  }



  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 3, linha 0;
  lcd.setCursor(1, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("Peso: ");
  lcd.print(weightValue);
  lcd.setCursor(1, 1);
  lcd.print("Gas:  ");
  lcd.print(analogRead(A0));
  delay(300);
   
  
}
