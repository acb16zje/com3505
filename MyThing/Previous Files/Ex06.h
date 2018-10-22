#ifndef LAB_06_H
#define LAB_06_H

#include<WebServer.h>
#include<WiFi.h>

// SSID and Password of your WiFi router
const char* ssid = "新垣結衣";
const char* password = "gakkikawaii";

// Create a WebServer on port 80
WebServer webServer(80);

const char MAIN_page[] PROGMEM = R"=====(
<html>
	<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <title>ESP32 Page</title>
	</head>
<body>
	<h1>今日山の日なんだよ、祝日だよ！</h1>
</body>
</html>
)=====";

#endif
