
ipEsp = "";


document.addEventListener("DOMContentLoaded", function () {

  fetchAndDisplayIP()
  .then(() => {
    ipEsp = document.getElementById("ipEsp").value;
    //const endpoint = `http://${ipEsp}`;
    
  })
  .catch((error) => {
    console.error("Failed to fetch IP:", error);
  });

  document
    .getElementById("formConfigESP")
    .addEventListener("submit", function (event) {
      event.preventDefault();
      
      //const ipEsp = document.getElementById("ipEsp").value;
      
      const nomeESP = document.getElementById("nomeESP").value;
      const descricaoESP = document.getElementById("descricaoESP").value;
      const ipCentral = document.getElementById("ipCentral").value;

      axios
        .post(`http://${ipEsp}/esp/config`, {
          nome: nomeESP,
          descricao: descricaoESP,
          ipCentral: ipCentral,
        })
        .then(function (response) {
          console.log(response.data);
          alert("Configuração salva com sucesso!");
        })
        .catch(function (error) {
          console.error("Erro ao salvar configuração:", error);
          alert("Erro ao salvar configuração.");
        });
    });
});
