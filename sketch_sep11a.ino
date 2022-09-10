#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Initialize Telegram BOT
#define BOTtoken "5391594642:AAEKkHyhMUUfiRbPH7Pyc-CUfhlPjmC9Hho"
#define CHAT_ID "1094421488"
X509List cert(TELEGRAM_CERTIFICATE_ROOT);

//port for temp and humidity 
DHT dht(D7, DHT11);

//for thinkspeak
WiFiClient client;

// for telegram
WiFiClientSecure client1; 
UniversalTelegramBot bot(BOTtoken, client1);

int PulseSensorPurplePin = A0; 
int Signal;       
int Threshold = 550;  

//ThinkSpeeak Channel Connection
long myChannelNumber = 1752096;
const char myWriteAPIKey[] = "N1INN7DVB7AZV7T3";
void setup() {
  //To get the current time for messaging over telegram
  configTime(0, 0, "pool.ntp.org");
  client1.setTrustAnchors(&cert);
  Serial.begin(9600);

  //Establishing a WIFI Connection 
  WiFi.begin("OnePlus6T", "djsnake1");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print("..");
  }
  Serial.println();
  Serial.println("NodeMCU is connected!");
  Serial.println(WiFi.localIP());

  //TEMPERATURE AND HUMIDITY
  dht.begin();  
  ThingSpeak.begin(client);

  //Sending bootup msg to Telegram Channel
  bot.sendMessage(CHAT_ID, "Bot started up", "");
}

void loop() {

  //Reading Pulse Rate
  Signal = analogRead(PulseSensorPurplePin);
  Serial.println(Signal);
  // Threshold for preventing false reading
  if(!(Signal>256 && Signal <768))
 {
      //Sending Alert to the bot
    bot.sendMessage(CHAT_ID, "ALERT!!Pulse Rate Not Stable", "");
  }
 

  //humdity and temperature
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.println("Temperature: " + (String) t);
  Serial.println("Humidity: " + (String) h);

  if(t>37 && t<89)
  {
      //Sending Alert to the bot
    bot.sendMessage(CHAT_ID, "ALERT!! Temperature Not Stable", "");
  }

  //Sending data to thinkspeak channel
  ThingSpeak.writeField(myChannelNumber, 1, t, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 2, h, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 3, Signal, myWriteAPIKey);
}