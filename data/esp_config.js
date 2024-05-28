



document.addEventListener("DOMContentLoaded", function () {

  // ipEsp = document.getElementById("ipEsp").value;
  // const endpoint = `http://${ipEsp}`;

   
  var nomeESP = document.getElementById("nomeESP").value;
  var descricaoESP = document.getElementById("descricaoESP").value;
  var ipCentral = document.getElementById("ipCentral").value;


    
  
  fetchAndDisplayIP()
  .then(() => {
    ipEsp = document.getElementById("ipEsp").value;
    getESPConfig(ipEsp);
    //
  })
  .catch((error) => {
    console.error("Failed to fetch IP:", error);
  });

 

  document
    .getElementById("formConfigESP")
    .addEventListener("submit", function (event) {
      event.preventDefault();
      
      //const ipEsp = document.getElementById("ipEsp").value;
      nomeESP = document.getElementById("nomeESP").value;
      descricaoESP = document.getElementById("descricaoESP").value;
      ipCentral = document.getElementById("ipCentral").value;

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


async function getESPConfig(ipEsp) {
  try {
      const response = await axios.get(`http://${ipEsp}/esp`);
      const config = response.data;

      // Preencher os campos do formulário com os dados recebidos
      nomeESP.value = config.name;
      descricaoESP.value = config.description;
      ipCentral.value = config.centralIP;
  } catch (error) {
      console.error('Erro ao obter a configuração do ESP:', error);
  }
}

