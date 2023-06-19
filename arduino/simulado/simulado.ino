#include <LiquidCrystal.h>
#include <LedControl.h>
#include "contador.h"
#include <EEPROM.h>
#include <Keypad.h>

#define LINEA_VACIA "                "
#define OFFSET_USUARIOS 16
#define ACEPTAR 8
#define CANCELAR 9
#define BORRAR 10
#define CLOCK 49
#define LOAD 48
#define DIN 47
#define LOOP while(true)

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

char mensaje[] = "Diego 201908327 - Brian 201807253 - Hugo 202004807 - Victor 201403946 - Henry 201906562";
bool primero= true;
bool opcionesMostradas = false;
int opcionAdministrador = -1;

// int para opciones en menus
int opcion = 0;
const int numeroDispositivos = 1; // Número de controladores MAX7219 conectados
const int pinDIN = 11; // Pin de datos (MOSI)
const int pinCLK = 13; // Pin de reloj (SCK)
const int pinCS = 10; // Pin de selección del dispositivo (SS)

int TipoLogin=""; //1) Admin, 2) USUARIO

LedControl matriz_driver = LedControl(pinDIN, pinCLK, pinCS, numeroDispositivos);
// string para 

char teclas[4][3] = { 
  {'1','2','3'}, 
  {'4','5','6'}, 
  {'7','8','9'}, 
  {'*','0','#'}
};

byte rowPins[4] = { 25, 26, 27, 28 };
byte colPins[4] = { 22, 23, 24 };
Keypad pad = Keypad(makeKeymap(teclas), rowPins, colPins, 4, 3);

byte userCounter = 0;
struct User {
    String name;
    String phoneNumber;
    String password;    
};
struct usuario {//corregir valores
    char nombre[11]; //tamaño de 8-9   [A-Z0-9]+
    char contra[11];//8   [0-9]+
    char numero[9] ;// 8-12  [A-Z0-9*#$!]+
};
void borrarEEPROM() {
    for (int i = 0; i < EEPROM.length(); i++)
        EEPROM.write(i, 0);
}

int logCounter = 1;
struct Log {
    int Id;
    char Description[15];
};


void setup() {//--------------------------------------------- setup -------------------------------------
  Serial.begin(9600);
  lcd.begin(16,4);
  // Imprime el mensaje inicialmente en la primera fila
  lcd.setCursor(0, 0);
  lcd.print(mensaje);

  matriz_driver.shutdown(0, false); // Activar el controlador MAX7219
  matriz_driver.setIntensity(0, 7); // Ajustar la intensidad de brillo (0-15)
  matriz_driver.clearDisplay(0); // Limpiar la matriz de LEDs

  for (int j=25; j<=28; j++){
    pinMode(j, INPUT);
  }
  for (int j=22; j<=24; j++){
    pinMode(j, OUTPUT);
  }
  //borrarEEPROM();

}

void loop() {//----------------------------------------------- loop --------------------------------------
  
  if(/*primero*/false){//para que solo se muestre al inicio del programa unicamente
    MensajeInicio();
    lcd.clear();
    primero = false;
  }
  Menu();
  
}

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

enum estados {//----------------------estados del munu de usuario
    MENU,
    REGISTRO,
    ESPERANDO,
    LOGIN,
    LOGS
} siguiente_estado, estado_actual = MENU;

byte opcion_menu = 0;
char nombre_temp[11];
char contra_temp[11];
char numero_temp[9] ;
int indiceNombre = 0;
int indiceContra = 0;
int indiceComple = 0;
int direccionEEPROM = 1;

boolean entradaAceptada() {
  LOOP {
  if (digitalRead(8)) {//aceptar
    delay(210);
    return true;
    }
  if (digitalRead(9)) {//cancelar
    delay(210);
    return false;
  }
 }
}
void limpiarBuffer() {
    int t0 = millis();
    int t1 = millis();
    LOOP {
        t1 = millis();
        while(Serial.available()) {
        Serial.read();
        }
        if ((t1 - t0 >= 100) && !Serial.available()) break;
    }
}
void imprimirAsteriscos(char* cadena) {
    for (; *cadena; cadena++)
        lcd.write('*');
}

