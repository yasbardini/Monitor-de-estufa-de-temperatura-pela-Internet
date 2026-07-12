#include "DS18B20.h" // inclui a bib do sensor de temperatura 
#include "delay.h" // inclui bib de delay

#include <stdio.h> // inclui bib de entrada e saída padrão

DS18B20::DS18B20(gpio_num_t pino) // construtor da classe DS18B20, recebe o pino do sensor como parâmetro
{
    DEBUG("DS18B20:Construtor\n"); // imprime mensagem de debug
    onewire = new ONEWIRE(pino); // cria objeto onewire com o pino do sensor, para comunicação com o sensor 
}

void DS18B20::programa(void) // função que programa o sensor, configurando a resolução e o modo de operação
{
    onewire->reset(); // envia sinal de reset para o sensor
    // O sensor DS18B20 tem uma memória interna chamada Scratchpad. O comando 0x4E avisa o sensor que vamos escrever nessa memória.
    onewire->writeByte(0x4E); // envia comando para escrever na memória de configuração do sensor, próximo byte enviado será o valor a ser escrito na memória de configuração
    onewire->writeByte(0x00); // envia valor para configurar o sensor, neste caso, 0x00 configura o sensor para resolução de 9 bits 
    //  significa que a temperatura será medida com uma precisão de 0,5°C 
    onewire->writeByte(0x00); // envia valor para configurar o sensor, neste caso, 0x00 configura o sensor para modo de operação normal, sem alarmes
    onewire->writeByte(0x7F); // configura o sensor para trabalhar na sua resolução máxima (12 bits), o que nos dá aquela precisão de até 4 casas decimais
}

void DS18B20::init(void) // função que inicializa o sensor, realizando a detecção do dispositivo e capturando seu endereço
{ 
    uint8_t serial[6], crc; // vetor de 6 bytes para armazenar o número de série do dispositivo e variável para armazenar o valor do CRC (ACK) 

    printf("*** init***\n");
    delay_ms(500); // aguarda 500 milissegundos para garantir que o sensor esteja pronto para comunicação

    printf("INIT\n");
    if (onewire->reset() == 0) // envia sinal de reset para o barramento 1-Wire, iniciando a comunicação com os dispositivos conectados
        printf("Detectou escravo na linha\n");
    else
        printf("Nao detectou escravo\n"); // escravo significa que o sensor está presente e pronto para comunicação, caso contrário, indica que não há dispositivos conectados no barramento

    onewire->writeByte(READ_ROM); // envia comando para ler o endereço do dispositivo conectado no barramento 1-Wire, permitindo que o ESP32 capture o código da família, número de série e CRC (ACK) do sensor

    printf("Codigo da Familia: %d\n", onewire->readByte()); // lê e imprime o código da família do dispositivo, que identifica o tipo de sensor conectado (DS18B20 tem código de família 0x28)
    for (uint8_t x = 0; x < 6; x++) // 
        serial[x] = onewire->readByte();

    printf("Numero de Serie : %d %d %d %d %d %d\n",
           serial[0], serial[1], serial[2], serial[3], serial[4], serial[5]);

    crc = onewire->readByte(); // lê e armazena o valor do CRC (ACK) do dispositivo, que é usado para verificar a integridade dos dados transmitidos no barramento 1-Wire
    printf("CRC= : %d\n", crc);

    delay_ms(1000);
}

void DS18B20::init2(void) // função que realiza uma segunda inicialização do sensor, verificando novamente a presença do dispositivo no barramento 1-Wire
// o sensor pode não estar pronto para comunicação na primeira inicialização, garantindo que o sensor esteja corretamente detectado antes de prosseguir com a leitura da temperatura
{
    printf("*** init2***\n"); 
    delay_ms(500);

    printf("INIT\n");
    if (onewire->reset() == 0)
        printf("Detectou escravo na linha\n");
    else
        printf("Nao detectou escravo\n");

    delay_ms(1000);
}

float DS18B20::readTemp(void) // função que lê a temperatura do sensor DS18B20, sem utilizar o endereço do dispositivo, assumindo que há apenas um sensor conectado no barramento 1-Wire
{
    float temperatura; 
    uint8_t a, b, inteira;
    float frac;

    onewire->reset(); // envia sinal de reset para o barramento 1-Wire, iniciando a comunicação com o sensor
    onewire->writeByte(SKIP_ROM); // envia comando para ignorar a seleção do dispositivo específico, permitindo que o ESP32 se comunique com o sensor conectado no barramento 1-Wire sem precisar fornecer o endereço do dispositivo
    onewire->writeByte(INICIA_CONVERSAO_TEMP); // envia comando para iniciar a conversão de temperatura no sensor DS18B20, solicitando que o sensor meça a temperatura ambiente e armazene o valor na memória interna
    delay_ms(1000);

    onewire->reset();
    onewire->writeByte(SKIP_ROM); // envia comando para ignorar novamente a seleção do dispositivo específico, garantindo que a leitura dos dados de temperatura seja realizada apenas do sensor conectado no barramento 1-Wire
    onewire->writeByte(READ_TEMP_MEMORY); // envia comando para ler os dados de temperatura armazenados na memória interna do sensor DS18B20, permitindo que o ESP32 capture os bytes que representam a temperatura medida pelo sensor

    a = onewire->readByte();
    b = onewire->readByte();
    printf("a=%u b=%u\n", a, b);


    inteira = (b << 4) | (a >> 4);

    frac = ((a & 1) * 0.0625f) + (((a >> 1) & 1) * 0.125f) + (((a >> 2) & 1) * 0.25f) + (((a >> 3) & 1) * 0.5f);

    temperatura = inteira + frac;
    return temperatura;
}