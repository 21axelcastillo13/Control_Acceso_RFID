from flask import Flask, render_template, jsonify, request
import serial
from fpdf import FPDF
from datetime import datetime
import os
from PyPDF2 import PdfWriter, PdfReader

# Configuración del puerto serial
try:
    arduino = serial.Serial(port='COM4', baudrate=9600, timeout=1)
except serial.SerialException as e:
    print(f"Error al abrir el puerto COM4: {e}")
    arduino = None

# Variables globales
access_records = []
fecha_actual = datetime.now().strftime("%d-%m-%Y")

app = Flask(__name__)

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/read_arduino")
def read_arduino():
    if arduino and arduino.in_waiting > 0:
        try:
            data = arduino.readline().decode().strip()
            if data:
                timestamp = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
                access_records.append((timestamp, data))
                return jsonify({"timestamp": timestamp, "data": data})
        except serial.SerialException as e:
            print(f"Error de lectura: {e}")
            return jsonify({"error": "Error de lectura del puerto serial"})
    return jsonify({})


@app.route("/generate_pdf", methods=["POST"])
def generate_pdf():
    pdf_filename = os.path.join('static', 'reportes', f"reporte-{fecha_actual}.pdf")
    temp_pdf_filename = os.path.join('static', 'reportes', "temp.pdf")  # Ruta para el PDF temporal

    def estruct_pdf(pdf):
        pdf.add_page()
        pdf.image("./static/img/logo.png", x=10, y=8, w=33)
        pdf.ln(15)
        pdf.set_font("Arial", "B", 14)
        pdf.set_text_color(29, 25, 52)
        pdf.cell(190, 10, "Registro de Acceso", 0, align="C")
        pdf.ln(10)

        ancho_total = 190
        ancho_columna_1 = ancho_total * 0.1
        ancho_columna_2 = ancho_total * 0.9

        pdf.set_font("Arial", "B", 12)
        pdf.cell(15, 10, "Fecha: ")
        pdf.cell(30, 10, fecha_actual)
        pdf.ln()

        pdf.set_font("Arial", "", 10)
        pdf.set_text_color(255, 255, 255)
        pdf.set_fill_color(0, 130, 202)
        pdf.cell(ancho_columna_1, 10, "Hora", border=0, align="C",fill=True)
        pdf.cell(ancho_columna_2, 10, "Datos de Acceso", border=0, align="C",fill=True)
        pdf.ln()

        pdf.set_font("Arial", "", 10)
        pdf.set_text_color(0,0,0)
         # Alternar el color de las filas
        for index, record in enumerate(access_records):
            if index % 2 == 0:  # Índices pares (0, 2, 4, ...) son grises
                pdf.set_fill_color(244, 244, 244)  # Color gris claro
            else:  # Índices impares (1, 3, 5, ...) son blancos
                pdf.set_fill_color(255, 255, 255)  # Color blanco

            pdf.cell(ancho_columna_1, 10, record[0].split(" ")[1], border=0, ln=0, align="C", fill=True)
            pdf.cell(ancho_columna_2, 10, record[1], border=0, fill=True)
            pdf.ln()
    
    if os.path.exists(pdf_filename):
        pdf_writer = PdfWriter()
        pdf_reader = PdfReader(pdf_filename)
        for page in pdf_reader.pages:
            pdf_writer.add_page(page)

        pdf = FPDF()
        estruct_pdf(pdf)
        pdf.output(temp_pdf_filename)  # Guarda el PDF temporal en la carpeta correcta
        temp_reader = PdfReader(temp_pdf_filename)

        for page in temp_reader.pages:
            pdf_writer.add_page(page)

        with open(pdf_filename, "wb") as output_pdf:
            pdf_writer.write(output_pdf)
        #print("Datos añadidos al PDF existente.")
        return jsonify({
            "message": "Se han guardado los cambios", 
            "filename": pdf_filename,
            "pdf_url": f'/static/reportes/reporte-{fecha_actual}.pdf'  # Asegúrate de que la URL se construya correctamente
        })
    else:
        pdf = FPDF()
        
        estruct_pdf(pdf)
        pdf.output(pdf_filename)
        #print("PDF generado exitosamente")
        return jsonify({
            "message": "PDF generado exitosamente", 
            "filename": f"reporte-{fecha_actual}.pdf", 
            "pdf_url": f'/static/reportes/reporte-{fecha_actual}.pdf'  # URL correcta del archivo
        })



if __name__ == "__main__":
    app.run(host='127.0.0.1', port=5001)  # Cambie 5000 por 5001 ya que estaba utilizado y daba error en com4