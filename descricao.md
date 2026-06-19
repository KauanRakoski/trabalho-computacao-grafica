# Relatório de Implementação - Crashando de Carros

Este documento avalia a implementação atual do projeto frente aos requisitos levantados em `SPEC.md` e funciona como um guia de estudo e revisão do trabalho desenvolvido.

## 1. Cobertura da Especificação (SPEC.md)

1. **Malhas poligonais complexas**: **Implementado**. O código utiliza a biblioteca `tiny_obj_loader` para carregar o modelo do mapa em `TrackMap.cpp` e os modelos dos personagens e veículos diretamente em `main.cpp` (e.g., Crash, Cortex, Trikee).
2. **Transformações geométricas controladas pelo usuário**: **Implementado**. Através de interações do teclado capturadas no `main.cpp`, a classe `Entity` processa comandos (W, A, S, D) aplicando matrizes de translação (vetor *forward*) e rotação (eixo Y).
3. **Diferentes tipos de câmeras**: **Implementado**. O arquivo `Camera.cpp` conta com três lógicas principais: `UpdateFollow` (terceira pessoa), `UpdateFirstPerson` (primeira pessoa) e `UpdateFrontView` (câmera frontal).
4. **Instâncias de objetos**: **Implementado**. O projeto usa o padrão de arquitetura de `Entity`, instanciando componentes com base em matrizes de controle independentes de suas malhas estruturais.
5. **Testes de intersecção**: **Implementado**. Estão localizados em `collisions.cpp` os testes entre AABBs (Axis-Aligned Bounding Boxes). O sistema se tornou mais avançado no arquivo `TrackMap.cpp`, com projeção de raios e cálculos baricêntricos nos triângulos 3D do cenário (`PointInTriangleXZ`) para resolver paredes e alturas do chão.
6. **Modelos de Iluminação em todos os objetos**: **Implementado**. O `shader_fragment.glsl` lida com uma equação de iluminação *Blinn-Phong*, com constantes que se alteram dependendo de variáveis de cenário (luzes externas e internas como *castle_int* ou *dungeon*).
7. **Mapeamento de texturas em todos os objetos**: **Implementado**. Texturas carregadas via `stb_image` com coordenadas passadas pelos objetos (`TrackMap.cpp` e `main.cpp`) aos *shaders*. Onde os modelos carecem de coordenadas UV nativas, utilizou-se fórmulas matemáticas esféricas e planares para gerá-las.
8. **Movimentação com curva Bézier cúbica**: **Implementado**. A função `EvaluateCubicBezier` está em `main.cpp` e cria a animação do vetor de posição da câmera no momento "Intro" (abertura da corrida).
9. **Animações baseadas no tempo ($\Delta t$)**: **Implementado**. As atualizações numéricas de cinemática do motor (física de gravidade e impulso das entidades) recebem os ganhos de `deltaTime` contados no *Game Loop*.

### Limitações vs. Superação de Expectativas
* **Efeitos Visuais**: Embora o `SPEC.md` tenha previsto a ausência de partículas, o arquivo `Physics.cpp` mostra uma **implementação funcional de um sistema de partículas** simulando fumaça (usando *billboarding*). É um "over-delivery" excelente que agregará nota.
* **Colisões**: As colisões planejadas envolviam apenas checagem de AABB, mas a implementação avançou muito e calculou inclinação de triângulos do chão do mapa, fazendo o veículo subir e descer rampas suavemente.

## 2. Principais Fluxos da Aplicação

1. **Inicialização**:
   - Criação da janela via GLFW e inicialização do contexto OpenGL.
   - Compilação dos programas de GPU (shaders).
   - Leitura de texturas de imagem e construção da `g_VirtualScene` (leitura dos arquivos `.obj`).
   - Instanciação de `Entity` (Player, NPCs) e `TrackMap` (Pista).
2. **Ciclo de Atualização (Game Loop)**:
   - **Cálculo Delta Time**: Avalia-se os milissegundos para manter um passo físico estável independente da taxa de quadros (FPS).
   - **Input e Estados**: Baseado nas teclas (ex: modo corrida, fim de jogo, pausa, modo câmera livre), atualizam-se velocidades e vetores.
   - **Física (Physics.cpp / TrackMap.cpp)**: As coordenadas dos carros colidem com as AABBs, o código de piso resolve a altura `Y` do carro dependendo da área do triângulo da pista que ele sobrepõe, enquanto colisões entre entidades disparam repulsões (`knockback`).
   - **Renderização e Câmera**: Os vetores *Look-At* das câmeras espelham os dados atuais da Entidade. O código percorre os shaders para redesenhar a pista, objetos dinâmicos e UI (Tempo/Voltas com a engine `textrendering`).

## 3. Arquivos Recomendados para Estudo

Para garantir uma resposta concisa a perguntas de implementação ou para futuras alterações, dominar a lógica dos arquivos abaixo é recomendável:

- **`main.cpp`**: Conecta tudo. Contém os parâmetros globais, criação da geometria `.obj`, *callbacks* de input de usuário e a curva paramétrica de Bézier que rege a câmera inicial.
- **`TrackMap.cpp / .h`**: É vital por armazenar o algoritmo matemático que detecta se o carro colide com encostas, mapeamento baricêntrico em 2D `PointInTriangleXZ` e triângulos de renderização maciça.
- **`Entity.cpp / .h`**: Compreensão da matriz hierárquica M = T * R * S que permite girar rodas globalmente/localmente e instanciar inimigos eficientemente.
- **`shader_fragment.glsl`**: Contém o coração da computação de GPU e interpolação final de cor, luz, material difuso, especularidade e a normalização de texturas.
- **`Camera.cpp`**: Simplifica como o cálculo LookAt constrói o conceito de 3ª pessoa ou 1ª pessoa através da distância base.

## 4. Pendências e Sugestões Finais

O projeto já está muito robusto e implementou tudo e mais um pouco da especificação solicitada. Sendo extremamente criterioso, se ainda houver prazo a usar, os próximos passos seriam:
* **Finalização de Interface**: Adicionar imagens/HUD além de textos na tela (exemplo: pequenas fotos em sprites 2D fixos de liderança ou voltas) daria a "cara" polida de *Crash Nitro Kart*.
* **Revisão de Caminhos (.obj/.png)**: Assegurem que na máquina do laboratório do professor as pastas de textura (ex.: `assets/...`) estão com os mesmos nomes de pastas locais.
* **Comportamento em Bordas (Sliding)**: Durante impactos contra AABBs, o veículo trava secamente. Pode-se implementar uma fórmula vetorial suave que permita ao carro deslizar pelos cantos em vez de parar por completo, mas é apenas um refino (não atrapalha o escopo final).
