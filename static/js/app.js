// Función para obtener datos del Arduino
async function fetchData() {
    const response = await fetch("/read_arduino");
    const data = await response.json();
    if (data.timestamp && data.data) {
        const table = document.getElementById("accessTable").getElementsByTagName("tbody")[0];
        const newRow = table.insertRow();
        const cell1 = newRow.insertCell(0);
        const cell2 = newRow.insertCell(1);
        cell1.textContent = data.timestamp;
        cell2.textContent = data.data;
    }
}

async function generatePDF() {
    const response = await fetch("/generate_pdf", { method: "POST" });
    const data = await response.json();

    // Revisa la respuesta completa
    console.log(data);  // Esto te permitirá ver si los datos incluyen 'pdf_url'

    // Asegúrate de que 'data.pdf_url' existe
    if (data.pdf_url) {
        // Mostrar el mensaje en el modal
        const modalMessage = document.getElementById("modalMessage");
        modalMessage.style.display = "flex";  // Mostrar los elementos en fila (ícono + texto)

        // Agregar un check en verde
        const checkIcon = document.getElementById("checkIcon");
        checkIcon.innerHTML = "&#10004;";  // Check verde

        // Agregar el mensaje
        const messageText = document.getElementById("messageText");
        messageText.textContent = "PDF generado exitosamente.";

        // Mostrar el modal
        const modal = document.getElementById("pdfModal");
        modal.style.display = "block";

        // Configurar el enlace para abrir el PDF
        const openButton = document.getElementById("openPDFButton");
        openButton.href = data.pdf_url;  // Usamos la URL proporcionada por el servidor
    } else {
        console.error("No se recibió la URL del PDF correctamente.");
    }

    // Cerrar el modal cuando el usuario haga clic en la 'x'
    const closeButton = document.querySelector(".close-button");
    closeButton.onclick = function() {
        const modal = document.getElementById("pdfModal");
        modal.style.display = "none";
    };

    // Cerrar el modal cuando se hace clic fuera de él
    window.onclick = function(event) {
        const modal = document.getElementById("pdfModal");
        if (event.target == modal) {
            modal.style.display = "none";
        }
    };
}






// Llama a fetchData cada segundo para actualizar los datos en tiempo real
setInterval(fetchData, 1000);