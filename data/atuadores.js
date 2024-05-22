

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
                    <p class="text-center"><b>Atualização</b>: ${
                      atuador.DataUltimaObs
                    }</p>
                    <button type="button" class="btn btn-primary btn-sm" 
                        onclick='apagarAtuadorById(${atuador.Id})'>
                        Apagar</button>
                    </div>
                    <button type="button" class="btn btn-${
                      atuador.Valor ? "success" : "danger"
                    } btn-sm"
                        onclick='toggleAtuador("${atuador.Pin}", "${
    atuador.Valor ? 0 : 1
  }", "${atuador.ModoOperacao}")'>
                        ${atuador.Valor ? "Ligado" : "Desligado"}
                    </button>
                
            </div>
        </div>
    </div>
    `;
}

document.addEventListener("DOMContentLoaded", function () {
  const idDispositivo = 1; // Substitua pelo ID do dispositivo desejado
  // const ipEsp = document.getElementById("ipEsp").value;
  // const endpoint = `http://${ipEsp}/atuadores`;

  // setInterval(() => atualizarDadosAtuadores(endpoint), 2000);
  fetchAndDisplayIP()
    .then(() => {
      const ipEsp = document.getElementById("ipEsp").value;
      const endpoint = `http://${ipEsp}/atuadores`;
      setInterval(() => atualizarDadosAtuadores(endpoint), 2000);
    })
    .catch((error) => {
      console.error("Failed to fetch IP:", error);
    });

  const formAtuador = document.getElementById("formAtuador");
  const btnApagarAtuadores = document.getElementById("cleanAtuadorData");

  formAtuador.addEventListener("submit", function (e) {
    e.preventDefault(); // Impede o comportamento padrão de envio do formulário
    const ipEsp = document.getElementById("ipEsp").value;
    const endpointEsp = `http://${ipEsp}/atuadores`;
    // Captura os dados do formulário
    const nomeAtuador = document.getElementById("nomeAtuador").value;
    const pinAtuador = document.getElementById("pinAtuador").value;
    //obter texto da unidade selecionada
    const idUnidade =
      document.getElementById("idUnidade").options[
        document.getElementById("idUnidade").selectedIndex
      ].text;

    console.log("idUnidade-->" + idUnidade);
    const tipo = document.getElementById("tipoAtuador").value;
    const modoOperacao =
      document.getElementById("modoOperacao").options[
        document.getElementById("modoOperacao").selectedIndex
      ].text;

    // Prepara o objeto com os dados
    const dadosAtuador = {
      id: -1,
      nome: nomeAtuador,
      tipo: tipo,
      pin: pinAtuador,
      modoOperacao: modoOperacao,
      valor: 0,
      dtCriacao: new Date(),
      dispositivoId: idDispositivo,
      unidade: idUnidade ? idUnidade : null,
    };

    jsonString = JSON.stringify(dadosAtuador);
    console.log(endpointEsp);

    // Envia a requisição POST
    axios
      .post(endpointEsp, dadosAtuador, {
        headers: { "Content-Type": "application/json" },
      })
      .then((response) => {
        alert("Atuador adicionado com sucesso:", response.data);
        // Aqui você pode adicionar alguma ação após o sucesso, como recarregar a lista de atuadores
      })
      .catch((error) => {
        alert(error.response.data);
      });
  });

  btnApagarAtuadores.addEventListener("click", function (e) {
    e.preventDefault(); // Impede o comportamento padrão de envio do formulário
    const endpointEsp = `http://${ipEsp}/clearAtuadorData`;
    // Envia a requisição POST
    axios
      .delete(endpointEsp, { headers: { "Content-Type": "application/json" } })
      .then((response) => {
        console.log("Dados dos atuadores apagados com sucesso:", response.data);
        // Aqui você pode adicionar alguma ação após o sucesso, como recarregar a lista de atuadores
      })
      .catch((error) => {
        console.error("Erro ao apagar os dados dos atuadores:", error);
      });
  });
});

