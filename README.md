# CurrentController

Projeto para controle de corrente elétrica utilizando a placa Nucleo-F446RE e um resistor shunt como sensor. A corrente é monitorada em tempo real, controlada por algoritmo PID, e ajustável via comunicação serial com interface gráfica.

## Descrição

Este projeto implementa um sistema de controle de corrente baseado em PID com um microcontrolador STM32F446RE (placa Nucleo). A medição da corrente é feita por meio de um resistor shunt e amplificador operacional, conforme o [esquemático](https://github.com/OtavioCruzatto/CurrentController/blob/master/Resources/Sch/Schematic%202.jpg). A corrente de saída é ajustada via DAC, e o sistema permite o monitoramento e ajuste dinâmico dos parâmetros PID por UART.

## Funcionalidades

- Medição de corrente via resistor shunt
- Controle PID em malha fechada com saída via DAC
- Comunicação serial (UART) para:
  - Envio de setpoint
  - Ajuste dos coeficientes PID (Kp, Ki, Kd)
  - Visualização da resposta do sistema em gráfico de linha
- Interface desktop desenvolvida em C#:
  - Repositório: [PidTuningHelper](https://github.com/OtavioCruzatto/PidTuningHelper)

## Pré-requisitos

- STM32CubeIDE ou STM32CubeMX
- Placa Nucleo-F446RE
- Circuito com resistor shunt e amplificador operacional
- Terminal serial (115200 baud rate, 8N1)
- Software de monitoramento: [PidTuningHelper](https://github.com/OtavioCruzatto/PidTuningHelper)

## Como Usar

1. Clone este repositório e abra o projeto no STM32CubeIDE.
2. Compile e grave o firmware na Nucleo-F446RE.
3. Monte o circuito de medição conforme o [esquemático](https://github.com/OtavioCruzatto/CurrentController/blob/master/Resources/Sch/Schematic%202.jpg).
4. Baixe e abra o [PidTuningHelper](https://github.com/OtavioCruzatto/PidTuningHelper).
5. Conecte à porta serial e ajuste o setpoint e os ganhos PID.
6. Visualize a resposta do sistema em tempo real com gráficos lineares.

## Comandos via UART

- Os comandos implementados para a comunicação entre o microcontrolador e a interface gráfica, podem ser consultados na seguinte [tabela de comandos](https://github.com/OtavioCruzatto/CurrentController/blob/master/Resources/DataPacket/Commands.xlsx).
- A seguinte ferramenta feita em C# auxilia no processo de codificar e decodificar comandos: [DataPacketViewer](https://github.com/OtavioCruzatto/DataPacketViewer).

## Ajuste PID

Os parâmetros Kp, Ki e Kd podem ser ajustados via interface gráfica durante a execução. Isso facilita a sintonia do controlador em diferentes condições de carga.

## Imagens
![plot](https://github.com/OtavioCruzatto/CurrentController/blob/master/Resources/Media/Images/GUI_1.jpg)

![plot](https://github.com/OtavioCruzatto/CurrentController/blob/master/Resources/Media/Images/Hardware_1.jpg)

## Licença

Este projeto está licenciado sob a licença MIT - consulte o arquivo [LICENSE](https://github.com/OtavioCruzatto/CurrentController/blob/master/LICENSE.md) para mais detalhes.

## Autor

Otavio Cruzatto  
[GitHub](https://github.com/OtavioCruzatto)  
[LinkedIn](https://www.linkedin.com/in/otaviocruzatto)