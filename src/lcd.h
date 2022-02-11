#include <Arduino.h>
#include <LiquidCrystal.h>

#define UPDATE_EVERY 1000

namespace LCD
{
  class Hitachi
  {
  public:
    Hitachi()
    {
      const auto rs = 12, en = 11, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
      this->lcd = new LiquidCrystal(rs, en, d4, d5, d6, d7);
    }

    ~Hitachi()
    {
      delete this->lcd;
    }

    void Update()
    {
      auto m = millis();
      if (m - this->last < UPDATE_EVERY) {
        return;
      }

      this->lcd->begin(16, 2);
      this->lcd->print(this->line_one);
      this->lcd->setCursor(0, 1);
      this->lcd->print(this->line_two);
      this->last = m;
    }

    void Report(float celcius, float duty)
    {
      this->line_one = "Temp: " + (String)celcius + (char)223 + "C";
      this->line_two = "Fan Duty: " + (String)(duty * 100) + "%";
    }

  private:
    LiquidCrystal *lcd;
    unsigned long last = 0;
    String line_one = "";
    String line_two = "";
  };
}