function abrirModalEdicao(
  atuadorId,
  nomeAtuador,
  pinAtuador,
  unidade,
  modoOperacao,
  tipo
) {
  // Preenche os campos do formulário com os dados atuais do atuador
  document.getElementById("editarNomeAtuador").value = nomeAtuador;
  document.getElementById("editarPinAtuador").value = pinAtuador;
  document.getElementById("editarUnidade").value = unidade;
  document.getElementById("editarTipo").value = tipo;
  document.getElementById("editarModoOperacao").value = modoOperacao;

  // Abre o modal
  $("#modalEditarAtuador").modal("show");

  document
    .getElementById("salvarEdicao")
    .addEventListener("click", function () {
      const dadosAtualizados = {
        nome: document.getElementById("editarNomeAtuador").value,
        pin: document.getElementById("editarPinAtuador").value,
        unidade: document.getElementById("editarUnidade").value,
        tipo: document.getElementById("editarTipo").value,
        modoOperacao: document.getElementById("editarModoOperacao").value,
      };

      // Chama a função para atualizar o atuador
      atualizarAtuador(atuadorId, dadosAtualizados);

      // Fecha o modal após a atualização
      $("#modalEditarAtuador").modal("hide");
    });

  document
    .getElementById("excluirAtuador")
    .addEventListener("click", function () {
      const atuadorId = document.getElementById("editarAtuadorId").value;
      // Chama a função para excluir o Atuador
      excluirAtuador(atuadorId);

      // Fecha o modal após a exclusão
      $("#modalEditarAtuador").modal("hide");
    });
}

function apagarAtuadorById(atuadorId) {
  const ipEsp = document.getElementById("ipEsp").value;
  const endpointEsp = `http://${ipEsp}/deleteAtuador?id=${atuadorId}`;
  // Envia a requisição DELETE
  axios
    .delete(endpointEsp, { headers: { "Content-Type": "application/json" } })
    .then((response) => {
      console.log("Atuador excluído com sucesso:", response.data);
      // Aqui você pode adicionar alguma ação após o sucesso, como recarregar a lista de atuadores
    })
    .catch((error) => {
      console.error("Erro ao excluir o atuador:", error);
    });
}

function atualizarAtuador(atuadorId, dadosAtualizados) {
  // Endpoint para atualizar um atuador (substitua pelo seu)
  console.log("atuadorId-->" + atuadorId);
  const ipEsp = document.getElementById("ipEsp").value;
  const endpoint = `http://${ipEsp}/atuadors/${atuadorId}`;

  // Envia a requisição PUT
  axios
    .put(endpoint, dadosAtualizados)
    .then((response) => {
      console.log("Atuador atualizado com sucesso:", response.data);
      // Aqui você pode adicionar alguma ação após o sucesso, como recarregar a lista de atuadores
    })
    .catch((error) => {
      console.error("Erro ao atualizar o atuador:", error);
    });
}

function excluirAtuador(atuadorId) {
  // Endpoint para excluir um atuador (substitua pelo seu)
  const endpoint = `http://localhost:8080/smartlab/api/atuadores/${atuadorId}`;

  // Envia a requisição DELETE
  axios
    .delete(endpoint)
    .then((response) => {
      console.log("Atuador excluído com sucesso:", response.data);
      // Aqui você pode adicionar alguma ação após o sucesso, como recarregar a lista de atuadores
    })
    .catch((error) => {
      console.error("Erro ao excluir o atuador:", error);
    });
}

function atualizarDadosAtuadores(endpoint) {
  axios
    .get(endpoint)
    .then(function (response) {
      const atuadores = response.data; // Assume que a resposta é diretamente uma lista de atuadores
      console.log(atuadores);
      const container = document.querySelector(".row"); // Seleciona o elemento onde os cards devem ser inseridos
      container.innerHTML = ""; // Limpa o conteúdo atual

      // Para cada atuador recebido, cria um card e adiciona ao container
      atuadores.forEach((atuador) => {
        console.log(atuador);
        container.innerHTML += criarCardAtuador(atuador);
      });
    })
    .catch(function (error) {
      console.error("Erro ao obter os atuadores:", error);
    });
}

function toggleAtuador(pin, valor, modoOperacao) {
  const ipEsp = document.getElementById("ipEsp").value;
  const endpointEsp = `http://${ipEsp}/toggleAtuador?pin=${pin}&valor=${valor}&modoOperacao=${modoOperacao}`;
  axios
    .post(endpointEsp, { headers: { "Content-Type": "application/json" } })
    .then((response) => {
      console.log("Estado do atuador atualizado com sucesso:", response.data);
      atualizarDadosAtuadores(`http://${ipEsp}/atuadores`); // Atualiza a lista de atuadores após a alteração
    })
    .catch((error) => {
      console.error("Erro ao atualizar o estado do atuador:", error);
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


