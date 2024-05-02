// Função para criar um card para cada sensor
function criarCardSensor(sensor) {
    return `
    <div class="col-sm-3">
        <div class="card text-black">
            <div class="card text-center">
                <div class="card-header">
                    <p class="text-center">
                        <b>${sensor.nome}</b>
                    </p>
                </div>
                <div class="card-body">
                    <b> ${sensor.valor_ultima_obs} ${sensor.unidadeSimbolo} </b>
                </div>
                <div class="card-footer">
                    <p class="text-center"><b>Atualização</b>: ${sensor.data_ultima_obs}</p>
                    <button type="button" class="btn btn-primary btn-sm" onclick="abrirModalEdicao('${sensor.id}', '${sensor.nome}', '${sensor.pin}', '${sensor.unidadeId}')">Editar</button>
                </div>
            </div>
        </div>
    </div>
    `;
}

document.addEventListener('DOMContentLoaded', function() {
    const idDispositivo = 1; // Substitua pelo ID do dispositivo desejado
    const endpoint = `http://localhost:8080/smartlab/api/dispositivos/${idDispositivo}`; // Substitua por seu domínio e rota correta
    
    
    atualizarDadosSensores(endpoint);
    getUnidades();

    // Configura para atualizar os dados a cada segundo (1000 milissegundos)
    setInterval(() => atualizarDadosSensores(endpoint), 2000);


        const formSensor = document.getElementById('formSensor');

        formSensor.addEventListener('submit', function(e) {
            e.preventDefault(); // Impede o comportamento padrão de envio do formulário
    
            // Captura os dados do formulário
            const nomeSensor = document.getElementById('nomeSensor').value;
            const pinSensor = document.getElementById('pinSensor').value;
            const idUnidade = document.getElementById('idUnidade').value;
    
            // Prepara o objeto com os dados
            const dadosSensor = {
                nome: nomeSensor,
                pin: pinSensor,
                dtCriacao: new Date(),
                dispositivoId: idDispositivo, 
                unidadeId: idUnidade ? idUnidade : null
            };
    
            // Endpoint para adicionar um novo sensor (substitua pelo seu)
            const endpoint = `http://localhost:8080/smartlab/api/sensores`;
    
            // Envia a requisição POST
            axios.post(endpoint, dadosSensor)
                .then(response => {
                    console.log('Sensor adicionado com sucesso:', response.data);
                    // Aqui você pode adicionar alguma ação após o sucesso, como recarregar a lista de sensores
                })
                .catch(error => {
                    console.error('Erro ao adicionar o sensor:', error);
                });
        });
    });

function abrirModalEdicao(sensorId, nomeSensor, pinSensor, idUnidade) {
    // Preenche os campos do formulário com os dados atuais do sensor
    document.getElementById('editarNomeSensor').value = nomeSensor;
    document.getElementById('editarPinSensor').value = pinSensor;
    document.getElementById('editarIdUnidade').value = idUnidade;
    document.getElementById('editarSensorId').value = sensorId;

    // Abre o modal
    $('#modalEditarSensor').modal('show');

    document.getElementById('salvarEdicao').addEventListener('click', function() {
        const sensorId = document.getElementById('editarSensorId').value;
        const dadosAtualizados = {
            nome: document.getElementById('editarNomeSensor').value,
            pin: document.getElementById('editarPinSensor').value,
            unidadeId: document.getElementById('editarIdUnidade').value
        };
    
        // Chama a função para atualizar o sensor
        atualizarSensor(sensorId, dadosAtualizados);
    
        // Fecha o modal após a atualização
        $('#modalEditarSensor').modal('hide');
    });

    document.getElementById('excluirSensor').addEventListener('click', function() {
        const sensorId = document.getElementById('editarSensorId').value;
        // Chama a função para excluir o sensor
        excluirSensor(sensorId);
    
        // Fecha o modal após a exclusão
        $('#modalEditarSensor').modal('hide');
    });
    
}

function atualizarSensor(sensorId, dadosAtualizados) {
    // Endpoint para atualizar um sensor (substitua pelo seu)
    const endpoint = `http://localhost:8080/smartlab/api/sensores/${sensorId}`;
    
    // Envia a requisição PUT
    axios.put(endpoint, dadosAtualizados)
        .then(response => {
            console.log('Sensor atualizado com sucesso:', response.data);
            // Aqui você pode adicionar alguma ação após o sucesso, como recarregar a lista de sensores
        })
        .catch(error => {
            console.error('Erro ao atualizar o sensor:', error);
        });
}

function excluirSensor(sensorId) {
    // Endpoint para excluir um sensor (substitua pelo seu)
    const endpoint = `http://localhost:8080/smartlab/api/sensores/${sensorId}`;
    
    // Envia a requisição DELETE
    axios.delete(endpoint)
        .then(response => {
            console.log('Sensor excluído com sucesso:', response.data);
            // Aqui você pode adicionar alguma ação após o sucesso, como recarregar a lista de sensores
        })
        .catch(error => {
            console.error('Erro ao excluir o sensor:', error);
        });
}

function atualizarDadosSensores(endpoint) {
    
    axios.get(endpoint)
        .then(function(response) {
            const dispositivo = response.data;
            console.log(dispositivo);
            const sensores = response.data.sensors;
            console.log(sensores);
            const container = document.querySelector('.row'); // Seleciona o elemento onde os cards devem ser inseridos
            container.innerHTML = ''; // Limpa o conteúdo atual

            // Para cada sensor recebido, cria um card e adiciona ao container
            sensores.forEach(sensor => {
                console.log(sensor);
                container.innerHTML += criarCardSensor(sensor);
            });
        })
        .catch(function(error) {
            console.error('Erro ao obter os sensores:', error);
        });

}

// Exemplo de função para buscar unidades e preencher as opções do dropdown
function getUnidades() {
    const endpointUnidades = `http://localhost:8080/smartlab/api/unidades`; // Substitua pela URL da sua API
    axios.get(endpointUnidades)
        .then(function(response) {
            const unidades = response.data;
            const dropdowns = document.querySelectorAll('.unidade-dropdown');
            dropdowns.forEach(dropdown => {
                console.log("ola");
                dropdown.innerHTML = unidades.map(unidade => `<option value="${unidade.id}">${unidade.descricao} - ${unidade.simbolo}</option>`).join('');
            });
        })
        .catch(function(error) {
            console.error('Erro ao obter unidades:', error);
        });
}





