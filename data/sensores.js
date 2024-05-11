

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
                        onclick='apagarSensorById(${sensor.Id})'>
                        Apagar</button>
                    </div>
            </div>
        </div>
    </div>
    `;
}



document.addEventListener("DOMContentLoaded", function () {
    fetchAndDisplayIP()
      .then(() => {
        const ipEsp = document.getElementById("ipEsp").value;
        const endpoint = `http://${ipEsp}/sensors`;
        setInterval(() => atualizarDadosSensores(endpoint), 2000);
      })
      .catch((error) => {
        console.error("Failed to fetch IP:", error);
      });

  // const ipEsp = document.getElementById("ipEsp").value;
  // const endpoint = `http://${ipEsp}/sensors`;
  // setInterval(() => atualizarDadosSensores(endpoint), 2000);

  // Chama a função para atualizar os dados dos sensores a cada segundo

  const idDispositivo = 1;
  const formSensor = document.getElementById("formSensor");
  const btnApagarSensores = document.getElementById("cleanSensorData");

  formSensor.addEventListener("submit", function (e) {
    e.preventDefault(); // Impede o comportamento padrão de envio do formulário
    const ipEsp = document.getElementById("ipEsp").value;
    const endpointEsp = `http://${ipEsp}/addSensor`;
    // Captura os dados do formulário
    const nomeSensor = document.getElementById("nomeSensor").value;
    const pinSensor = document.getElementById("pinSensor").value;
    //obter texto da unidade selecionada
    const idUnidade =
      document.getElementById("idUnidade").options[
        document.getElementById("idUnidade").selectedIndex
      ].text;

    console.log("idUnidade-->" + idUnidade);
    const tipo = document.getElementById("tipoSensor").value;
    const modoOperacao =
      document.getElementById("modoOperacao").options[
        document.getElementById("modoOperacao").selectedIndex
      ].text;

    // Prepara o objeto com os dados
    const dadosSensor = {
      id: -1,
      nome: nomeSensor,
      tipo: tipo,
      pin: pinSensor,
      modoOperacao: modoOperacao,
      valor: 0,
      dtCriacao: new Date(),
      dispositivoId: idDispositivo,
      unidadeId: idUnidade ? idUnidade : null,
    };

    jsonString = JSON.stringify(dadosSensor);
    console.log(endpointEsp);

    // Envia a requisição POST
    axios
      .post(endpointEsp, dadosSensor, {
        headers: { "Content-Type": "application/json" },
      })
      .then((response) => {
        console.log("Sensor adicionado com sucesso:", response.data);
        // Aqui você pode adicionar alguma ação após o sucesso, como recarregar a lista de sensores
      })
      .catch((error) => {
        console.error("Erro ao adicionar o sensor:", error);
      });
  });

  btnApagarSensores.addEventListener("click", function (e) {
    e.preventDefault(); // Impede o comportamento padrão de envio do formulário
    const endpointEsp = `http://${ipEsp}/clearSensorData`;
    // Envia a requisição POST
    axios
      .delete(endpointEsp, { headers: { "Content-Type": "application/json" } })
      .then((response) => {
        console.log("Dados dos sensores apagados com sucesso:", response.data);
        // Aqui você pode adicionar alguma ação após o sucesso, como recarregar a lista de sensores
      })
      .catch((error) => {
        console.error("Erro ao apagar os dados dos sensores:", error);
      });
  });
});

// function abrirModalEdicao(sensorId, nomeSensor, pinSensor, unidade, modoOperacao, tipo) {
//     // Preenche os campos do formulário com os dados atuais do sensor
//     document.getElementById('editarNomeSensor').value = nomeSensor;
//     document.getElementById('editarPinSensor').value = pinSensor;
//     document.getElementById('editarUnidade').value = unidade;
//     document.getElementById('editarTipo').value = tipo;
//     document.getElementById('editarModoOperacao').value = modoOperacao;

//     // Abre o modal
//     $('#modalEditarSensor').modal('show');

