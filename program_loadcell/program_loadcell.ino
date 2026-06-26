#define ENCODER_OPTIMIZE_INTERRUPTS
#include <LiquidCrystal.h>
#include <Encoder.h>


int pr = 400; //ubah ke p/r dari motor yang anda gunakan

LiquidCrystal lcd (8, 9, 4, 5, 6, 7);
int lcd_key = 0;
int adc_key_in = 0;
Encoder encModule (24, 25);
Encoder encMotor (50, 52);
long positionModule =-999;
long positionMotor = -999;
// unsigned long PrevTime = 0;
float Voltage = 0, prevVolt = -1;
float vRead = 0;
float R1 = 30000;
float R2 = 3300;
int y = 0;
unsigned long PrevTime = 0;
unsigned long currentTime = 0;
unsigned long timeDiff = 0;

#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5
#define dirMotor 2
#define pwmMotor 3
int read_LCD_buttons() {
  adc_key_in = analogRead(0);
  if (adc_key_in> 1000) return btnNONE;
  if (adc_key_in < 50) return btnRIGHT;
  if (adc_key_in< 250) return btnUP; 
  if (adc_key_in < 450) return btnDOWN; 
  if (adc_key_in < 650) return btnLEFT; 
  if (adc_key_in < 950) return btnSELECT;
  return btnNONE;
}
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print ("DUTTY = ");
  lcd.setCursor(0,1);
  lcd.print ("RPM = ");
  pinMode (10, OUTPUT);
  // analogWrite (10,0); //plan 1
  analogWrite (10,50); //plan 2
  pinMode (dirMotor, OUTPUT); 
  pinMode (pwmMotor, OUTPUT);
  digitalWrite (dirMotor, LOW); 
  // digitalWrite (pwmMotor, HIGH); //plan 1
  analogWrite (pwmMotor,0); //plan 2
}

void loop() {
  
  long newEncModule, newEncMotor;
  newEncModule = encModule.read()/2;
  newEncMotor = encMotor.read();
  Serial.println(newEncMotor);

  if (newEncModule != positionModule) {
    int dutty = abs(newEncModule) % 256;
    int duttyCycle = dutty * 100 / 255; 
    if(y == 0){
      lcd.setCursor(8, 0);
      lcd.print(duttyCycle);
      lcd.print("  ");
    }
    positionModule = newEncModule;
    // analogWrite (10,dutty); //plan 1
    analogWrite (pwmMotor,dutty); //plan2
    // Serial.println(duttyCycle);
  }

  if (abs(newEncMotor) != positionMotor) {
    // float RPM = (newEncMotor - positionMotor) / ((millis() - PrevTime)/1000) * 60 / pr; //abs
    // float RPM = (newEncMotor - positionMotor) / (millis() - PrevTime) * 1000 * 60 / pr; //abs
    // float RPM = (newEncMotor - positionMotor) / ((millis() - PrevTime)/1000) * 60 / pr;
    

    currentTime = millis();
    timeDiff = currentTime - PrevTime;
    float RPM = (-newEncMotor + positionMotor) * 60000.0 / (pr * timeDiff); 
    PrevTime = currentTime;

    PrevTime = millis();
    lcd.setCursor(6, 1 - y);
    lcd.print(RPM); // /4
    positionMotor = newEncMotor;
    if(newEncModule < 0){
      digitalWrite (dirMotor, HIGH); 
    }
    else digitalWrite (dirMotor, LOW); 
    delay(50);
    // Serial.println(RPM);
  }

  
  vRead = analogRead(10) * 3.3 / 1024; 
  Voltage = vRead / (R2 / (R1 + R2)); 
  if(prevVolt != Voltage){
    if(y ==1){
      lcd.setCursor(4, 1);
      lcd.print(Voltage);
    }
    // Serial.println(Voltage);
    prevVolt = Voltage;
  }
    

  lcd_key = read_LCD_buttons();

  switch (lcd_key) {
    case btnUP:{
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print ("DUTTY = ");
      lcd.setCursor(0,1);
      lcd.print ("RPM = ");
      y = 0;
      break;
    }
    case btnDOWN: {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print ("RPM = ");
      lcd.setCursor(0,1);
      lcd.print ("V = ");
      y = 1;
      break;
    }
  }
}