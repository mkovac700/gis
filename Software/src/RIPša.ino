#include <Arduino.h>
#include "../lib/sigfox/wisol_sigfox.h"
#include "../lib/SparkFunRHT03/src/SparkFunRHT03.h"
 
void setup();
void loop();

int button = D3; 
const int RHT03_DATA_PIN = D3; 

RHT03 rht; 

void setup() {
    Serial.begin(9600);

    rht.begin(RHT03_DATA_PIN); 
    
    pinMode(button, INPUT_PULLUP);
    
    wisol_sigfox__serial_init();
    
    if (wisol_sigfox__ready()) {
        Serial.println("Sigfox module is ready.");
    
        char buf[32];
        wisol_sigfox__get_id(buf);
        Serial.print("ID="); Serial.println(buf);
    
        wisol_sigfox__get_pac(buf);
        Serial.print("PAC="); Serial.println(buf);
    
        wisol_sigfox__get_firmware_version(buf, sizeof(buf));
        Serial.print("FirmwareVersion="); Serial.println(buf);
    
        wisol_sigfox__get_firmware_vcs_version(buf, sizeof(buf));
        Serial.print("FirmwareVCSVersion="); Serial.println(buf);
    
        wisol_sigfox__get_library_version(buf, sizeof(buf));
        Serial.print("LibraryVersion="); Serial.println(buf);
    } 
    else {
        Serial.println("Sigfox module is NOT ready.");
    }
}
 
char  buf[16];
const char  *format = "%.1f%.1f"; 

float latestHumidity = 0;
float latestTempC = 0;
 
void loop() {
	int updateRet = rht.update();
	
	if (updateRet == 1){
        latestHumidity = rht.humidity();
        latestTempC = rht.tempC();

        Serial.println("\nData successfully retrieved from sensor.");
		Serial.println("Humidity: " + String(latestHumidity,1) + " %");
		Serial.println("Temp (C): " + String(latestTempC,1) + " deg C");
	}
	else{
        Serial.println("\nFailed to retrieve data from sensor. Trying again...");
		delay(RHT_READ_INTERVAL_MS);
	}

    if(digitalRead(button)) {
        Serial.println("\nSending message via Sigfox Wisol...");
    
        int l = snprintf(buf, sizeof(buf), format, latestHumidity, latestTempC);
    
        if (wisol_sigfox__send_frame((const uint8_t*)buf, l, false)) {
            Serial.println("Message sent successfully!");
        }
        else{
            Serial.println("Message NOT sent successfully!");
        }
    }

    Serial.println("\nWaiting to repeat process...\n");

    for(int i=10;i>0;i--){
        if(i==10){
            Serial.print("Time left (min): " + String(i) + "\r");
        }
        else{
            Serial.print("Time left (min): 0" + String(i) + "\r");
        }
        delay(60000);
    }
}