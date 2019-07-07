int pinGasSensor = A0;
int pinRequest = 3;

int gasValue = 0;
int weightValue = 18;

int filter = 0;
int gasAux = 0;
int request = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pinGasSensor, INPUT);
  pinMode(pinRequest, INPUT);
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

  // TODO: ADJUST WEIGHT VALUE WITH BUTTONS
  
  delay(100);
}
