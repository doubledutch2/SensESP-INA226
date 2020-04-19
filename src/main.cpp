// ina226_example_main_cpp.txt

#include <Arduino.h>

//#define SERIAL_DEBUG_DISABLED

#define USE_LIB_WEBSOCKET true

#include "sensesp_app.h"
#include "signalk/signalk_output.h"
#include "ina226value.h"
#include "sensors/i2c_tools.h"

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
  pINA226->configure();  // uses the default values
  pINA226->calibrate(0.1,1.0);  // uses the default values
  // Now the INA226 is ready for reading, which will be done by the INA226value class.

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
