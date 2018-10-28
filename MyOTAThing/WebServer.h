// WebServer.h
#ifndef WEB_H
#define WEB_H

#include <ESPWebServer.h>
#include <WiFiClient.h>

// SSID and Password of your WiFi AP
String apSSID = "赤道以南,gakki还是最美";
String apPass = "gakkismile";

bool start_ota;

const char *templatePage[] = {
  "<html><head>"
  "<meta charset='utf-8'>"
  "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
  "<title>",
  "Gakki Access Point - Home",                                          // 1
  "</title><style>body {font-family: sans-serif;font-size: 1.5em;"
  "text-align: center;} nav ul {list-style-type: none; margin: 0;"
  "padding: 0; background-color: #f1f1f1;} nav ul li {display: inline;}"
  "li a {color: #000; padding: 8px 16px;"
  "text-decoration: none;}"
  "</style></head><body>"
  "<nav><ul>"
  "<li><a href='/'>Home</a></li>"
  "<li><a href='/status'>Status</a></li>"
  "<li><a href='/wifi'>WiFi</a></li>"
  "</ul></nav>",                                                        // 2
  "",                                                                   // 3
  "",                                                                   // 4
  "</body></html>"
};

ESPWebServer webServer(80);     // Create a WebServer on port 80

// Ex07.h model solution
typedef struct { int position; const char *replacement; } replacement_t;

void getHtml(String& html, const char *[], int, replacement_t [], int);

// getting the length of an array in C can be complex...
// https://stackoverflow.com/questions/37538/how-do-i-determine-the-size-of-my-array-in-c
#define ALEN(a) ((int) (sizeof(a) / sizeof(a[0]))) // only in definition scope!

#define GET_HTML(strout, boiler, repls) \
  getHtml(strout, boiler, ALEN(boiler), repls, ALEN(repls));

#endif
