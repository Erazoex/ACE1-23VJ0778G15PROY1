#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

byte userCounter = 0;
struct User {
    string name;
    int phoneNumber;
    string password;    
};

int logCounter = 1;
struct Log {
    int Id;
    char Description[15];
};

void setup() {
  

}

void loop() {
  // put your main code here, to run repeatedly:

}
