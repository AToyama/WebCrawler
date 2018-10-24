//https://github.com/whoshuu/cpr-example/blob/master/CMakeLists.txt
//CMakeList template
//https://www.submarino.com.br/categoria/instrumentos-musicais/violao

/*
- variaveis de ambiente(threads e link)
- medidas de tempo (usar semaphoro)
- coleta de dados
- relatorio <3
*/

#include <iostream>
#include <fstream>
#include <cpr/cpr.h>
#include <string>
#include <boost/regex.hpp>
#include <chrono>
#include <thread>

#include "produto.h"
#include "crawler.h"
#include "semaphore.h"

using namespace std;


void produceProd(vector<string>& produtos_url,vector<string>& produtos_html,vector<double>& tempos_produto,Semaphore& sm_pc, Semaphore& sm_producer, Semaphore& sm_tempoOcioso,double& tempo_ocioso){
	
	sm_producer.acquire();
//	cout << produtos_html.size() << " -  HTML\n";
//		cout << produtos_url.size() << " -  URL\n";
	if(produtos_url.empty()){
			cout << "\n\n\n\n\n ----------------- \n\n\n\n";
			sm_producer.release();
			return;
	}
	string url = produtos_url.back();
	produtos_url.pop_back();

	sm_producer.release();

	auto r = cpr::Get(cpr::Url{url});

	sm_tempoOcioso.acquire();
	
	tempo_ocioso += r.elapsed;
	
	sm_producer.acquire();
	tempos_produto.push_back(r.elapsed);
	sm_producer.release();

	sm_tempoOcioso.release();


	string html_prod = r.text;
	
	produtos_html.push_back(html_prod);
	sm_pc.release();

//	cout << produtos_html.size() << " -  html\n";
//	cout << produtos_url.size() << " -  url\n";
	
	if(produtos_url.empty()){
			cout << "\n\n\n\n\n ----------------- \n\n\n\n";

			return;
	}	

	produceProd(produtos_url,produtos_html,tempos_produto,sm_pc,sm_producer,sm_tempoOcioso,tempo_ocioso);
	

}

void consumeProd(vector<string>& produtos_url,vector<string>& produtos_html,vector<double>& tempos_produto,Semaphore& sm_pc,Semaphore& sm_consumer,int url_size,int& count_prod,double& tempo_produto_total){
	
	if(count_prod >= url_size){	
		
		return;
	}
				
	sm_pc.acquire();

	chrono::time_point<chrono::high_resolution_clock> start_prod, end_prod;
	Produto produto;	

	sm_consumer.acquire();

	start_prod = chrono::high_resolution_clock::now();
	produto = create_prod(produtos_html.back());
	produtos_html.pop_back();
	end_prod = chrono::high_resolution_clock::now();
    chrono::duration<double> func3 = end_prod - start_prod;

	double tempo_produto_download = tempos_produto.back();
	tempos_produto.pop_back();
	count_prod ++;

	sm_consumer.release(); 	

	double tempo_produto = func3.count()+tempo_produto_download;
	string json = produto.jsonGen();
	cout << json << "\n\n";
	cout << "tempo p/ produto: " << tempo_produto << "s\n";

	sm_consumer.acquire();
	tempo_produto_total += tempo_produto;
	sm_consumer.release();

	cout << "----------//----------//----------//\n\n";
	
	consumeProd(produtos_url,produtos_html,tempos_produto,sm_pc,sm_consumer,url_size,count_prod,tempo_produto_total);

}


int main(int argc, char** argv) {

	//declarações	
	double tempo_ocioso = 0;
	double tempo_produto_total = 0;
	int n_produto = 0;
	int n_pagina = 0;
	vector <string> urls,produtos_url,produtos_html;
	vector <double> tempos_produto;
	chrono::time_point<chrono::high_resolution_clock> start, end, start_oci,end_oci;
	
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


	int n_threads;
	char* num = getenv("NUM_THREADS");
    if(num){
        n_threads = atoi(num);
    }
    else{
        n_threads = 4;
    }

	int n_consumer_threads = n_threads/2;
	int n_producer_threads = n_threads - n_consumer_threads;

	//passa por todas as páginas de produto
	while(url != "last_page"){

		n_pagina ++;
		
		cout << "\n\n----------------------PAGINA "<<n_pagina<<"----------------------\n\n";

		thread consumer_threads[n_consumer_threads];
		thread producer_threads[n_producer_threads];

		Semaphore sm_pc, sm_producer, sm_consumer, sm_tempoOcioso;

		sm_pc.init(0);
		sm_producer.init(1);
		sm_consumer.init(1);
		sm_tempoOcioso.init(1);
		
		produtos_url = getProd(html);
		//cout << produtos_url.size() << "\n";
		int url_size = produtos_url.size() - n_consumer_threads + 1;
		int count_prod = 0;

		for (int i = 0; i < n_producer_threads; ++i)
		{
			producer_threads[i] = thread(produceProd,ref(produtos_url),ref(produtos_html),ref(tempos_produto),ref(sm_pc),ref(sm_producer),ref(sm_tempoOcioso),ref(tempo_ocioso));
		}

		for (int j = 0; j < n_consumer_threads; ++j)
		{
			consumer_threads[j] = thread(consumeProd,ref(produtos_url),ref(produtos_html),ref(tempos_produto),ref(sm_pc),ref(sm_consumer),ref(url_size),ref(count_prod),ref(tempo_produto_total));
		}

		for (int i = 0; i < n_producer_threads; ++i)
		{
			producer_threads[i].join();
		}

		for (int j= 0; j < n_consumer_threads; ++j)
		{
			consumer_threads[j].join();
		}
		n_produto += count_prod;
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

    
} 