//     document.getElementById('salvarEdicao').addEventListener('click', function() {
//         const dadosAtualizados = {
//             nome: document.getElementById('editarNomeSensor').value,
//             pin: document.getElementById('editarPinSensor').value,
//             unidade: document.getElementById('editarUnidade').value,
//             tipo: document.getElementById('editarTipo').value,
//             modoOperacao: document.getElementById('editarModoOperacao').value
//         };

//         // Chama a função para atualizar o sensor
//         atualizarSensor(sensorId, dadosAtualizados);

//         // Fecha o modal após a atualização
//         $('#modalEditarSensor').modal('hide');
//     });

//     document.getElementById('excluirSensor').addEventListener('click', function() {
//         const sensorId = document.getElementById('editarSensorId').value;
//         // Chama a função para excluir o sensor
//         excluirSensor(sensorId);

//         // Fecha o modal após a exclusão
//         $('#modalEditarSensor').modal('hide');
//     });

// }

function apagarSensorById(sensorId) {
  const ipEsp = document.getElementById("ipEsp").value;
  const endpointEsp = `http://${ipEsp}/deleteSensor?id=${sensorId}`;
  // Envia a requisição DELETE
  axios
    .delete(endpointEsp, { headers: { "Content-Type": "application/json" } })
    .then((response) => {
      console.log("Sensor excluído com sucesso:", response.data);
      // Aqui você pode adicionar alguma ação após o sucesso, como recarregar a lista de sensores
    })
    .catch((error) => {
      console.error("Erro ao excluir o sensor:", error);
    });
}

function atualizarSensor(sensorId, dadosAtualizados) {
  // Endpoint para atualizar um sensor (substitua pelo seu)
  console.log("sensorId-->" + sensorId);
  const ipEsp = document.getElementById("ipEsp").value;
  const endpoint = `http://${ipEsp}/sensors/${sensorId}`;

  // Envia a requisição PUT
  axios
    .put(endpoint, dadosAtualizados)
    .then((response) => {
      console.log("Sensor atualizado com sucesso:", response.data);
      // Aqui você pode adicionar alguma ação após o sucesso, como recarregar a lista de sensores
    })
    .catch((error) => {
      console.error("Erro ao atualizar o sensor:", error);
    });
}

function excluirSensor(sensorId) {
  // Endpoint para excluir um sensor (substitua pelo seu)
  const endpoint = `http://localhost:8080/smartlab/api/sensores/${sensorId}`;

  // Envia a requisição DELETE
  axios
    .delete(endpoint)
    .then((response) => {
      console.log("Sensor excluído com sucesso:", response.data);
      // Aqui você pode adicionar alguma ação após o sucesso, como recarregar a lista de sensores
    })
    .catch((error) => {
      console.error("Erro ao excluir o sensor:", error);
    });
}

function atualizarDadosSensores(endpoint) {
  axios
    .get(endpoint)
    .then(function (response) {
      const sensores = response.data; // Assume que a resposta é diretamente uma lista de sensores
      console.log(sensores);
      const container = document.querySelector(".row"); // Seleciona o elemento onde os cards devem ser inseridos
      container.innerHTML = ""; // Limpa o conteúdo atual

      // Para cada sensor recebido, cria um card e adiciona ao container
      sensores.forEach((sensor) => {
        console.log(sensor);
        container.innerHTML += criarCardSensor(sensor);
      });
    })
    .catch(function (error) {
      console.error("Erro ao obter os sensores:", error);
    });
}

// Exemplo de função para buscar unidades e preencher as opções do dropdown
function getUnidades() {
  const endpointUnidades = `http://localhost:8080/smartlab/api/unidades`; // Substitua pela URL da sua API
  axios
    .get(endpointUnidades)
    .then(function (response) {
      const unidades = response.data;
      const dropdowns = document.querySelectorAll(".unidade-dropdown");
      dropdowns.forEach((dropdown) => {
        console.log("ola");
        dropdown.innerHTML = unidades
          .map(
            (unidade) =>
              `<option value="${unidade.id}">${unidade.descricao} - ${unidade.simbolo}</option>`
          )
          .join("");
      });
    })
    .catch(function (error) {
      console.error("Erro ao obter unidades:", error);
    });
}


