# Capacitated Vehicle Routing Problem (CVRP)

O CVRP se trata de um problema de otimização combinatória da classe NP-difícil bastante conhecido e estudado na literatura.
 
Para entender o problema, imagine um conjunto de veículos, cada um com uma capacidade máxima de coleta, e um conjunto pré-determinado de clientes, onde cada par de clientes tem um custo de transição entre eles e cada cliente possui uma quantidade de material a ser recolhida. A ideia é que cada veículo passe somente uma vez por um determinado cliente, cada cliente seja visitado somente por um veículo e cada veículo volte ao ponto de partida no final do trajeto. Para reduzir custos, é necessário escolher uma rota, dentre todas as rotas possíveis, onde o somatório dos custos da viagem entre os clientes seja a menor e que não exceda a capacidade de coleta de algum veículo.

## Formas de resolução

Existem dois métodos que podem ser seguidos para resolver problemas como o descrito anteriormente, os exatos ou os heurísticos.

Métodos exatos visam iterar sobre todo o conjunto de soluções de um problema para obter sua resposta. Por conta disso, eles retornam a solução ótima do problema, ou seja, a melhor dentre todas as soluções. Por outro lado, para problemas da classe NP, não é possível encontrar estas soluções em tempo computacional aceitável a partir de certo ponto. Para ilustrar, uma instância do CVRP com 1 veículo e 60 vértices possui uma quantidade de soluções possíveis semelhante a quantidade de átomos no universo!

Por outro lado, existem os métodos heurísticos, que tentar resolver estes problemas com um tempo computacional aceitável sem garantir sua otimalidade, mas que podem trazer boas soluções, e os meta-heurísticos, que tem a mesma finalidade, porém com o intuito de resolver vários tipos de problemas diferentes com um mesmo algoritmo.

O algorítmio implementado neste repositório para resolver o CVRP se trata de uma meta-heurística denominada [GILS-RVND](https://www.sciencedirect.com/science/article/abs/pii/S037722171200269X) adaptada para resolver o presente problema.

## Instância do CVRP

As instâncias do CVRP são criado a partir de um conjunto de cidades, cada uma com sua localização geográfica. Em posse desses dados, os custos de deslocamento podem ser calculados para cada par de cidades, onde é descrito por uma matriz, podendo esta ser simétrica ou assimétrica. E um conjunto fictício de veículos, comcapacidades variadas.

As instâncias contidas neste repositório são **simétricas**, e estão todas localizadas em `./instances` ou podem ser obtidas no [CVRPLIB](http://vrp.galgos.inf.puc-rio.br/index.php/en/), onde também contêm os resultados ótimos de cada instância obtidos por métodos exatos. Segue um exemplo de matriz simétrica para um problema com 4 vértices.

```
M = | -- 10 20 30 |
    | 10 -- 10 20 |
    | 20 10 -- 10 |
    | 30 20 10 -- |
```

*OBS: As instâncias presentes neste repositório foram adaptadas para facilitar a leitura, mas possuem os mesmos valores das instâncias originais.*

## Compilando e executando

Para compilar o programa, basta utilizar o comando `make` pelo terminal na pasta raiz, gerando o arquivo executável `cvrp.cpp`.

Para executar, basta utilizar o seguinte comando, **substituindo INSTÂNCIA por qualquer instância dentro da pasta `./instances`**.

``` bash
./cvrp instances/INSTANCIA
```

## Resultados

A presente implementação do algoritmo consegue encontrar os resultados ótimos das instâncias presentes na pasta.

## TODO

* Adaptar um número maior de instâncias
* Criar tabela com os resultados computacionais
