#include <Wire.h>

// Wire: SDA is 18, SCL is 19  - https://www.pjrc.com/teensy/td_libs_Wire2.html
// Wire2: SDA is 25, SCL is 24  - https://www.pjrc.com/teensy/td_libs_Wire2.html
// Pinout for accel is Vin, 3.3V, GND, SCL, SDA, INT2, INT1, DRDY

#define SAD_W_A 0x19
#define SAD_R_A 0x19

#define CTRL1_A 0x20

#define OUT_X_L 0x28
#define OUT_Z_L 0x2C

class Accelerometer {
public:
    void setup() {
        Wire2.begin(); // Initialize I2C communication
        delay(1000); // Delay for stability
    
        Wire2.beginTransmission(SAD_W_A); // Begin transmission to device
        Wire2.write(CTRL1_A); // Suboperation code to write
        Wire2.write(0x97); // Suboperation code to write
        Wire2.endTransmission(); // End transmission
    }
    
    void getAllData() {
        // Writing to the device
        Wire2.beginTransmission(SAD_W_A); // Begin transmission to device
        Wire2.write(OUT_X_L | 0x80); // Suboperation code to write
        Wire2.endTransmission(); // End transmission
    
        delay(10); // Wait for device to process the write operation
    
        // Reading from the device
        Wire2.requestFrom(SAD_R_A, 6); // Request one byte of data from the device
        if (Wire2.available() == 6) {
            uint8_t xl = Wire2.read();
            uint8_t xh = Wire2.read();
            int16_t xdata = (xh << 8) | xl;
            uint8_t yl = Wire2.read();
            uint8_t yh = Wire2.read();
            int16_t ydata = (yh << 8) | yl;
            uint8_t zl = Wire2.read();
            uint8_t zh = Wire2.read();
            int16_t zdata = (zh << 8) | zl;
            Serial.print("x: ");
            Serial.print(convertRawToDegrees(xdata));  // divide by 16384.0 to get Gs
            Serial.print(", y: ");
            Serial.print(convertRawToDegrees(ydata));
            Serial.print(", z: ");
            Serial.println(convertRawToDegrees(zdata));
        } else {
            Serial.println("No data received");
        }
    }

    float getZDegrees() {
      // Writing to the device
        Wire2.beginTransmission(SAD_W_A); // Begin transmission to device
        Wire2.write(OUT_Z_L | 0x80); // Suboperation code to write
        Wire2.endTransmission(); // End transmission
    
        delay(10); // Wait for device to process the write operation
    
        // Reading from the device
        Wire2.requestFrom(SAD_R_A, 2); // Request one byte of data from the device
        if (Wire2.available() == 2) {
            uint8_t zl = Wire2.read();
            uint8_t zh = Wire2.read();
            int16_t zdata = (zh << 8) | zl;
            return convertRawToDegrees(zdata);
        } else {
            return 404;
        }
    } 

private:
    float convertRawToDegrees(int16_t raw) {
        float Gs = raw / 16384.0;
        Gs = Gs > 1.0 ? 1.0 : Gs;
        Gs = Gs < -1.0 ? -1.0 : Gs;
        return asin(Gs) * 180.0 / M_PI;
    }
};