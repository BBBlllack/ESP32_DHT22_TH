#include <Arduino.h>
#include <WiFi.h>
#include <vector>
#include <HTTPClient.h>
#include <ctime>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 4
#define DHTTYPE DHT22
#define DelayMs 2500

using namespace std;

HTTPClient http;
DHT_Unified dht(DHTPIN, DHTTYPE);

String getNowTime(){
  time_t now = time(0);
  tm *ltm = localtime(&now);
  return String(ltm->tm_hour) + ":" + String(ltm->tm_min) + ":" + String(ltm->tm_sec);
}

vector<String> getAvaiWifi(){
  Serial0.println("Starting scan avaiable wifi...");
  vector<String> wifis;
  int nets = WiFi.scanNetworks();
  for (int i = 0; i < nets; i++)
  {
    wifis.push_back(WiFi.SSID(i));
  }
  return wifis;
}

void WifiInit(){
  char *ssid = "ChinaNet-cH73-2f";
  char *pass = "sw18931135691";
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial0.println("WiFi is connected!\n name:" + String(ssid));
    Serial0.println(WiFi.localIP());
    Serial0.println("ESP32 MAc: " + WiFi.macAddress());
  }

}

// the setup function runs once when you press reset or power the board
void setup() {
  // No need to initialize the RGB LED
  Serial0.begin(115200);
  for(auto &w: getAvaiWifi()){
    Serial0.println(w);
  }
  WifiInit();
  dht.begin();
}

vector<float> getDHT_U(){
  // Get temperature event and print its value.
  vector<float> data;
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial0.println(F("Error reading temperature!"));
  }
  else {
    Serial0.print(F("Temperature: "));
    Serial0.print(event.temperature);
    data.push_back(event.temperature);
    Serial0.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial0.println(F("Error reading humidity!"));
  }
  else {
    Serial0.print(F("Humidity: "));
    Serial0.print(event.relative_humidity);
    data.push_back(event.relative_humidity);
    Serial0.println(F("%"));
  }
  return data;
}

void sendReq(vector<float> data){
  String api = "http://192.168.1.150:5000/recv?temp=";
  api.concat(String(data[0]));
  api.concat("&humid=");
  api.concat(String(data[1]));
  http.begin(String(api));
  int http_code = http.GET();
  if (http_code == HTTP_CODE_OK)
  {
    api.concat(", ");
    api.concat(http.getString());
    Serial0.println(api);
  }
}

void loop() {
  sendReq(getDHT_U());
  delay(DelayMs);
}
