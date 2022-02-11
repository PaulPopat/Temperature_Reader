#include <Arduino.h>

#define THERMISTORPIN A0
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define NUMSAMPLES 5
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 10000

namespace Temperature
{
  class Reader
  {
  public:
    Reader()
    {
      analogReference(EXTERNAL);
    }

    float GetTemperature() const
    {
      return this->ToCelcius(this->TakeResistance(this->TakeAverageReading()));
    }

  private:
    float ToCelcius(float resistance) const
    {
      auto steinhart = log(resistance / THERMISTORNOMINAL);
      steinhart /= BCOEFFICIENT;
      steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);
      steinhart = 1.0 / steinhart;
      return steinhart - 273.15;
    }

    float TakeAverageReading() const
    {
      uint8_t i;
      int samples[NUMSAMPLES];

      for (i = 0; i < NUMSAMPLES; i++)
      {
        samples[i] = analogRead(THERMISTORPIN);
        delay(10);
      }

      float total = 0;
      for (i = 0; i < NUMSAMPLES; i++)
      {
        total += samples[i];
      }

      return total / NUMSAMPLES;
    }

    float TakeResistance(float average) const
    {
      return SERIESRESISTOR / (1023 / average - 1);
    }
  };
}