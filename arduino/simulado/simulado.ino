#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

char mensaje[] = "Diego - Brian - Hugo 202004807 - Victor - Henry";

byte userCounter = 0;
struct User {
    String name;
    int phoneNumber;
    String password;    
};

int logCounter = 1;
struct Log {
    int Id;
    char Description[15];
};

void MensajeInicio() {
  int longitudMensaje = strlen(mensaje);
  
  for (int desplazamiento = 0; desplazamiento <= longitudMensaje + 16; desplazamiento++) {
    lcd.clear();
    
    // Calcula la posición de inicio del mensaje en función del desplazamiento
    int posicionInicio = max(0, desplazamiento - 16);
    
    // Calcula la posición de fin del mensaje
    int posicionFin = min(longitudMensaje, desplazamiento);
    
    // Imprime el mensaje desplazado
    lcd.setCursor(0, 0);
    lcd.print(mensaje + posicionInicio);
    if (posicionFin > 16) {
      lcd.setCursor(posicionFin - 16, 1);
      lcd.print(mensaje + posicionInicio + 16);
    }
    
    delay(20);  // Pausa de 20 milisegundos
  }
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16,4);
  // Imprime el mensaje inicialmente en la primera fila
  lcd.setCursor(0, 0);
  lcd.print(mensaje);

}

void loop() {
  MensajeInicio();

}