void Menu(){//-------------------------------------------- Menu-----------------------------------

  char teclado = getTeclado();
  MostrarDigito((teclado));

  if (!opcionesMostradas) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Menu Principal");
    lcd.setCursor(0,1);
    lcd.print(">>Inicio sesion");
    opcionesMostradas = true;
  } else {
    if (teclado == '8') {
      lcd.clear();
      opcionAdministrador = 0;
      lcd.setCursor(0,0);
      lcd.print("Menu Principal");
      lcd.setCursor(0,1);
      lcd.print(">>Inicio sesion");
      
    } else if (teclado == '2'){
      lcd.clear();
      opcionAdministrador = 1;
      lcd.setCursor(0,0);
      lcd.print(">>Menu Principal");
      lcd.setCursor(0,1);
      lcd.print("Inicio sesion");
    }

    if (teclado == '*') {
      if (opcionAdministrador == 0) {
        //configuracionProducto();
      } else if (opcionAdministrador == 1) {
        MenuUsuario();
      }
    }
  }

}

void registro() {
  String nombreUsuario = escribirEnPantalla("Menu registro", "Nombre:");
  if (nombreUsuario != "" && getName(nombreUsuario)) { registro(); }
  String passwordUsuario = escribirEnPantalla("Menu registro", "Password:");
  if (passwordUsuario == "") { registro(); }
  String numeroUsuario = escribirEnPantalla("Menu registro", "Numero:");
  if (numeroUsuario == "") { registro(); }
  User newUser;
  newUser.name = nombreUsuario;
  newUser.password = cifrarXOR(passwordUsuario, passwordUsuario);
  newUser.phoneNumber = numeroUsuario;
  setUser(newUser);
  lcd.clear();
  imprimirEnPantalla("Registro", 0, 0);
  imprimirEnPantalla("Exitoso!", 0, 1);
  Menu();
}

void inicioSesion() {
  // TODO: implementar inicio sesion admin
  // por medio de la aplicacion Bluetooth.
  String nombreUsuario = escribirEnPantalla("Inicio Sesion", "Nombre:");
  if (nombreUsuario == "") { inicioSesion(); }
  String passwordUsuario = escribirEnPantalla("Inicio Sesion", "Password:");
  if (passwordUsuario == "" && isUserByNameAndPassword(nombreUsuario, passwordUsuario)) { inicioSesion(); }
  User *logged;
  logged = getUserByNameAndPassword(nombreUsuario, passwordUsuario);
}

