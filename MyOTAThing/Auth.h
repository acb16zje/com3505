#ifndef AUTH_H
#define AUTH_H

// Login username and password ///////////////////////////////////////////
const char* loginID = "esp32";
const char* loginPass = "admin";

size_t headerkeyssize;

void setupAuth();
bool isAuthenticated();

#endif
