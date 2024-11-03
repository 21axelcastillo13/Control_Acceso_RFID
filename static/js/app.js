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

// Función para generar PDF
async function generatePDF() {
    const response = await fetch("/generate_pdf", { method: "POST" });
    const data = await response.json();

    // Mostrar el mensaje en el modal
    document.getElementById("modalMessage").textContent = data.message;
    const modal = document.getElementById("pdfModal");
    modal.style.display = "block"; // Mostrar el modal

    // Cerrar el modal cuando el usuario haga clic en la 'x'
    const closeButton = document.querySelector(".close-button");
    closeButton.onclick = function() {
        modal.style.display = "none";
    }

    // Cerrar el modal cuando se hace clic fuera de él
    window.onclick = function(event) {
        if (event.target == modal) {
            modal.style.display = "none";
        }
    }
}


// Llama a fetchData cada segundo para actualizar los datos en tiempo real
setInterval(fetchData, 1000);