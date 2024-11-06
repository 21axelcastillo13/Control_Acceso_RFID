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
byte Fabiana[4] = {0xF5, 0xCA, 0x65, 0x79};
byte Arnold[4] = {0xB3, 0x8D, 0x83, 0x0D};  
byte Aleman[4] = {0xE3, 0x2F, 0x7D, 0xFA};

const int cantidadEjemplo = 10;
String usuariosDentro[cantidadEjemplo];
unsigned long tiempoEntrada[cantidadEjemplo];
int cantidadUsuariosDentro = 0;

const unsigned long TIEMPO_ESPERA = 10000; // 10 segundos de espera
const unsigned long TIEMPO_LIMPIEZA = 3000; // 3 segundos para limpiar la pantalla
const unsigned long TIEMPO_INACTIVIDAD = 15000; // 15 segundos de inactividad para apagar pantalla

unsigned long tiempoMostrar;
unsigned long ultimoUso;
bool limpiarPantalla = false;
bool pantallaEncendida = true;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  mostrarMensajeInicio();
  pinMode(BUZZER_PIN, OUTPUT);
  ultimoUso = millis(); // Iniciar último uso al inicio
}

void loop() {
  // Control de apagado de backlight por inactividad
  if (pantallaEncendida && millis() - ultimoUso >= TIEMPO_INACTIVIDAD) {
    lcd.noBacklight(); // Apagar pantalla por inactividad
    pantallaEncendida = false;
  }

  // Si no hay actividad y no se ha pasado una tarjeta, muestra "Bienvenido UJCV"
  if (!limpiarPantalla && !mfrc522.PICC_IsNewCardPresent()) {
    mostrarMensajeInicio();
  }

  if (limpiarPantalla) {
    if (millis() - tiempoMostrar >= TIEMPO_LIMPIEZA) {
      lcd.clear();
      limpiarPantalla = false;
      mostrarMensajeInicio();
    }
  }

  if (!mfrc522.PICC_IsNewCardPresent())
    return;

  if (!mfrc522.PICC_ReadCardSerial())
    return;

  // Encender backlight si estaba apagado
  if (!pantallaEncendida) {
    lcd.backlight();
    pantallaEncendida = true;
  }

  ultimoUso = millis(); // Actualizar tiempo de última actividad

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
            lcd.print("Salida:");
            lcd.setCursor(0, 1);
            lcd.print(nombre);
            quitarUsuario(nombre);
            tiempoMostrar = millis();
            limpiarPantalla = true;
        } else {
            lcd.setCursor(0, 0);
            lcd.print("No puedes salir");
            lcd.setCursor(0, 1);
            lcd.print("  Favor espera  ");
            tiempoMostrar = millis();
            limpiarPantalla = true;
        }
    } else {
        Serial.print("Entrada: ");
        Serial.println(nombre);
        lcd.setCursor(0, 0);
        lcd.print("Entrada: ");
        lcd.setCursor(0, 1);
        lcd.print(nombre);
        agregarUsuario(nombre);
        tiempoMostrar = millis();
        limpiarPantalla = true;
    }
  } else {
    sonidoError();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("     Acceso     ");
    lcd.setCursor(1, 1);
    lcd.print("   Denegado    ");
    Serial.println("Acceso denegado: usuario no registrado en el sistema");
    tiempoMostrar = millis();
    limpiarPantalla = true;
  }

  mfrc522.PICC_HaltA();  
}

void mostrarMensajeInicio() {
  lcd.setCursor(0, 0);
  lcd.print(" Control Acceso ");
  lcd.setCursor(0, 1);
  lcd.print("      UJCV      ");
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
  tone(BUZZER_PIN, 1500, 200); 
  delay(200); 
}

void sonidoError() {
  tone(BUZZER_PIN, 100, 500); 
  delay(500);  
}
