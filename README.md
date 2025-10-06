# SoftBlocks

Este programa é um programa que utiliza a técnica de *raytracing* para
renderizar por software uma cena 3d com voxels. Além disso, ele também utiliza
o *OpenMP* para paralelizar o processo, permitindo que haja otimizações.

Este programa está distribuído sob a licença GNU GPLv3+.

![](screenshot.png)

# Compilação e execução

São necessárias as seguintes dependências:
- SDL2
- SDL2\_image
- SDL2\_ttf
- cmake
- g++
- openmp
- git

Clone este repositório no seu computador e acesse sua pasta. Depois disso,
digite no seu terminal:

```
$ cmake -B build
$ cd build && make
$ cd ..
$ ./build/softblocks
```

O programa irá rodar adequadamente. O programa necessita do arquivo de imagens
para rodar, que aqui é chamado de *textures.png*.

# Controles

O programa os seguintes controles:

```
W - andar para frente
S - andar para trás
A - andar para esquerda
D - andar para direita
espaço - pular
mouse - movimentar a câmera
botão direito do mouse - remover bloco
botão esquerdo do mouse - colocar bloco
1-9 - escolher bloco
```