void MenuUsuario() {
  bool salir=true;
  // TODO: implementar menu usuario
  // por medio de la aplicacion Bluetooth.
  while(salir){
    switch (estado_actual) {
      case ESPERANDO: {//-------------------------------esperando----------------------------
        limpiarBuffer();
        lcd.clear();
        lcd.print(" Esperando  una ");
        lcd.setCursor(0, 1);
        lcd.print("   conexion...  ");
        char caracter;
        bool esperando=true;
        while(esperando){
          if(Serial.available()){
            caracter = Serial.read();       
            if(caracter == 'c'){
              lcd.clear();
              lcd.print("Conectado");
              delay(100);
              estado_actual = siguiente_estado;
              esperando=false;
            }
          }
        }
         break;
       }//fin esperando

        
        case MENU: {//----------------------------------------menu-----------------------------
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("  Menu Principal");            
            lcd.setCursor(0, 2);
            lcd.print("1. LOGIN");
            lcd.setCursor(0, 3);
            lcd.print("2. REGISTRO");
            lcd.setCursor(0, (opcion_menu));
            lcd.print(">>");  
            delay(10);
            LOOP {
                bool salir = false;
                char tecla = pad.getKey();
                delay(50);
                if (tecla) {
                  char teclax=tecla;
                  
                  switch (teclax){
                    case '1':
                      opcion_menu=1;  
                      lcd.setCursor(0, 0);
                      lcd.print("  Menu Principal");            
                      lcd.setCursor(0, 2);
                      lcd.print("1. LOGIN");
                      lcd.setCursor(0, 3);
                      lcd.print("2. REGISTRO");
                      lcd.setCursor(0, (opcion_menu+1));
                      lcd.print(">>");
                      estado_actual = ESPERANDO;
                      siguiente_estado = LOGIN;
                      salir=true;
                    break;
                    case '2':
                      opcion_menu=2;                 
                      lcd.setCursor(0, 0);
                      lcd.print("  Menu Principal");            
                      lcd.setCursor(0, 2);
                      lcd.print("1. LOGIN");
                      lcd.setCursor(0, 3);
                      lcd.print("2. REGISTRO");
                      lcd.setCursor(0, (opcion_menu+1));
                      lcd.print(">>");
                      estado_actual = ESPERANDO;
                      siguiente_estado = REGISTRO;
                      salir=true;                      
                      break;
                      
                    default:
                      salir=false;
                    break;
                  }                  
                  
                }//fin if(tecla)
             
                if (salir){ break;}
            }
            break;
        }
        case LOGIN: {//-----------------------------------------------case login----
            memset(nombre_temp, 0, 11);    
            memset(contra_temp, 0, 11);    
            memset(numero_temp, 0,  9);    
            struct usuario nuevo_usuario;
      LOOP {
          limpiarBuffer();
          //enviarConfirmar("usuario");
              Serial.print("usuario");
              memset(nombre_temp, 0, 11);//destino, valor, tamaño    
                lcd.clear();
                lcd.print("L O G I N");
                lcd.setCursor(0, 1);
                lcd.print(" - NOMBRE:");
                lcd.setCursor(0, 2);
                Serial.flush();
                delay(5);
                // OBTENER CADENA DE APLICACIÓN -- Nombre
                bool seEnvioAlgo = false;
                int indiceNombre = 0;
                long int t0 = millis();
                long int t1 = millis();
          limpiarBuffer();
                LOOP {
                    // SI YA SE ENVIO ALGO DESDE LA APLICACION
                    while (Serial.available()) {
                        seEnvioAlgo = true;
                    //   RECIBIRLO
                        nombre_temp[indiceNombre++] = Serial.read();
                    }
                    // CONTROLAR CUANTO HA PASADO DESDE QUE COME...
                    if (seEnvioAlgo) {
                        t1 = millis();
                        if (t1 - t0 >= 50) break;
                    } else {
                        t0 = millis();
                        t1 = millis();
                    }
                }
                lcd.print(nombre_temp);
                lcd.setCursor(0, 3);
                lcd.print("Correcto?       ");
                delay(10);
              if (entradaAceptada()){ 
              lcd.setCursor(0, 2);
              lcd.print(LINEA_VACIA);
              lcd.setCursor(0, 3);
              lcd.print(LINEA_VACIA);
              break;
              }
            }
      LOOP {
          limpiarBuffer();
          //enviarConfirmar("contrasena");
          Serial.print("contrasena");
              memset(contra_temp, 0, 11);    
                lcd.clear();
                lcd.print("L O G I N");
                lcd.setCursor(0, 1);
                lcd.print(" - Contras:");
                lcd.setCursor(0, 2);
                delay(5);
                // OBTENER CADENA DE APLICACIÓN -- Nombre
                bool seEnvioAlgo = false;
                int indiceContra = 0;
                long int t0 = millis();
                long int t1 = millis();
          limpiarBuffer();
                LOOP {
                    // SI YA SE ENVIO ALGO DESDE LA APLICACION
                    while (Serial.available()) {
                        seEnvioAlgo = true;
                    //   RECIBIRLO
                        contra_temp[indiceContra++] = Serial.read();
                    }
                    // CONTROLAR CUANTO HA PASADO DESDE QUE COME...
                    if (seEnvioAlgo) {
                        t1 = millis();
                        if (t1 - t0 >= 50) break;
                    } else {
                        t0 = millis();
                        t1 = millis();
                    }
                }
    imprimirAsteriscos(contra_temp);
                lcd.setCursor(0, 3);
                lcd.print("Correcto?       ");
                delay(10);
              if (entradaAceptada()){ 
              lcd.setCursor(0, 2);
              lcd.print(LINEA_VACIA);
              lcd.setCursor(0, 3);
              lcd.print(LINEA_VACIA);
              break;
              }
            }
      //enviarConfirmar("NADA");
      // LEER EEPROM
      byte usuarios = 0;
      EEPROM.get(0, usuarios);
      int siguiente_direccion = OFFSET_USUARIOS;
      bool encontrado = false;
      for (int i = 0; i < usuarios; i++) {
          struct usuario usuario_existente;
    EEPROM.get(siguiente_direccion, usuario_existente);
    //contra_temp=cifrarXOR(contra_temp,contra_temp);
    if (strcmp(nombre_temp, usuario_existente.nombre) == 0 && \
        strcmp(contra_temp, usuario_existente.contra) == 0) {
        encontrado = true;
    }
          siguiente_direccion += sizeof(struct usuario);
      }
      lcd.clear();
      if (encontrado) {
        lcd.print("ENCONTRADO");
        salir=false; //--------------------------------------------ahora entramos a las opciones del menu user
      }
      else {
        lcd.print("NO ENCONTRADO");
        estado_actual=MENU;
      }
      siguiente_estado = MENU;
      delay(100);
            break;
        }
        case REGISTRO: {//--------------------------------------case registro-------------------------------
          lcd.clear();
          lcd.print("R E G I S T R O");
            memset(nombre_temp, 0, 11);    
            memset(contra_temp, 0, 11);    
            memset(numero_temp, 0,  9);    
            struct usuario nuevo_usuario;                  
            delay(50);
          LOOP {// es necesario el bucle por si no se cancela el valor enviado
              lcd.clear();
              lcd.print("R E G I S T R O");
              lcd.setCursor(0, 1);
              lcd.print(" - NOMBRE:");
              lcd.setCursor(0, 2);
              delay(1);
              limpiarBuffer();
              //enviarConfirmar("usuario");
              Serial.print("usuario");   
              Serial.flush();    
              memset(nombre_temp, 0, 11);    
              
              // OBTENER CADENA DE APLICACIÓN -- Nombre
              bool seEnvioAlgo = false;
              int indiceNombre = 0;
              long int t0 = millis();
              long int t1 = millis();
              limpiarBuffer();
              LOOP {
                  // SI YA SE ENVIO ALGO DESDE LA APLICACION
                  while (Serial.available()) {
                      seEnvioAlgo = true;
                  //   RECIBIRLO
                      nombre_temp[indiceNombre++] = Serial.read();
                  }
                  // CONTROLAR CUANTO HA PASADO DESDE QUE COME...
                  if (seEnvioAlgo) {
                      t1 = millis();
                      if (t1 - t0 >= 50) break;
                  } else {
                      t0 = millis();
                      t1 = millis();
                  }
                }
              lcd.print(nombre_temp);
              lcd.setCursor(0, 3);
              lcd.print("Correcto?       ");
              delay(10);
              if (entradaAceptada()){ 
              lcd.setCursor(0, 2);
              lcd.print(LINEA_VACIA);
              lcd.setCursor(0, 3);
              lcd.print(LINEA_VACIA);
              break;
              }
             }//---------------------
      LOOP {
          //limpiarBuffer();
          //enviarConfirmar("telefono");
              memset(numero_temp, 0, 9);//dest, val, long
                lcd.clear();
                lcd.print("R E G I S T R O");
                lcd.setCursor(0, 1);
                lcd.print(" - Celular:");
                lcd.setCursor(0, 2);
                // OBTENER CADENA DE APLICACIÓN -- Nombre
                bool seEnvioAlgo = false;
                int indiceNumero = 0;
                long int t0 = millis();
                long int t1 = millis();
          limpiarBuffer();
                LOOP {
                    // SI YA SE ENVIO ALGO DESDE LA APLICACION
                    while (Serial.available()) {
                        seEnvioAlgo = true;
                    //   RECIBIRLO
                        numero_temp[indiceNumero++] = Serial.read();
                    }
                    // CONTROLAR CUANTO HA PASADO DESDE QUE COME...
                    if (seEnvioAlgo) {
                        t1 = millis();
                        if (t1 - t0 >= 50) break;
                    } else {
                        t0 = millis();
                        t1 = millis();
                    }
                }
                lcd.print(numero_temp);
                lcd.setCursor(0, 3);
                lcd.print("Correcto?       ");
                delay(200);
              if (entradaAceptada()) {
                lcd.setCursor(0, 2);
                lcd.print(LINEA_VACIA);
                lcd.setCursor(0, 3);
                lcd.print(LINEA_VACIA);
                break;
                }
    
            }//---------------------------------------
      LOOP {
          //limpiarBuffer();
          //enviarConfirmar("contrasena");
              memset(contra_temp, 0, 11);    
                lcd.clear();
                lcd.print("R E G I S T R O");
                lcd.setCursor(0, 1);
                lcd.print(" - Contras:");
                lcd.setCursor(0, 2);
                // OBTENER CADENA DE APLICACIÓN -- Nombre
                bool seEnvioAlgo = false;
                int indiceContra = 0;
                long int t0 = millis();
                long int t1 = millis();
          limpiarBuffer();
                LOOP {
                    // SI YA SE ENVIO ALGO DESDE LA APLICACION
                    while (Serial.available()) {
                        seEnvioAlgo = true;
                    //   RECIBIRLO
                        contra_temp[indiceContra++] = Serial.read();
                    }
                    // CONTROLAR CUANTO HA PASADO DESDE QUE COME...
                    if (seEnvioAlgo) {
                        t1 = millis();
                        if (t1 - t0 >= 50) break;
                    } else {
                        t0 = millis();
                        t1 = millis();
                    }
                }
    imprimirAsteriscos(contra_temp);
                lcd.setCursor(0, 3);
                lcd.print("Correcto?       ");
                delay(200);
              if (entradaAceptada()) {
                lcd.setCursor(0, 2);
                lcd.print(LINEA_VACIA);
                lcd.setCursor(0, 3);
                lcd.print(LINEA_VACIA);
                break;
                }
    
            }//************
      //enviarConfirmar("NADA");
      //contra_temp=cifrarXOR(contra_temp, contra_temp);
      memcpy(nuevo_usuario.nombre, nombre_temp, 11);
      memcpy(nuevo_usuario.numero, numero_temp, 9);
      memcpy(nuevo_usuario.contra, contra_temp, 11);
      // LEER EEPROM
      byte usuarios = 0;
      EEPROM.get(0, usuarios);
      int siguiente_direccion = OFFSET_USUARIOS;
      for (int i = 0; i < usuarios; i++) {
          siguiente_direccion += sizeof(struct usuario);
      }
      EEPROM.put(siguiente_direccion, nuevo_usuario);
      usuarios++;
      EEPROM.put(0, usuarios);
      estado_actual = MENU;
      lcd.clear();
       break;                         
        }                                 
        
    }
    delay(10);
  }//**********************************************segunda parte menu admin*******************************
  lcd.clear();
  LOOP {
    delay(1);
    if (digitalRead(CANCELAR)) {
      opcion = 0;
      Menu();
    }
    char tecla = getTeclado();
    if (tecla == '2') {
      if (opcion < 1) {
        opcion = 3;
      } else {
        opcion--;
      }
      lcd.clear();
    } else if (tecla == '8') {
      if (opcion > 2) {
        opcion = 0;
      } else {
        opcion++;
      }
      lcd.clear();
    }
    if (opcion == 0) {
      imprimirEnPantalla(">> ingreso cel", 0, 0);
      imprimirEnPantalla("   retiro cel", 0, 1);
      imprimirEnPantalla("   cerrar sesion", 0, 2);
      imprimirEnPantalla("   eliminar acc", 0, 3);
    } else if (opcion == 1) {
      imprimirEnPantalla("   ingreso cel", 0, 0);
      imprimirEnPantalla(">> retiro cel", 0, 1);
      imprimirEnPantalla("   cerrar sesion", 0, 2);
      imprimirEnPantalla("   eliminar acc", 0, 3);
    }  else if (opcion == 2) {
      imprimirEnPantalla("   ingreso cel", 0, 0);
      imprimirEnPantalla("   retiro cel", 0, 1);
      imprimirEnPantalla(">> cerrar sesion", 0, 2);
      imprimirEnPantalla("   eliminar acc", 0, 3);
    }  else if (opcion == 3) {
      imprimirEnPantalla("   ingreso cel", 0, 0);
      imprimirEnPantalla("   retiro cel", 0, 1);
      imprimirEnPantalla("   cerrar sesion", 0, 2);
      imprimirEnPantalla(">> eliminar acc", 0, 3);
    }
    Serial.println(opcion);
  }
}

