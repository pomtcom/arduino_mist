#include <ESP8266WiFi.h>
#include <time.h>

const char* ssid = "3bbPOMTN";                  //ใส่ชื่อ SSID Wifi
const char* password = "T;y==yp000000";          //ใส่รหัสผ่าน

int timezone = 7 * 3600;                    //ตั้งค่า TimeZone ตามเวลาประเทศไทย
int dst = 0;                                //กำหนดค่า Date Swing Time

const int mistOne = D2;
const int mistTwo = D4;

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);


  pinMode(mistOne, OUTPUT);
  pinMode(mistTwo, OUTPUT);
  Serial.println("Set default output to LOW");

  digitalWrite(mistOne, LOW);
  digitalWrite(mistTwo, LOW);


  WiFi.mode(WIFI_STA);                                        //เชื่อมต่อ Wifi
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(",");
    delay(1000);
  }
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");     //ดึงเวลาจาก Server
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");



}

void loop()
{
  time_t now = time(nullptr);
  //  Serial.println(ctime(&now));

  Serial.println(ctime(&now));

  struct tm* p_tm = localtime(&now);

  //  Serial.println("Year is");
  //  Serial.println(p_tm->tm_year);
  //  Serial.println("Hour is");
  //  Serial.println(p_tm->tm_hour);
  //  Serial.println("Minute is");
  //  Serial.println(p_tm->tm_min);


  //  if (p_tm->tm_year != 70) {
  //    if ((p_tm->tm_hour >= 8) && (p_tm->tm_hour <= 18) )
  //    {
  //      if ((p_tm->tm_min == 0 ) || (p_tm->tm_min == 15) || (p_tm->tm_min == 30) || (p_tm->tm_min == 45))
  //      {
  //        Serial.println("Open relay loop");
  //        digitalWrite(mistOne, HIGH);
  //        delay(600000);
  //        digitalWrite(mistOne, LOW);
  //      }
  //    }
  //  }


  if (p_tm->tm_year != 70) {
    if ((p_tm->tm_hour >= 8) && (p_tm->tm_hour <= 18) )
    {
      if ((p_tm->tm_min >= 0  && p_tm->tm_min < 10) || (p_tm->tm_min >= 20 && p_tm->tm_min < 30) ||  (p_tm->tm_min >= 40 && p_tm->tm_min < 50))
      {
        Serial.println("Mode1");
        digitalWrite(mistOne, HIGH);
        digitalWrite(mistTwo, LOW);
      }
      else {
        Serial.println("Mode2");
        digitalWrite(mistOne, LOW);
        digitalWrite(mistTwo, HIGH);
      }
    }
    else {
      Serial.println("Mode offline1");
      digitalWrite(mistOne, LOW);
      digitalWrite(mistTwo, LOW);
    }
  }
  else {
    Serial.println("Mode offline2");
    digitalWrite(mistOne, LOW);
    digitalWrite(mistTwo, LOW);
  }

  //      digitalWrite(mistOne, LOW);
  //      Serial.println("LOW");
  //      delay(2000);
  //      digitalWrite(mistOne, HIGH);
  //      Serial.println("HIGH");
  //      delay(2000);

  delay(1000);
}
