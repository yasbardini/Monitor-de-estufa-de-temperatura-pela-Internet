# README - Teste da V1

Esta versão V1 permite validar a lógica principal do trabalho mesmo sem o sensor físico: conexão Wi-Fi, conexão MQTT, configuração remota dos limites, resposta aos tópicos de consulta e publicação automática de alertas quando a temperatura simulada sai da faixa configurada. O broker usado nos testes é o público `broker.emqx.io`, exatamente como indicado no enunciado e no material de apoio.

## Objetivo da V1

A V1 serve para comprovar que o ESP32:

- conecta no Wi-Fi
- conecta ao broker MQTT
- assina os tópicos exigidos
- responde às solicitações `/informa/...`
- aceita configuração remota dos limites
- publica alertas automáticos quando a temperatura ultrapassa os limites

## Pré-requisitos

Antes dos testes, é necessário ter:

- o ESP32 com a V1 gravada e executando
- SSID e senha Wi-Fi corretamente configurados no projeto
- utilitários `mosquitto_pub` e `mosquitto_sub` instalados para publicar e assinar tópicos via terminal

## Broker MQTT

Broker de teste:

```bash
broker.emqx.io
```

O MQTT funciona no modelo publish-subscribe, então qualquer cliente inscrito em um tópico recebe as mensagens publicadas nele. Como o broker é público, pode haver interferência de outros testes usando os mesmos tópicos.

## Tópicos do trabalho

### Tópicos publicados pelo dispositivo

- `/alerta/temperaturaAlta`
- `/alerta/temperaturaBaixa`
- `/responde/temperaturaCorrente`
- `/responde/limiteAlta`
- `/responde/limiteBaixa`

### Tópicos assinados pelo dispositivo

- `/configura/alta`
- `/configura/baixa`
- `/informa/temperaturaCorrente`
- `/informa/limiteAlta`
- `/informa/limiteBaixa`

## Monitoramento dos tópicos

Abra terminais separados para acompanhar os tópicos de resposta e alerta:

```bash
mosquitto_sub -h broker.emqx.io -t "/responde/temperaturaCorrente"
mosquitto_sub -h broker.emqx.io -t "/responde/limiteAlta"
mosquitto_sub -h broker.emqx.io -t "/responde/limiteBaixa"
mosquitto_sub -h broker.emqx.io -t "/alerta/temperaturaAlta"
mosquitto_sub -h broker.emqx.io -t "/alerta/temperaturaBaixa"
```

Esses são os tópicos definidos no enunciado para respostas e alertas.

## Teste 1 - Verificar conexão

Com o ESP ligado e o monitor serial aberto, confirme:

- conexão Wi-Fi
- conexão com o broker MQTT
- assinatura dos tópicos MQTT

No serial, a expectativa é ver mensagens informando conexão de rede, conexão MQTT e confirmação das inscrições.

## Teste 2 - Consultar temperatura atual

Envie o comando:

```bash
mosquitto_pub -h broker.emqx.io -t "/informa/temperaturaCorrente" -n
```

Resposta esperada: o ESP publica um valor numérico em `/responde/temperaturaCorrente`

## Teste 3 - Consultar limite alto

Envie:

```bash
mosquitto_pub -h broker.emqx.io -t "/informa/limiteAlta" -n
```

Resposta esperada: o ESP publica o valor atual do limite alto em `/responde/limiteAlta`

## Teste 4 - Consultar limite baixo

Envie:

```bash
mosquitto_pub -h broker.emqx.io -t "/informa/limiteBaixa" -n
```

Resposta esperada: o ESP publica o valor atual do limite baixo em `/responde/limiteBaixa`

## Teste 5 - Alterar limite alto

Envie, por exemplo:

```bash
mosquitto_pub -h broker.emqx.io -t "/configura/alta" -m "24.0"
```

Depois consulte o valor:

```bash
mosquitto_pub -h broker.emqx.io -t "/informa/limiteAlta" -n
```

Resposta esperada: `/responde/limiteAlta` deve retornar `24.00` ou valor equivalente.

## Teste 6 - Alterar limite baixo

Envie:

```bash
mosquitto_pub -h broker.emqx.io -t "/configura/baixa" -m "19.0"
```

Depois consulte:

```bash
mosquitto_pub -h broker.emqx.io -t "/informa/limiteBaixa" -n
```

Resposta esperada: `/responde/limiteBaixa` deve retornar `19.00` ou valor equivalente.

## Teste 7 - Verificar alerta de temperatura alta

Como a V1 usa temperatura simulada, force um cenário de limite alto muito baixo:

```bash
mosquitto_pub -h broker.emqx.io -t "/configura/alta" -m "10.0"
```

Resposta esperada: na próxima leitura periódica, se a temperatura simulada estiver acima desse valor, o ESP publica em `/alerta/temperaturaAlta`

## Teste 8 - Verificar alerta de temperatura baixa

Force um cenário de limite baixo muito alto:

```bash
mosquitto_pub -h broker.emqx.io -t "/configura/baixa" -m "40.0"
```

Resposta esperada: na próxima leitura periódica, se a temperatura simulada estiver abaixo desse valor, o ESP publica em `/alerta/temperaturaBaixa`

## Temporização

O ciclo de leitura da temperatura deve ocorrer a cada 5 segundos, então pode ser necessário aguardar alguns segundos após ajustar os limites para observar os alertas automáticos.
