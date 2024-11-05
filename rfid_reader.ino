#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);

byte LecturaUID[4];
byte Angel[4] = {0x35, 0x82, 0xE4, 0xC5};    
byte Axel[4] = {0x1E, 0x4F, 0xD3, 0x18}; 
byte Francisco[4] = {0x83, 0x6A, 0x95, 0xEC};  
byte Fabiana[4] = {0xF5, 0xCA, 0x65, 0x76};//// 
byte Arnold[4] = {0xB3, 0x8D, 0x83, 0x0D};  
byte Aleman[4] = {0xE3, 0x2F, 0x7D, 0xFA};  



const int cantidadEjemplo=10;
// Arreglo para almacenar los usuarios dentro y su tiempo de entrada
String usuariosDentro[cantidadEjemplo];
unsigned long tiempoEntrada[cantidadEjemplo]; // Almacena el tiempo de entrada de cada usuario
int cantidadUsuariosDentro = 0;

const unsigned long TIEMPO_ESPERA = 10000; // 10 segundos de espera

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent())   
    return;

  if (!mfrc522.PICC_ReadCardSerial()) 
    return;

  // Leer UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    LecturaUID[i] = mfrc522.uid.uidByte[i];
    //Serial.print(LecturaUID[i], HEX); // Imprimir en formato hexadecimal
    //Serial.print(" ");
  }
  //Serial.println();

  // Identificar el nombre del usuario basado en el UID
  String nombre = "";

  if (comparaUID(LecturaUID, Angel)) {
      nombre = "Angel Carranza";
  } else if (comparaUID(LecturaUID, Axel)) {
      nombre = "Axel Castillo";
  } else if (comparaUID(LecturaUID, Arnold)) {
      nombre = "Arnold Mejia";
  } else if (comparaUID(LecturaUID, Francisco)) {
      nombre = "Francisco Villanueva";
  } else if (comparaUID(LecturaUID, Fabiana)) {
      nombre = "Fabiana Guzman";
  } else if (comparaUID(LecturaUID, Aleman)) {
      nombre = "Angel Aleman";
  }

  if (nombre != "") {
      // Verificar si el usuario ya está dentro
      if (estaDentro(nombre)) {
          if (puedeSalir(nombre)) {
              Serial.print("Salida: ");
              Serial.println(nombre);
              quitarUsuario(nombre);
          } else {
              //Serial.println("Debe esperar 10 segundos antes de poder marcar la salida.");
          }
      } else {
          Serial.print("Entrada: ");
          Serial.println(nombre);
          agregarUsuario(nombre);
      }
  } else {
      Serial.println("Acceso denegado: usuario no registrado en el sistema");
  }

  mfrc522.PICC_HaltA();  
}

boolean comparaUID(byte lectura[], byte usuario[]) {
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (lectura[i] != usuario[i])
      return false;  
  }
  return true;
}

bool estaDentro(String nombre) {
    for (int i = 0; i < cantidadUsuariosDentro; i++) {
        if (usuariosDentro[i] == nombre) {
            return true;
        }
    }
    return false;
}

void agregarUsuario(String nombre) {
    if (cantidadUsuariosDentro < cantidadEjemplo) {  // Limitar a 10 usuarios
        usuariosDentro[cantidadUsuariosDentro] = nombre;
        tiempoEntrada[cantidadUsuariosDentro] = millis(); // Guardar tiempo de entrada
        cantidadUsuariosDentro++;
    }
} 

bool puedeSalir(String nombre) {
    for (int i = 0; i < cantidadUsuariosDentro; i++) {
        if (usuariosDentro[i] == nombre) {
            unsigned long tiempoActual = millis();
            unsigned long tiempoTranscurrido = tiempoActual - tiempoEntrada[i];
            return tiempoTranscurrido >= TIEMPO_ESPERA; // Verificar si han pasado 10 segundos
        }
    }
    return false;
}

void quitarUsuario(String nombre) {
    for (int i = 0; i < cantidadUsuariosDentro; i++) {
        if (usuariosDentro[i] == nombre) {
            // Desplazar usuarios hacia la izquierda
            for (int j = i; j < cantidadUsuariosDentro - 1; j++) {
                usuariosDentro[j] = usuariosDentro[j + 1];
                tiempoEntrada[j] = tiempoEntrada[j + 1];
            }
            cantidadUsuariosDentro--;
            break;
        }
    }
}
