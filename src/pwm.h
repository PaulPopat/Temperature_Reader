#include <Arduino.h>

#define OC1A_PIN 9
#define PIN_SENSE 2
#define DEBOUNCE 0
#define FANSTUCK_THRESHOLD 500

#define MIN_TEMP 23
#define MAX_TEMP 40
#define TEMP_RANGE (MAX_TEMP - MIN_TEMP)

namespace PWM
{

  unsigned long volatile ts1 = 0, ts2 = 0;

  void TachISR()
  {
    unsigned long m = millis();
    if ((m - ts2) > DEBOUNCE)
    {
      ts1 = ts2;
      ts2 = m;
    }
  }

  class Manager
  {
  public:
    Manager()
    {
      pinMode(PIN_SENSE, INPUT_PULLUP);
      pinMode(OC1A_PIN, OUTPUT);
      attachInterrupt(digitalPinToInterrupt(PIN_SENSE), TachISR, FALLING);
      this->SetupPWM();
    }

    float UpdateTemperature(float celcius) const
    {
      float duty = pow((celcius - MIN_TEMP) / TEMP_RANGE, 1.5);
      duty = duty < 0
                 ? 0
             : duty > 1
                 ? 1
                 : duty;
      OCR1A = (uint16_t)(320 * duty);
      return duty;
    }

    unsigned long CalcRPM() const
    {
      if (millis() - ts2 < FANSTUCK_THRESHOLD && ts2 != 0)
      {
        return (60000 / (ts2 - ts1)) / 2;
      }
      else
        return 0;
    }

  private:
    void SetupPWM() const
    {
      TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
      TCCR1B = (1 << CS10) | (1 << WGM13);
      ICR1 = 320;
      OCR1A = 0;
      OCR1B = 0;
    }
  };
}