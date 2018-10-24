# WebCrawler
Web crawler em c++
Projeto da matéria de supercomputação
INSPER

## Projeto

Esse projeto realiza uma análise real-time de todos os produtos listados de um categoria específica do e-commerce https://www.submarino.com.br/

O objetivo é realizar uma análise de desempenho comparando uma versão sequencial do projeto com uma multithread.

## Bibliotecas

* [cpr](https://whoshuu.github.io/cpr/) - Fazer o download das páginas HTML
* [Boost Regex](https://theboostcpplibraries.com/boost.regex) - Realizar o parsing dos HTML
* [mprof](https://pypi.org/project/memory_profiler/) - Usado para os teste de memória

### Instalação

Primeiro é necessário clonar o repositório

```
git clone https://github.com/AToyama/WebCrawler 
```
Esse projeto é compilado utilizando CMake, ao entrar no diretório clonado compile com:

```
$CMake .
$Make
```
Isso irá gerar dois executáveis "seq_crawler" e "par_crawler", que são respectivamente as versões sequencial e paralela do projeto.

## Executando os testes

Para rodar os teste primeiro é necessário de uma URL válida da qual iremos realizar o rastreamento dos produtos. No site da [submarino](https://www.submarino.com.br/) os produtos são divididos em categoria e então em sub-categorias, logo o formato de url que utilizamos segue o seguinte formato:


´´´
- https://www.submarino.com.br/categoria/[ categoria ]/[ sub-categoria ]
´´´

Alguns exemplos de links válidos:

- https://www.submarino.com.br/categoria/audio/micro-system
- https://www.submarino.com.br/categoria/eletrodomesticos/geladeira-refrigerador
- https://www.submarino.com.br/categoria/instrumentos-musicais/violao

..