void MenuAdmin() {
  // TODO: implementar menu admin
  // por medio de la aplicacion Bluetooth.
  lcd.clear();
  LOOP {
    delay(1);
    if (digitalRead(CANCELAR)) {
      opcion = 0;
      Menu();
    }
    char tecla = getTeclado();
    if (tecla == '2') {
      if (opcion < 1) {
        opcion = 1;
      } else {
        opcion--;
      }
      lcd.clear();
    } else if (tecla == '8') {
      if (opcion > 0) {
        opcion = 0;
      } else {
        opcion++;
      }
      lcd.clear();
    }
    if (opcion == 0) {
      imprimirEnPantalla(">> ver logs", 0, 0);
      imprimirEnPantalla("   estado sist.", 0, 1);
    } else if (opcion == 1) {
      imprimirEnPantalla("   ver logs", 0, 0);
      imprimirEnPantalla(">> estado sist.", 0, 1);
    }
    Serial.println(opcion);
  }
}

void pintarEnPantalla(String array[]) {
  lcd.clear();
  for (int i = 0; i < array->length(); i++) {
    imprimirEnPantalla(array[i], 0, i);
  }
}

void imprimirEnPantalla(String texto, int x, int y) {
  lcd.clear();
  lcd.setCursor(x, y);
  lcd.print(texto);
  delay(400);
}

