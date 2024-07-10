document.addEventListener("DOMContentLoaded", function () {
  fetchAndDisplayIP()
      .then(() => {
          const ipEsp = document.getElementById("ipEsp").value;
          const endpointSensores = `http://${ipEsp}/sensors`;
          const endpointAtuadores = `http://${ipEsp}/atuadores`;
          setInterval(() => atualizarDados(endpointSensores, 'sensor'), 2000);
          setInterval(() => atualizarDados(endpointAtuadores, 'atuador'), 2000);
      })
      .catch((error) => {
          console.error("Failed to fetch IP:", error);
      });

  // const ipEsp = document.getElementById("ipEsp").value;
  // const endpointSensores = `http://${ipEsp}/sensors`;
  // const endpointAtuadores = `http://${ipEsp}/atuadores`;
  // setInterval(() => atualizarDados(endpointSensores, "sensor"), 3000);
  // setInterval(() => atualizarDados(endpointAtuadores, "atuador"), 3000);

  const idDispositivo = 1;
  const formDispositivo = document.getElementById("formDispositivo");

  formDispositivo.addEventListener("submit", function (e) {
    e.preventDefault(); // Impede o comportamento padrão de envio do formulário
    const ipEsp = document.getElementById("ipEsp").value;
    const tipoDispositivo = document.getElementById("tipoDispositivo").value;
    const endpointEsp =`http://${ipEsp}/addDevice`;
    // Captura os dados do formulário
    const nomeDispositivo = document.getElementById("nomeDispositivo").value;
    const pinDispositivo = document.getElementById("pinDispositivo").value;
    const idUnidade =
      document.getElementById("idUnidade").options[
        document.getElementById("idUnidade").selectedIndex
      ].text;
    const tipo = document.getElementById("tipoDispositivoDetalhe").value;
    const modoOperacao =
      document.getElementById("modoOperacao").options[
        document.getElementById("modoOperacao").selectedIndex
      ].text;

    // Prepara o JSON com os dados
    const dadosDispositivo = {
      id: -1,
      nome: nomeDispositivo,
	  tipoDispositivo: tipoDispositivo,
      tipo: tipo,
      pin: pinDispositivo,
      modoOperacao: modoOperacao,
      valor: 0,
      dtCriacao: new Date(),
      dispositivoId: idDispositivo,
      unidade: idUnidade ? idUnidade : null,
    };

    jsonString = JSON.stringify(dadosDispositivo);
    console.log(endpointEsp);
    // Enviar peido POST
    axios
      .post(endpointEsp, dadosDispositivo, {
        headers: { "Content-Type": "application/json" },
      })
      .then((response) => {
        alert("Dispositivo adicionado com sucesso:", response.data);
      })
      .catch((error) => {
        alert(error.response.data);
      });
  });

});

// Função para criar um card para cada sensor
function criarCardSensor(sensor) {
    return `
        <div class="col-sm-3">
            <div class="card text-black">
                <div class="card text-center">
                    <div class="card-header">
                        <p class="text-center">
                            <b>${sensor.Nome}</b>
                        </p>
                    </div>
                    <div class="card-body">
                        <b> ${sensor.Valor} ${sensor.Unidade} </b>
                    </div>
                    <div class="card-footer">
                        <p class="text-center"><b>Atualização</b>: ${sensor.DataUltimaObs}</p>
                        <button type="button" class="btn btn-primary btn-sm" 
                            onclick='removeDispositivo("sensor", ${sensor.Id})'>
                            Apagar</button>
                    </div>
                </div>
            </div>
        </div>
    `;
}

