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
$ cmake .
$ make
```
Isso irá gerar dois executáveis "seq_crawler" e "par_crawler", que são respectivamente as versões sequencial e paralela do projeto.

## Executando os testes

Para rodar os teste primeiro é necessário de uma URL válida da qual iremos realizar o rastreamento dos produtos. No site da [submarino](https://www.submarino.com.br/) os produtos são divididos em categoria e então em sub-categorias, logo o formato de url que utilizamos segue o seguinte formato:


```
- https://www.submarino.com.br/categoria/[ categoria ]/[ sub-categoria ]
```

Alguns exemplos de links válidos:

- https://www.submarino.com.br/categoria/audio/micro-system
- https://www.submarino.com.br/categoria/eletrodomesticos/geladeira-refrigerador
- https://www.submarino.com.br/categoria/instrumentos-musicais/violao

Para executar o programa:

**Sequencial:**

```
$ ./seq_crawler <https://www.submarino.com.br/categoria/[ categoria ]/[ sub-categoria ]>
```

**Paralelo:**

```
$ ./par_crawler <https://www.submarino.com.br/categoria/[ categoria ]/[ sub-categoria ]>
```

No paralelo também temos a opção de selecionar o número de threads para executar o programa, por padrão ele é 4, mas podemos atribuir isso a partir de um váriavel de ambiente da seguinte maneira (exemplo para 8  threads) :

```
$ NUM_THREADS=8 ./par_crawler <https://www.submarino.com.br/categoria/[ categoria ]/[ sub-categoria ]>
```
## Dados:

Como saída do programa teremos para cada produto um json no seguinte formato:

```
{
"nome": "",
"descricao": "",
"foto": "",
"preco": 0,
"preco_parcelado": 0,
"preco_num_parcelas": 0,
"categoria": "",
"url": ""
}
```

E as seguintes medidas de tempo

- Tempo para download da página e análise de cada produto
- Tempo médio de todos produtos
- Tempo total ocioso do programa (tempo para o download de todas as páginas)
- Tempo de execução total do programa

## Teste de Memória:

Para realizar os teste de memória basta colocar o seguinte comando antes do executável

```
$ mprof run ./[executavel] [URL]
```

O programa irá rodar normalmente, para gerar o gráfico de consumo de memória basta esperar o programa acabar de rodar e executar:

```
$ mprof plot
```

Para a versão paralela do programa deve ser levada em conta que se for utilizar a variável de ambiente NUM_THREADS, ela deve ser declarada antes do comando do teste de memória, da seguinte maneira:

```
$ NUM_THREADS=[] mprof run ./[executavel] [URL]
```
