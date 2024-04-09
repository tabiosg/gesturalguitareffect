#define POT_PIN A14  // TODO - this needs to change

// A14 is Pin 38

class Potentiometer {
  private:
  public:
    void setup() {
      pinMode(POT_PIN, INPUT); // Set up A14 as an analog input pin
    }
    
    int getRawData() {
      return analogRead(POT_PIN); // Return the raw value
    }
    
    float getConvertedData() {
      int rawData = getRawData();
      float scaledValue = map(rawData, 0, 1023, 0, 100) / 100.0; // Scale the value to range 0.0 to 1.0
      #ifdef DEBUG
      String output = "$POTENTRAWTOSCALED," + String(rawData) + "," + String(scaledValue) + ",";
      Serial.println(output);
      #endif
      return scaledValue; // Return the scaled value
    }
};