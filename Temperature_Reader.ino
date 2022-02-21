#include "./src/pwm.h"
#include "./src/temperature.h"
#include "./src/lcd.h"

LCD::Hitachi *display;
PWM::Manager *pwm_manager;
Temperature::Reader *temperature_reader;

void setup(void)
{
  Serial.begin(9600);
  display = new LCD::Hitachi();
  pwm_manager = new PWM::Manager();
  temperature_reader = new Temperature::Reader();
}

void loop(void)
{
  float celcius = temperature_reader->GetTemperature();
  float duty = pwm_manager->UpdateTemperature(celcius);
  Serial.println("Temperature: " + (String)celcius + "Duty: " + (String)duty);
  display->Report(celcius, duty);
  display->Update();
}
