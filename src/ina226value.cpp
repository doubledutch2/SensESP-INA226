#include "ina226value.h"
#include "sensesp.h"
#include <HardwareSerial.h>

// INA226value represents a value read from a Texaxs Instruments INA226 High Side DC Current Sensor.
INA226value::INA226value(INA226* pINA226, INA226ValType val_type, uint read_delay, String config_path) :
                   NumericSensor(config_path), pINA226{pINA226}, val_type{val_type}, read_delay{read_delay} {
      className = "INA226value";
      load_configuration();
}


void INA226value::enable() {
  app.onRepeat(read_delay, [this](){
      double scale = 0.00001;  // i.e. round to nearest one-hundred-thousandth
      switch (val_type) { 
        case bus_voltage: output = (int)(pINA226->readBusVoltage() / scale) * scale; // Volts
                          Serial.print("Bus voltage: ");
                          Serial.print(pINA226->readBusVoltage());
                          Serial.print('-');
                          Serial.println(pINA226->readBusVoltage(), 5);
                break;
        case shunt_voltage: output = (int)(pINA226->readShuntVoltage() / scale) * scale; // Volts
                            Serial.print("Shunt voltage: ");
                            Serial.println(pINA226->readShuntVoltage(), 5);
                break;
        case current: output = (int)(pINA226->readShuntCurrent() / scale) * scale; // Amps
                      Serial.print("Shunt current (amps): ");
                      Serial.println(pINA226->readShuntCurrent(), 5);  
                break;
        case power: output = (int)(pINA226->readBusPower() / scale) * scale; // Watts
                    Serial.print("Bus power (watts): ");
                    Serial.println(pINA226->readBusPower(), 5);
                break; 
        case load_voltage: output = ((int)((pINA226->readBusVoltage() + pINA226->readShuntVoltage()) / scale)) * scale; // Volts
                           Serial.print("Load voltage: ");
                           Serial.println(pINA226->readBusVoltage() + pINA226->readShuntVoltage(), 5);
                break; 
        default: debugE("FATAL: invalid val_type parameter.");  
      }
      
      notify();
 });
}

JsonObject& INA226value::get_configuration(JsonBuffer& buf) {
  JsonObject& root = buf.createObject();
  root["read_delay"] = read_delay;
  root["value"] = output;
  return root;
  };

  static const char SCHEMA[] PROGMEM = R"###({
    "type": "object",
    "properties": {
        "read_delay": { "title": "Read delay", "type": "number", "description": "The time, in milliseconds, between each read of the input" },
        "value": { "title": "Last value", "type" : "number", "readOnly": true }
    }
  })###";


  String INA226value::get_config_schema() {
  return FPSTR(SCHEMA);
}

bool INA226value::set_configuration(const JsonObject& config) {
  String expected[] = {"read_delay"};
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      return false;
    }
  }
  read_delay = config["read_delay"];
  return true;
}
