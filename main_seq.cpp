//https://github.com/whoshuu/cpr-example/blob/master/CMakeLists.txt
//CMakeList template
//https://www.submarino.com.br/categoria/instrumentos-musicais/violao

#include <iostream>
#include <fstream>
#include <cpr/cpr.h>
#include <string>
#include <boost/regex.hpp>
#include <chrono>
#include "produto.h"
#include "crawler.h"

using namespace std;

int main(int argc, char** argv) {

	//declarações	
	vector <string> urls,produtos;
	double tempo_ocioso = 0;
	double tempo_produto_total = 0;
	int n_produto = 0;
	int n_pagina = 0;
	chrono::time_point<chrono::high_resolution_clock> start, end, start_prod, end_prod, start_oci, end_oci;
	
	//inicio da contagem de tempo total do programa
	start = chrono::high_resolution_clock::now();

	//ṕegar url como entrada
	//string url = "https://www.submarino.com.br/categoria/instrumentos-musicais/equipamento-para-dj";
	string url = argv[1];

	//pegar o html da pagina inicial
	auto r = cpr::Get(cpr::Url{url});
	tempo_ocioso += r.elapsed;

	//html da pagina inicial
	string html = r.text;

	//passa por todas as páginas de produto
	while(url != "last_page"){

		n_pagina ++;
		
		cout << "\n\n----------------------PAGINA "<<n_pagina<<"----------------------\n\n";

		//pega a lista de produtos da pagina
		produtos = getProd(html);		

		// gerar o json para cada produto
		for (int unsigned i = 0; i < produtos.size(); ++i)
		{
			//inicio da contagem de tempo para o produto
			start_prod = chrono::high_resolution_clock::now();			
			//pega a pagina de um produto
			auto r = cpr::Get(cpr::Url{produtos[i]});
			//marcação do tempo ocioso para cada produto
			tempo_ocioso += r.elapsed;


			//análise de um produto
			string html_prod = r.text;


			Produto produto = create_prod(html_prod);
			n_produto++;
			string json = produto.jsonGen();
			//marcação do tempo para cada produto
			end_prod = chrono::high_resolution_clock::now();
			
			
			//retorna um objeto no formato json com informações do produto
			cout << json << "\n\n";

    		chrono::duration<double> func2 = end_prod - start_prod;
	    	cout << "tempo p/ produto: " << func2.count() << "s\n";
	    	tempo_produto_total += func2.count();

	    	//contagem de produtos
			//cout << "numero do produto: " <<n_produto << "\n\n";
			cout << "----------//----------//----------//\n\n";
			cout <<"tempo ocioso: "<< tempo_ocioso << "s\n";
		}

		//proxima pagina
		urls.push_back(url);
		url = nextPage(html,url);
		auto r = cpr::Get(cpr::Url{url});
		html = r.text;
		tempo_ocioso += r.elapsed;
	}

	cout << "tempo médio por produto: " << tempo_produto_total/n_produto << "\n";
	cout << "tempo total ocioso: " << tempo_ocioso << "s\n";

	//marcação do tempo total para o programa
	end = chrono::high_resolution_clock::now();
    chrono::duration<double> func3 = end - start;
    cout << "tempo total: " << func3.count() << "s\n";

    /*
tempo p/ produto: 0.888815s
numero do produto: 1267

----------//----------//----------//

tempo médio por produto: 0.762588
tempo total ocioso: 962.03s
tempo total: 1013.87s


    */
    
} 