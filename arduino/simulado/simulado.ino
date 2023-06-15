#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

char mensaje[] = "Diego - Brian - Hugo 202004807 - Victor - Henry";
const int PIN_BUTTON = 2;  // Pin de seleccion
int menuIndex = 0; //estado del menu
const int cambio = 10; //boton para cambiar en el menu inicio
bool primero= true;

/*char teclas={ {'1','2','3'}, \
              {'4','5','6'}, \
              {'7','8','9'}, \
              {'*','0','#'} };*/


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

void Menu(){//-------------------------------------------- Menu-----------------------------------
  if(primero){
    lcd.print("  Menu Principal");
    lcd.setCursor(0, 2);
    lcd.print(">> Inicio de sesion");
    lcd.setCursor(0, 3);
    lcd.print("   Registro");
    primero=false;
  }
  

  if (digitalRead(cambio) == LOW) {
    delay(100);
    menuIndex = (menuIndex + 1) % 2; // Cambiar de opción de menú

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Menu Principal");
 
  if (menuIndex == 0){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Menu Principal");
    lcd.setCursor(0, 2);
    lcd.print(">> Inicio de sesion");
    lcd.setCursor(0, 3);
    lcd.print("   Registro");
  } else { 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Menu Principal");
    lcd.setCursor(0, 2);
    lcd.print("   Inicio de sesion");
    lcd.setCursor(0, 3);
    lcd.print(">> Registro");
  }
}
}

void setup() {//--------------------------------------------- setup -------------------------------------
  Serial.begin(9600);
  lcd.begin(16,4);
  // Imprime el mensaje inicialmente en la primera fila
  lcd.setCursor(0, 0);
  lcd.print(mensaje);

  pinMode(cambio, INPUT_PULLUP);

  for (int j=25; j<=28; j++){
    pinMode(j, INPUT);
  }
  for (int j=22; j<=24; j++){
    pinMode(j, OUTPUT);
  }

}

void loop() {//----------------------------------------------- loop --------------------------------------
  if(primero){//para que solo se muestre al inicio del programa unicamente
    MensajeInicio();
    lcd.clear();
  }
  
  Menu();

}
