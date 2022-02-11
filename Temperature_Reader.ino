#include <LiquidCrystal.h>

#define THERMISTORPIN A0
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define NUMSAMPLES 5
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 10000

#define OC1A_PIN 9
#define OC1B_PIN 10

#define MIN_TEMP 23
#define MAX_TEMP 40
#define TEMP_RANGE (MAX_TEMP - MIN_TEMP)

#define PIN_SENSE 2
#define DEBOUNCE 0
#define FANSTUCK_THRESHOLD 500

#define UPDATE_EVERY 10

const int rs = 12, en = 11, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

unsigned long volatile ts1 = 0,ts2 = 0;

void TachISR() {
    unsigned long m = millis();
    if ((m-ts2) > DEBOUNCE) {
        ts1 = ts2;
        ts2 = m;
    }
}


unsigned long CalcRPM(){
    if (millis() - ts2 < FANSTUCK_THRESHOLD && ts2 != 0) {
        return (60000 / (ts2 - ts1)) / 2;
    } else return 0;
}

void SetupPwm() {
  pinMode(OC1A_PIN, OUTPUT);
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
  TCCR1B = (1 << CS10) | (1 << WGM13);
  ICR1 = 320;
  OCR1A = 0;
  OCR1B = 0;
}

void SetPwmDuty(float f) {
  f = f < 0 ? 0 : f > 1 ? 1 : f;
  OCR1A = (uint16_t)(320 * f);
}

float ToCelcius(float resistance) {
  float steinhart = log(resistance / THERMISTORNOMINAL);
  steinhart /= BCOEFFICIENT;
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);
  steinhart = 1.0 / steinhart;
  return steinhart - 273.15;
}

float TakeAverageReading() {
  uint8_t i;
  int samples[NUMSAMPLES];

  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
  
  float total = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     total += samples[i];
  }

  return total / NUMSAMPLES;
}

float TakeResistance(float average) {
  return SERIESRESISTOR / (1023 / average - 1);
}

void Report(float celcius, float duty) {
  lcd.begin(16, 2);
  lcd.print("Temp: " + (String)celcius + "*C");
  lcd.setCursor(0,1);
  lcd.print("Duty: " + (String)(duty * 100) + "%");
}

void setup(void) {
  Serial.begin(9600);
  pinMode(PIN_SENSE, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_SENSE), TachISR, FALLING);
  analogReference(EXTERNAL);
  SetupPwm();
}

int count = 0;

void loop(void) {
  float celcius = ToCelcius(TakeResistance(TakeAverageReading()));
  float duty = pow((celcius - MIN_TEMP) / TEMP_RANGE, 1.5);
  SetPwmDuty(duty);
  CalcRPM();

  count++;
  if (count >= UPDATE_EVERY) {
    Report(celcius, duty);
    count = 0;
  }
}
