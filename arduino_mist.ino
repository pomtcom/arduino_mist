#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266WiFiMulti.h>
#include <time.h>



const char* ssid = "3bbPOMTN";                  //ใส่ชื่อ SSID Wifi
const char* password = "T;y==yp000000";          //ใส่รหัสผ่าน

//const char* ssid = "tawatchaip_2.4G";                  //ใส่ชื่อ SSID Wifi
//const char* password = "0123456789";          //ใส่รหัสผ่าน

int timezone = 7 * 3600;                    //ตั้งค่า TimeZone ตามเวลาประเทศไทย
int dst = 0;                                //กำหนดค่า Date Swing Time

const int mistOne = D4;
const int mistTwo = D5;
const int fanOut = D6;

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);


  pinMode(mistOne, OUTPUT);
  pinMode(mistTwo, OUTPUT);
  pinMode(fanOut, OUTPUT);
  Serial.println("Set default output to LOW");

  digitalWrite(mistOne, LOW);
  digitalWrite(mistTwo, LOW);
  digitalWrite(fanOut, LOW);


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
  String printMessage = "";
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  if (p_tm->tm_year != 70) {
    if ((p_tm->tm_hour >= 8) && (p_tm->tm_hour <= 18) )
    {
      digitalWrite(fanOut, HIGH);
      
      if ((p_tm->tm_min >= 0  && p_tm->tm_min < 10) || (p_tm->tm_min >= 20 && p_tm->tm_min < 30) ||  (p_tm->tm_min >= 40 && p_tm->tm_min < 50))
      {
        printMessage = printMessage + "Mode1" ;
        digitalWrite(mistOne, HIGH);
        digitalWrite(mistTwo, LOW);
      }
      else {
        printMessage = printMessage + "Mode2" ;
        digitalWrite(mistOne, LOW);
        digitalWrite(mistTwo, HIGH);
      }
    }
    else {
      if ( (p_tm->tm_hour > 18) && (p_tm->tm_hour < 20) ){
        digitalWrite(fanOut, HIGH);
      }else{
        digitalWrite(fanOut, LOW);
      }
      
      
      printMessage = printMessage + "Mode OFFLINE" ;
      digitalWrite(mistOne, LOW);
      digitalWrite(mistTwo, LOW);
    }
  }
  else {

    printMessage = printMessage + "Mode ERROR" ;
    digitalWrite(mistOne, LOW);
    digitalWrite(mistTwo, LOW);
  }

  printMessage = printMessage + " -> " ;
  printMessage = printMessage + String(ctime(&now)) ;
  printMessage.replace("\n", "");
  Serial.println(printMessage);

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    WiFiClient client;
    HTTPClient http;

    String requestURL = "http://log.regblogs.info/arduino_elastic_index/my_doc/temp_id";

    http.begin(client, requestURL); //HTTP
    http.addHeader("Content-Type", "application/json");

    String templateMessage = "{\"message\" : \"REPLACE_WITH_STRING_HERE\"}";
    templateMessage.replace("REPLACE_WITH_STRING_HERE", printMessage) ;
    String logMessage = templateMessage;
    Serial.print("logMessage is: ");
    Serial.println(logMessage);

    int httpCode = http.POST(logMessage);
    //    Serial.println("[HTTP] POST...\n");
    Serial.print("response code is: ");
    Serial.println(httpCode);
    if (httpCode > 0) {

      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        //        Serial.println("received payload:\n<<");
        //        Serial.println(payload);
        //        Serial.println(">>");
      }
    }
    else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }



  //  =========for HTTPS connection======
  //  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
  //
  //    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  //    client->setInsecure();
  //    HTTPClient https;
  //    String requestURL = "https://kq3wtvluxh.execute-api.ap-southeast-1.amazonaws.com/v1/arduinolog?message=" + printMessage;
  //
  //    String logMessage = "{\r\n    \"message\" : REPLACE_WITH_STRING_HERE\r\n}".replace("REPLACE_WITH_STRING_HERE", printMessage) ;
  //    Serial.print("logMessage is: ", logMessage);
  //
  //    https.begin(*client, requestURL);
  //    int httpCode = https.GET();                                                                 //Send the request
  //
  //    if (httpCode > 0) { //Check the returning code
  //      String payload = https.getString();   //Get the request response payload
  //      Serial.println("GET REQUEST status is: " + String(httpCode));
  //      Serial.println("called lambda status: " + payload);                     //Print the response payload
  //    }
  //
  //    https.end();   //Close connection
  //
  //  }

  delay(1000);
}
