#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wi-Fi
const char* ssid = "Shinobi";
const char* password = "Metalurg@1966";

// MQTT
const char* mqtt_server = "m9.wqtt.ru";
const int mqtt_port = 15794;
const char* mqtt_user = "u_XLCMK6";
const char* mqtt_password = "r7Ier6j8";

// RELAY
const String relay_topic = "releControl";
bool relay_on = false;

WiFiClient espClient;
PubSubClient client(espClient);
//------------------------------------------------------------------------------------------------------------
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//------------------------------------------------------------------------------------------------------------
void updateStatePins(void){
    if(relay_on){
      digitalWrite(LED_BUILTIN, HIGH);
    }else{
      digitalWrite(LED_BUILTIN, LOW);
    }
}
//------------------------------------------------------------------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Callback");
  String data_pay;
  for (int i = 0; i < length; i++) {
    data_pay += String((char)payload[i]);
  }
  Serial.println(data_pay); 
  if( String(topic) == relay_topic ){
        if(data_pay == "ON" || data_pay == "1") relay_on = true;
        if(data_pay == "OFF" || data_pay == "0") relay_on = false;
  }
  updateStatePins();
}
//------------------------------------------------------------------------------------------------------------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266-" + WiFi.macAddress();
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password) ) {
      Serial.println("connected");
      client.subscribe( (relay_topic + "/#").c_str() );
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
//------------------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}
//------------------------------------------------------------------------------------------------------------
void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
}


