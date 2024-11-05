#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define RST_PIN 9
#define SS_PIN 10
#define BUZZER_PIN 3

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Cambia la dirección si es necesario

byte LecturaUID[4];
byte Angel[4] = {0x35, 0x82, 0xE4, 0xC5};    
byte Axel[4] = {0x1E, 0x4F, 0xD3, 0x18}; 
byte Francisco[4] = {0x83, 0x6A, 0x95, 0xEC};  
byte Fabiana[4] = {0xF5, 0xCA, 0x65, 0x76};
byte Arnold[4] = {0xB3, 0x8D, 0x83, 0x0D};  
byte Aleman[4] = {0xE3, 0x2F, 0x7D, 0xFA};

const int cantidadEjemplo = 10;
String usuariosDentro[cantidadEjemplo];
unsigned long tiempoEntrada[cantidadEjemplo];
int cantidadUsuariosDentro = 0;

const unsigned long TIEMPO_ESPERA = 10000; // 10 segundos de espera
const unsigned long TIEMPO_LIMPIEZA = 3000; // 3 segundos para limpiar la pantalla
unsigned long tiempoMostrar; // Tiempo en que se mostró el mensaje
bool limpiarPantalla = false; // Estado para controlar la limpieza de pantalla

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  if (limpiarPantalla) {
    if (millis() - tiempoMostrar >= TIEMPO_LIMPIEZA) {
      lcd.clear(); // Limpiar la pantalla
      limpiarPantalla = false; // Resetear el estado
    }
  }

  if (!mfrc522.PICC_IsNewCardPresent())   
    return;

  if (!mfrc522.PICC_ReadCardSerial()) 
    return;

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    LecturaUID[i] = mfrc522.uid.uidByte[i];
  }

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
    sonidoConfirmacion();
    lcd.clear();
    if (estaDentro(nombre)) {
        if (puedeSalir(nombre)) {
            Serial.print("Salida: ");
            Serial.println(nombre);
            lcd.setCursor(0, 0);
            lcd.print("Salida: ");
            lcd.setCursor(0, 1);
            lcd.print(nombre);
            quitarUsuario(nombre);
        } else {
            lcd.setCursor(0, 0);
            lcd.print("No puedes salir");
            lcd.setCursor(0, 1);
            lcd.print("Por favor,espera");
            delay(2000); 
            lcd.clear();
                }
    } else {
        Serial.print("Entrada: ");
        Serial.println(nombre);
        lcd.setCursor(0, 0);
        lcd.print("Entrada: ");
        lcd.setCursor(0, 1);
        lcd.print(nombre);
        agregarUsuario(nombre);
    }
    tiempoMostrar = millis(); // Iniciar temporizador para limpiar pantalla
    limpiarPantalla = true; // Activar limpieza de pantalla
  } else {
    sonidoError();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Acceso denegado");
    Serial.println("Acceso denegado: usuario no registrado en el sistema");
    tiempoMostrar = millis(); // Iniciar temporizador para limpiar pantalla
    limpiarPantalla = true; // Activar limpieza de pantalla
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
    if (cantidadUsuariosDentro < cantidadEjemplo) {
        usuariosDentro[cantidadUsuariosDentro] = nombre;
        tiempoEntrada[cantidadUsuariosDentro] = millis();
        cantidadUsuariosDentro++;
    }
} 

bool puedeSalir(String nombre) {
    for (int i = 0; i < cantidadUsuariosDentro; i++) {
        if (usuariosDentro[i] == nombre) {
            unsigned long tiempoActual = millis();
            unsigned long tiempoTranscurrido = tiempoActual - tiempoEntrada[i];
            return tiempoTranscurrido >= TIEMPO_ESPERA;
        }
    }
    return false;
}

void quitarUsuario(String nombre) {
    for (int i = 0; i < cantidadUsuariosDentro; i++) {
        if (usuariosDentro[i] == nombre) {
            for (int j = i; j < cantidadUsuariosDentro - 1; j++) {
                usuariosDentro[j] = usuariosDentro[j + 1];
                tiempoEntrada[j] = tiempoEntrada[j + 1];
            }
            cantidadUsuariosDentro--;
            break;
        }
    }
}

void sonidoConfirmacion() {
  tone(BUZZER_PIN, 900, 200); 
  delay(200); 
}

void sonidoError() {
  tone(BUZZER_PIN, 500, 500); 
  delay(500);  
}
