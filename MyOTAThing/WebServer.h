#ifndef WEB_H
#define WEB_H

#include <ESPWebServer.h>

// Create a WebServer on port 80
ESPWebServer webServer(80);

// SSID and Password of your WiFi AP
String apSSID = "Gakki - ";
String apPass = "gakkismile";

bool startOTA = false;  // For determining whether to start the update
bool startReset = false; // For determining whether to start factory reset

const char *templatePage[] = {
  "<html><head>"
  "<meta charset='utf-8'>"
  "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
  "<title>",
  "Gakki Access Point - Home",                                          // 1
  "</title><style>body {font-family: sans-serif; font-size: 1.5em;"
  "text-align: center; max-width:600px; margin: 0 auto}"
  ".menu {list-style-type: none; margin: 0; padding: 0; overflow: auto;"
  "border: 1px solid #e7e7e7; background-color: #f1f1f1;}"
  ".menu li {float: left} .menu li a {display: block; color: #000;"
  "text-decoration: none; padding: 10px} .menu li a:hover"
  "{background-color: #ddd} table {width:100%; font-size: 24px;}"
  "td {border: 1px solid #dddddd; padding: 8px;}"
  ".update {border: 3px solid #12a138; display: inline-block; padding:"
  "8px 16px; vertical-align: middle; overflow: hidden; text-decoration:"
  "none; color: #12a138; background-color: #fff; text-align: center;"
  "cursor: pointer; white-space: nowrap;} .update:hover {background-color:"
  "#12a138; color: #fff;} .reset {border: 3px solid #d81313; display:"
  "inline-block; padding: 8px 16px; vertical-align: middle; overflow:"
  "hidden; text-decoration: none; color: #d81313; background-color: #fff;"
  "text-align: center; cursor: pointer; white-space: nowrap;} .reset:"
  "hover {background-color: #d81313; color: #fff;} input[type=submit]"
  "{border: 3px solid #000; display: inline-block; padding: 8px 20px;"
  "vertical-align: middle; overflow: hidden; text-decoration: none; color:"
  "#000; background-color: #fff; text-align: center; cursor: pointer;"
  "white-space: nowrap; font-size: 18px;}"
  "</style></head><body>",
  "<nav><ul class='menu'>"
  "<li><a href='/'>Status</a></li>"
  "<li><a href='/wifi'>WiFi</a></li>"
  "<li style='float:right'><a href='/logout'>Logout</a></li>"
  "</ul></nav>",                                                        // 3
  "",                                                                   // 4
  "",                                                                   // 5
  "</body><script>document.addEventListener('click', function() {"
  "if (document.getElementById('hidRadio').checked) {"
  "document.getElementById('hidText').disabled = false} else {"
  "document.getElementById('hidText').disabled = true}})</script></html>"
};

void startWebServer();
void routes();
void hndlNotFound();
void hndlRoot();
void hndlWifi();
void hndlWifichz();
void hndlOTA();
void apListForm(String& f);

// Ex07.h model solution
typedef struct { int position; const char *replacement; } replacement_t;

void getHtml(String& html, const char *[], int, replacement_t [], int);

// getting the length of an array in C can be complex...
// https://stackoverflow.com/questions/37538/how-do-i-determine-the-size-of-my-array-in-c
#define ALEN(a) ((int) (sizeof(a) / sizeof(a[0]))) // only in definition scope!

#define GET_HTML(strout, boiler, repls) \
  getHtml(strout, boiler, ALEN(boiler), repls, ALEN(repls));

#endif
