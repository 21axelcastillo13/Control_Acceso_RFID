#Proyecto Control de acceso con Arduino y Flask

Este proyecto utiliza un módulo RFID MFRC522 para leer identificaciones de tarjetas y un servidor Flask para gestionar los registros de acceso y generar informes en PDF.

#Requisitos de Hardware
- **Arduino Uno** (o compatible)
- **Módulo RFID MFRC522**
- **Protoboard** (Opcional)
- **Cables de conexión**
- **Tarjetas RFID** (con los UIDs especificados en el código)

### Conexiones del Módulo RFID
- Conectar el módulo MFRC522 al Arduino de la siguiente manera:
  - SDA (pin10)
  - SCK (pin 13)
  - MOSI (pin 11)
  - MISO (pin 12)
  - IRQ (no se conecta)
  - GND (pin GND)
  - RST (pin 9)
  - 3.3V (pin 3.3V)

## Requisitos de Software

### Para el Proyecto de Python (Flask)

Este proyecto utiliza las siguientes librerías de Python:

- **Flask**: Framework.
- **PySerial**: Comunicación con el puerto de Arduino.
- **FPDF**: Para generar los PDF.
- **PyPDF2**: Para manipulación de archivos PDF existentes.
- **datetime**: Para manejo de fechas y horas.

#### Instalación de Dependencias

Puedes instalar las dependencias de Python utilizando pip:

```bash
pip install Flask
pip install pyserial
pip install fpdf
pip install PyPDF2