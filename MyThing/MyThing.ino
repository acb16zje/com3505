// Thing.ino
// COM3505 lab exercise runner sketch

/////////////////////////////////////////////////////////////////////////////
// NOTE!!!
// DON'T edit these files, do your coding in MyThing!
/////////////////////////////////////////////////////////////////////////////

#include <WiFi.h>
#include<WebServer.h>

const char* ssid     = "gakki";
const char* password = "gakkismile";

const char* host = "com3505.gate.ac.uk";

WebServer webServer(80);

// initialisation entry point
void setup() {
  Serial.begin(115200);
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int value = 0;

void loop(){
  webServer.handleClient();
  delay(5000);
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClient client;
  const int httpPort = 9191;
  if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
  }

  String url = "/com3505";
  url += "?email=";
  url += "jhng1@sheffield.ac.uk";
  url += "&mac=";
  url += "30:ae:a4:27:57:3c";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                "Host: " + host + "\r\n" +
                "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
      if (millis() - timeout > 5000) {
          Serial.println(">>> Client Timeout !");
          client.stop();
          return;
      }
  }

  while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }

  Serial.println();
  Serial.println("closing connection");
}
