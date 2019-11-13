#include <ESP8266WiFi.h>
#define myPeriodic 1 

// Thingspeak API
const char* server2= "api.thingspeak.com";
String apiKey = "MXIGDOH7LXR8ZV7E";

// Wifi Router Configuration
const char* ssid = "wifi";
const char* password = " ";
int sent = 0;

// Input Passive Infrared Sensor Pin
int inputPir = 4;                          
int val;

// Input LED Pin
int ledPin = 16;

// Input Potentiometer Pin
int potentiometer = 17;

// Potentiometer Value Reading Value
int potentiometerValueReading = 0;

// WiFiServer Configuration
WiFiServer server(80);

void setup() {
  //PIR
  pinMode(inputPir, INPUT);
  pinMode(potentiometer, INPUT);  

  Serial.begin(9600);
  delay(10);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  
}
 
void loop() {
  potentiometerValueReading = analogRead(potentiometer);

  // Multiplied by 3.3 since the source is 3.3V
  float convertedValue = (potentiometerValueReading / 1024.0) * 3.3;
    
  Serial.print("Potensiometer reading: ");Serial.println(potentiometerValueReading);
  Serial.print("Converted value : ");Serial.println(convertedValue);

  float potentiometer;
  sendDATA(convertedValue);
  int count = myPeriodic;
  while(count--)
  delay(1000);

  //SendData
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1)  {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(ledPin, LOW);
    value = LOW;
  }
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Led pin is now: ");
 
  if(value == HIGH) {
    client.print("On");
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button>Turn On </button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button>Turn Off </button></a><br />");  
  client.println("</html>");
  client.println(" ");
  client.println(" ");
  client.print("Pembacaan potensiometer : ");
  client.println(potentiometerValueReading);
  client.print("convertedValue : ");
  client.println(convertedValue);
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}

void printDigits(int digits) {
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void sendDATA(float val)
{  
   WiFiClient client;
  
   if (client.connect(server2, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
   Serial.println("WiFi Client connected ");
   
   String postStr = apiKey;
   postStr += "&field1=";
   postStr += String(val);
   postStr += "\r\n\r\n";
   
   client.print("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n\n");
   client.print(postStr);
   delay(10);
   
   }
   sent++;
 client.stop();
}
