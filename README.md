# Computação Gráfica e Visualização I (INF01047) - INF/UFRGS

Este repositório contém o código base para o trabalho final. O enunciado completo do trabalho final está no Moodle:

https://moodle.ufrgs.br/mod/assign/view.php?id=6018620


## Relatório

Abaixo, imagens do jogo desenvolvido em funcionamento:

![alt text](image.png)

![alt text](image-1.png)

### Aplicação desenvolvida

Crashando de Carros consiste em uma aplicação gráfica que visa simular, de forma simplificada, o jogo Crash Nitro Kart de Playstation 2, um jogo de corrida com os personagens caricatos da franquia Crash. Sendo assim, implementou-se a cópia de um mapa específico, Once Upon a Tire, com dois personagens: o Crash, controlado pelo jogador, e o Cortex, NPC adversário. O objetivo do jogo é ser o primeiro a completar 2 voltas pela pista, podendo ganhar ou perder, além de ser possível controlar câmeras tanto em terceira quanto em primeira pessoa. Para jogar, disponibilizam-se duas maneiras de realizar os principais comandos de movimentação do personagem, pelas teclas W,A,S,D ou por controle genérico USB. 

### Contribuições de Cada Membro

Os membros trabalharam de forma fluida, de forma que suas responsabilidades se misturam em alguns aspectos. Porém, podemos elencar dados principais abaixo:

**Kauan Rakoski**

- Adição de modelos do Crash e Cortex com seus carros;
- Criação de um modelo de entidade que abstrai as aplicações matriciais;
- Movimentação dos carros baseada em inputs do usuário;
- Implementação da trilha sonora;
- Criação do sistema de partículas que gera a fumaça;
- Implementação dos checkpoints e sistema básico de vitória;

**Pedro Schuck**

- Inclusão de modelos de iluminação à cena virtual;
- Correção de bugs envolvendo as texturas, iluminação e renderização do mapa;
- Instanciação de caixas ao longo da pista;
- Implementação de curvas de Bézier para a movimentação inicial da câmera;
- Adição de diferentes modos de câmera;
- Atualização da interface com o tempo decorrido no jogo;

De modo geral, creio que ambos avaliem a colaboração como positiva.

### Parágrafo Avaliando uso de IA generativa

Dado a liberação de uso, os membros usaram IA para consulta e para acelerarem seu trabalho. O principal uso de IA está na geração do mapa - arquivo trackMap.cpp - onde a implementação foi totalmente executada com uso de um agente de código (AntiGravity) usando o modelo Gemini 3.1 Pro (High). 

Inicialmente, foi uma surpresa o agente ter implementado algo complexo tão rápido, porém com o decorrer do tempo o grupo percebeu que tal implementação não era satisfatória. Além de conter bugs de colisão e renderização de texturas, o agente foi incapaz de resolvê-los completamente (ao corrigir algo, piorava outro aspecto). O fato de terceirizar essa operação de geração também deixou a equipe distante da lógica de pensamento e teve de ser gasto um tempo grande para compreender o artefato gerado. 

Junto à isso, para tentar corrigir bugs, uma sequência de prompts no estilo "It is still wrong, fix" teve de ser feita (omitida do arquivo prompts.md), mesmo com descrições ligeiramente claras de implementação. Desse modo, avalio (Kauan Rakoski) que a geração com IA está longe de ser 100% viável, pois além de parecer depender de sorte, afasta o desenvolvedor do código, e o que ganha de tempo sacrifica em qualidade. Talvez com orquestrações de agentes seja melhor, porém precisa-se gastar muito tempo configurando agentes, logo acho que compensa mais gastar esse tempo escrevendo código.

Em seções mais pequenas e com escopo bem delimitado, porém, o uso de agentes fornece bons resultados com uma implementação rápida. A dupla considera que, nestes cenários, o emprego de tais ferramentas foi de grande utilidade, permitindo que detalhes como aplicação de texturas nos modelos 3D e instanciação destes na cena virtual fossem facilitados, além de reduzir a carga de trabalho de tarefas complexas, como uso de modelos de iluminação. Dessa forma, concluo (Pedro Schuck) que mesmo sendo inefciente para algumas tarefas, o uso de IA ainda consiste em um recurso valioso para criar e corrigir código, devendo ser continuamente explorado conforme tais tecnologias evoluem. 

### Manual de como jogar o jogo

Para a movimentação do personagem:

- `W`, `A`, `S`, `D` para frente, esquerda, trás (ré) e direita;
- `1` para câmera em primeira pessoa;
- `2` para câmera em terceira pessoa;
- `3` para câmera olhando para trás;
- `p` e `o` para alternar entre projeção perspectiva e ortográfica;
- `r` para reiniciar o jogo, assim que ganha ou perde;
- `esc` para encerrar a aplicação;
  
  Alternativamente, com controles USB:
  
- Joystick esquerdo para rotacionar o carro (esquerda e direita);
- `X` para acelerar e `Quadrado` para ré;

### Comandos de compilação

No windows, tendo o cmake configurado, pode-se utilizar os seguintes comandos:

```
cmake --build build
cd bin/debug
./main
```

Alternativamente, pode-se usar o script `run.cmd` que executa os comandos acima. 

### Comentários Importantes

Os prompts utilizados haviam sido descritos em um arquivo prompts.md. Em algum momento do desenvolvimento, este arquivo se perdeu e perdemos alguns dos prompts, que foram reconstituídos depois. As funções executadas por IA estão documentadas, e commits feitos com uso de agentes de IA estão especificados com os próprios prompts utilizados. 

Caso haja alguma falta de especificação, gostaríamos de esclarecer que se trata de descuido nos detalhes e não ato de má fé. O grupo se dispõe a esclarecer quaisquer assuntos referentes ao uso de IA generativa.