// Função para criar um card para cada atuador
function criarCardAtuador(atuador) {
    return `
        <div class="col-sm-3">
            <div class="card text-black">
                <div class="card text-center">
                    <div class="card-header">
                        <p class="text-center">
                            <b>${atuador.Nome}</b>
                        </p>
                    </div>
                    <div class="card-body">
                        <b> ${atuador.Valor} ${atuador.Unidade} </b>
                    </div>
                    <div class="card-footer">
                        <p class="text-center"><b>Atualização</b>: ${atuador.DataUltimaObs}</p>
                        <button type="button" class="btn btn-primary btn-sm" 
                            onclick='removeDispositivo("atuador", ${atuador.Id})'>
                            Apagar</button>
                        <button type="button" class="btn btn-${atuador.Valor ? "success" : "danger"} btn-sm"
                            onclick='toggleAtuador("${atuador.Pin}", "${atuador.Valor ? 0 : 1}", "${atuador.ModoOperacao}")'>
                            ${atuador.Valor ? "Ligado" : "Desligado"}
                        </button>
                    </div>
                </div>
            </div>
        </div>
    `;
}

function removeDispositivo(tipo, dispositivoId) {
    const ipEsp = document.getElementById("ipEsp").value;
    const endpointEsp = tipo === 'sensor' ? `http://${ipEsp}/deleteSensor?id=${dispositivoId}` : `http://${ipEsp}/deleteAtuador?id=${dispositivoId}`;
    // Envia a requisição DELETE
    axios.delete(endpointEsp, { headers: { "Content-Type": "application/json" } })
    .then((response) => {
        console.log(`${tipo.charAt(0).toUpperCase() + tipo.slice(1)} excluído com sucesso:`, response.data);
        // Atualiza a lista de dispositivos
        tipo === 'sensor' ? getSensores() : getAtuadores();
    })
    .catch((error) => {
        console.error(`Erro ao excluir o ${tipo}:`, error);
    });
}

function atualizarDados(endpoint, tipo) {
  axios
    .get(endpoint)
    .then(function (response) {
      const dispositivos = response.data; // Assume que a resposta é diretamente uma lista de dispositivos
      console.log(dispositivos);
      const container = document.querySelector(`.row#${tipo}es`); // Seleciona o elemento onde os cards devem ser inseridos
      container.innerHTML = ""; // Limpa o conteúdo atual

      // Para cada dispositivo recebido, cria um card e adiciona ao container
      dispositivos.forEach((dispositivo) => {
        console.log(dispositivo);
        container.innerHTML +=
          tipo === "sensor"
            ? criarCardSensor(dispositivo)
            : criarCardAtuador(dispositivo);
      });
    })
    .catch(function (error) {
      console.error(`Erro ao obter os ${tipo}s:`, error);
    });
}

function toggleAtuador(pin, valor, modoOperacao) {
  const ipEsp = document.getElementById("ipEsp").value;
  axios
    .post(`http://${ipEsp}/toggleAtuador`, null, {
      params: {
        pin: pin,
        valor: valor,
        modoOperacao: modoOperacao,
      },
    })
    .then((response) => {
      alert("Estado do atuador alterado com sucesso!");
      getAtuadores();
    })
    .catch((error) => {
      console.error("Erro ao alternar estado do atuador:", error);
    });
}

function fetchAndDisplayIP() {
    return new Promise((resolve, reject) => {
        axios.get('/ip')
        .then(response => {
            document.getElementById('ipEsp').value = response.data;
            resolve(response.data);
        })
        .catch(error => {
            reject(error);
        });
    });
}

// Exemplo de função para buscar unidades e preencher as opções do dropdown
// function getUnidades() {
//   const endpointUnidades = `http://localhost:8080/smartlab/api/unidades`; // Substitua pela URL da sua API
//   axios
//     .get(endpointUnidades)
//     .then(function (response) {
//       const unidades = response.data;
//       const dropdowns = document.querySelectorAll(".unidade-dropdown");
//       dropdowns.forEach((dropdown) => {
//         dropdown.innerHTML = unidades
//           .map(
//             (unidade) =>
//               `<option value="${unidade.id}">${unidade.descricao} - ${unidade.simbolo}</option>`
//           )
//           .join("");
//       });
//     })
//     .catch(function (error) {
//       console.error("Erro ao obter unidades:", error);
//     });
// }