char getTeclado() {
  char key = pad.getKey();
  if (key != NO_KEY) {
    Serial.println(key);
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

    Serial.print(tecla);

    MostrarDigito((tecla));
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

bool isUserByNameAndPassword(String name, String password) {
   int value = EEPROM.read(0);
  User temp;
  for (int i = 0; i < value; i++) {
    EEPROM.get(sizeof(int)+ sizeof(User)*i, temp);
    if (temp.name == name && password == descifrarXOR(temp.name, password)) {
      return true;
    }
  }
  return false;
}

User* getUserByNameAndPassword(String name, String password) {
   int value = EEPROM.read(0);
  User temp;
  for (int i = 0; i < value; i++) {
    EEPROM.get(sizeof(int)+ sizeof(User)*i, temp);
    if (temp.name == name && password == descifrarXOR(temp.name, password)) {
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

String cifrarXOR(char mensaje[], char clave[], int longitud) {
  // Cifrado XOR byte a byte
  String mensajeCifrado = "";
  for (int i = 0; i < longitud; i++) {
    char caracterCifrado = mensaje[i] ^ clave[i];
    mensajeCifrado += caracterCifrado;
  }
  
  return mensajeCifrado;
}
String cifrarXOR2(char mensaje[], char clave[], int longitud) {
  // Cifrado XOR byte a byte
  String mensajeCifrado = "";
  for (int i = 0; i < longitud; i++) {
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

void MostrarDigito(char numDecenas) {


  if (numDecenas == '0') {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, cero[k]);
    }
  } else if (numDecenas == '1') {
    Serial.println("Entro");
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, uno[k]);
    }
  } else if (numDecenas == '2') {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, dos[k]);
    }
  } else if (numDecenas == '3') {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, tres[k]);
    }
  } else if (numDecenas == '4') {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, cuatro[k]);
    }
  } else if (numDecenas == '5') {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, cinco[k]);
    }
  } else if (numDecenas == '6') {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, seis[k]);
    }
  } else if (numDecenas == '7') {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, siete[k]);
    }
  } else if (numDecenas == '8') {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, ocho[k]);
    }
  } else if (numDecenas == '9') {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, nueve[k]);
    }
  } else if (numDecenas == '*') {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, Astk[k]);
    }
  }else if (numDecenas == '#') {
    for (int k = 0; k < 8; k++) {
      matriz_driver.setRow(0, k, Numeral[k]);
    }
  }
  delay(50);

}//contador con dos digitos_________________________________
