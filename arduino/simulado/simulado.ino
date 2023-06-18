#include <LiquidCrystal.h>
#include "contador.h"
#include <EEPROM.h>
#include <Keypad.h>

#define ACEPTAR 8
#define CANCELAR 9
#define BORRAR 10
#define LOOP while(true)

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

char mensaje[] = "Diego 201908327 - Brian 201807253 - Hugo 202004807 - Victor - Henry";
const int PIN_BUTTON = 2;  // Pin de seleccion
int menuIndex = 0; //estado del menu
const int cambio = 10; //boton para cambiar en el menu inicio
bool primero= true;

// string para 

char teclas[4][3] = { 
  {'1','2','3'}, 
  {'4','5','6'}, 
  {'7','8','9'}, 
  {'*','0','#'}
};

byte rowPins[4] = { 53, 52, 51, 50 };
byte colPins[4] = { 22, 23, 24 };
Keypad pad = Keypad(makeKeymap(teclas), rowPins, colPins, 4, 3);

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

void registro() {
  String nombreUsuario = escribirEnPantalla(" Registro", "Nombre: ");
  if (getName(nombreUsuario) && nombreUsuario != "") { registro(); }
  String passwordUsuario = escribirEnPantalla(" Registro", "Nombre: ");
  if (passwordUsuario == "") { registro(); }

}

char getTeclado() {
  char key = pad.getKey();
  if (key != NO_KEY) {
    Serial.print(key);
    return key;
  }
  return ' ';
}

String escribirEnPantalla(String textoPrincipal, String textoSecundario) {
  lcd.clear();
  String palabra = "";
  LOOP {
    char tecla = pad.getKey();
    if (tecla != NO_KEY) {
      palabra += tecla;
    }

    

    delay(200);
    if (digitalRead(ACEPTAR)) {
      return palabra;
    } else if (digitalRead(BORRAR)) {
      palabra = palabra.substring(0, palabra.length() - 1);
    } else if (digitalRead(CANCELAR)) {
      break;
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(textoPrincipal);
      lcd.setCursor(0, 1);
      lcd.print(textoSecundario);
      lcd.setCursor(0, 2);
      lcd.print(palabra);
    }
  }
  return "";
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

bool getName(String name) {
  int value = EEPROM.read(0);
  User temp;
  for (int i = 0; i < value; i++) {
    EEPROM.get(sizeof(int)+ sizeof(User)*i, temp);
    if (temp.name == name) {
      return true;
    }
  }
  return false;
}

User* getUser(String name) {
  int value = EEPROM.read(0);
  User temp;
  for (int i = 0; i < value; i++) {
    EEPROM.get(sizeof(int)+ sizeof(User)*i, temp);
    if (temp.name == name) {
      return &temp;
    }
  }
  return nullptr;
}

void setUser(User newUser) {
  int value = EEPROM.read(0);
  value++;
  EEPROM.put(sizeof(int)+ value*sizeof(User), newUser);
}

String cifrarXOR(String mensaje, String clave) {
  if (mensaje.length() != clave.length()) {
    Serial.println("Error: La longitud del mensaje y la clave no coinciden.");
    return;
  }
  
  // Cifrado XOR byte a byte
  String mensajeCifrado = "";
  for (int i = 0; i < mensaje.length(); i++) {
    char caracterCifrado = mensaje[i] ^ clave[i];
    mensajeCifrado += caracterCifrado;
  }
  
  return mensajeCifrado;
}

String descifrarXOR(String mensajeCifrado, String clave) {
  // Descifrado XOR byte a byte
  String mensajeDescifrado = "";
  for (int i = 0; i < mensajeCifrado.length(); i++) {
    char caracterDescifrado = mensajeCifrado[i] ^ clave[i];
    mensajeDescifrado += caracterDescifrado;
  }
  
  return mensajeDescifrado;
}

void MostrarDigito(String numDecenas) {

  if (numDecenas == "0") {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, cero[k]);
    }
  } else if (numDecenas == "1") {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, uno[k]);
    }
  } else if (numDecenas == "2") {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, dos[k]);
    }
  } else if (numDecenas == "3") {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, tres[k]);
    }
  } else if (numDecenas == "4") {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, cuatro[k]);
    }
  } else if (numDecenas == "5") {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, cinco[k]);
    }
  } else if (numDecenas == "6") {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, seis[k]);
    }
  } else if (numDecenas == "7") {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, siete[k]);
    }
  } else if (numDecenas == "8") {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, ocho[k]);
    }
  } else if (numDecenas == "9") {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, nueve[k]);
    }
  } else if (numDecenas == "*") {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, Astk[k]);
    }
  }else if (numDecenas == "#") {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, Numeral[k]);
    }
  }

}//contador con dos digitos_________________________________