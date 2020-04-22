// ina226_example_main_cpp.txt
//  Check this website for wiring: http://e2e.ti.com/support/amplifiers/f/14/t/856127?Problems-with-Arduino-of-the-INA226-module

#include <Arduino.h>

//#define SERIAL_DEBUG_DISABLED

#define USE_LIB_WEBSOCKET true

#include "sensesp_app.h"
#include "signalk/signalk_output.h"
#include "ina226value.h"
#include "sensors/i2c_tools.h"

void checkConfig(INA226* ina)
{
  Serial.print("Mode:                  ");
  switch (ina->getMode())
  {
    case INA226_MODE_POWER_DOWN:      Serial.println("Power-Down"); break;
    case INA226_MODE_SHUNT_TRIG:      Serial.println("Shunt Voltage, Triggered"); break;
    case INA226_MODE_BUS_TRIG:        Serial.println("Bus Voltage, Triggered"); break;
    case INA226_MODE_SHUNT_BUS_TRIG:  Serial.println("Shunt and Bus, Triggered"); break;
    case INA226_MODE_ADC_OFF:         Serial.println("ADC Off"); break;
    case INA226_MODE_SHUNT_CONT:      Serial.println("Shunt Voltage, Continuous"); break;
    case INA226_MODE_BUS_CONT:        Serial.println("Bus Voltage, Continuous"); break;
    case INA226_MODE_SHUNT_BUS_CONT:  Serial.println("Shunt and Bus, Continuous"); break;
    default: Serial.println("unknown");
  }
  
  Serial.print("Samples average:       ");
  switch (ina->getAverages())
  {
    case INA226_AVERAGES_1:           Serial.println("1 sample"); break;
    case INA226_AVERAGES_4:           Serial.println("4 samples"); break;
    case INA226_AVERAGES_16:          Serial.println("16 samples"); break;
    case INA226_AVERAGES_64:          Serial.println("64 samples"); break;
    case INA226_AVERAGES_128:         Serial.println("128 samples"); break;
    case INA226_AVERAGES_256:         Serial.println("256 samples"); break;
    case INA226_AVERAGES_512:         Serial.println("512 samples"); break;
    case INA226_AVERAGES_1024:        Serial.println("1024 samples"); break;
    default: Serial.println("unknown");
  }

  Serial.print("Bus conversion time:   ");
  switch (ina->getBusConversionTime())
  {
    case INA226_BUS_CONV_TIME_140US:  Serial.println("140uS"); break;
    case INA226_BUS_CONV_TIME_204US:  Serial.println("204uS"); break;
    case INA226_BUS_CONV_TIME_332US:  Serial.println("332uS"); break;
    case INA226_BUS_CONV_TIME_588US:  Serial.println("558uS"); break;
    case INA226_BUS_CONV_TIME_1100US: Serial.println("1.100ms"); break;
    case INA226_BUS_CONV_TIME_2116US: Serial.println("2.116ms"); break;
    case INA226_BUS_CONV_TIME_4156US: Serial.println("4.156ms"); break;
    case INA226_BUS_CONV_TIME_8244US: Serial.println("8.244ms"); break;
    default: Serial.println("unknown");
  }

  Serial.print("Shunt conversion time: ");
  switch (ina->getShuntConversionTime())
  {
    case INA226_SHUNT_CONV_TIME_140US:  Serial.println("140uS"); break;
    case INA226_SHUNT_CONV_TIME_204US:  Serial.println("204uS"); break;
    case INA226_SHUNT_CONV_TIME_332US:  Serial.println("332uS"); break;
    case INA226_SHUNT_CONV_TIME_588US:  Serial.println("558uS"); break;
    case INA226_SHUNT_CONV_TIME_1100US: Serial.println("1.100ms"); break;
    case INA226_SHUNT_CONV_TIME_2116US: Serial.println("2.116ms"); break;
    case INA226_SHUNT_CONV_TIME_4156US: Serial.println("4.156ms"); break;
    case INA226_SHUNT_CONV_TIME_8244US: Serial.println("8.244ms"); break;
    default: Serial.println("unknown");
  }
  
  Serial.print("Max possible current:  ");
  Serial.print(ina->getMaxPossibleCurrent());
  Serial.println(" A");

  Serial.print("Max current:           ");
  Serial.print(ina->getMaxCurrent());
  Serial.println(" A");

  Serial.print("Max shunt voltage:     ");
  Serial.print(ina->getMaxShuntVoltage());
  Serial.println(" V");

  Serial.print("Max power:             ");
  Serial.print(ina->getMaxPower());
  Serial.println(" W");
}

ReactESP app([] () {
  #ifndef SERIAL_DEBUG_DISABLED
  Serial.begin(115200);

  // A small arbitrary delay is required to let the
  // serial port catch up

  delay(100);
  Debug.setSerialEnabled(true);
  #endif

  // Create the SensESPApp with whatever "standard sensors" you want: noStdSensors, allStdSensors, or uptimeOnly.
  // The default is allStdSensors.
  sensesp_app = new SensESPApp(noStdSensors);
  scan_i2c();

  // Create a pointer to an INA226, which represents the physical sensor.
  // 0x40 is the default address. Chips can be modified to use 0x41 (shown here), 0x44, or 0x45.
  // It's up to you to provide the proper values for begin(), configure(), and calibrate() - see
  // https://github.com/jarzebski/Arduino-INA226.
  auto* pINA226 = new INA226();
  pINA226->begin();  // uses the default address of 0x40
  pINA226->configure();
  //    pINA226->configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);  // uses the default values
  //    pINA226->calibrate();  // uses the default values
  pINA226->calibrate(0.1,0.001);  // uses the default values
  // Now the INA226 is ready for reading, which will be done by the INA226value class.

  checkConfig(pINA226);

  // Define the read_delay you're going to use, if other than the default of 500 ms.
  const uint read_delay = 10000; // once per second

  // Create an INA226value, which is used to read a specific value from the INA226, and send its output
  // to SignalK as a number (float). This one is for the bus voltage.
  auto* pINA226busVoltage = new INA226value(pINA226, bus_voltage, read_delay, "/someElectricDevice/busVoltage");
      
      pINA226busVoltage->connectTo(new SKOutputNumber("electrical.someelectricdevice.busVoltage"));

  // Do the same for the shunt voltage.
  auto* pINA226shuntVoltage = new INA226value(pINA226, shunt_voltage, read_delay, "someElectricDevice/shuntVoltage");
      
      pINA226shuntVoltage->connectTo(new SKOutputNumber("electrical.someelectricdevice.shuntVoltage"));

  // Do the same for the current (amperage).
  auto* pINA226current = new INA226value(pINA226, current, read_delay, "someElectricDevice/current");
      
      pINA226current->connectTo(new SKOutputNumber("electrical.someelectricdevice.current"));   

  // Do the same for the power (watts).
  auto* pINA226power = new INA226value(pINA226, power, read_delay, "someElectricDevice/power");
      
      pINA226power->connectTo(new SKOutputNumber("electrical.someelectricdevice.power"));  

  // Do the same for the load voltage.
  auto* pINA226loadVoltage = new INA226value(pINA226, load_voltage, read_delay, "someElectricDevice/loadVoltage");
      
      pINA226loadVoltage->connectTo(new SKOutputNumber("electrical.someelectricdevice.loadVoltage"));         

  // Here you can add as many INA226value's as you need to read all the values you want. See the INA226
  // example for details.    

  sensesp_app->enable();
});
