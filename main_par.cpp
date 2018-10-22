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


void produceProd(vector<string>& produtos_url,vector<string>& produtos_html,Semaphore& sm_pc, Semaphore& sm_producer){
	
	//inicio da contagem de tempo para o produto
	//start_prod = chrono::high_resolution_clock::now();
	
	sm_producer.acquire();

	string url = produtos_url.back();

	produtos_url.pop_back();

	sm_producer.release();

	auto r = cpr::Get(cpr::Url{url});

	string html_prod = r.text;
	
	/*
	//marcação do tempo ocioso para cada produto
	end_oci = chrono::high_resolution_clock::now();
	chrono::duration<double> func1 = end_oci - start_prod;
	tempo_ocioso += func1.count();
	*/
	//novo semaforo
	
	
	produtos_html.push_back(html_prod);
	//cout << "-------------P-------------\n";
	//cout << "url - " << produtos_url.size() << "\n";
	//cout << "html - " << produtos_html.size() << "\n";

	sm_pc.release();

	
	if(produtos_url.empty()){
			cout << "\n\n\n\n\n ----------------- \n\n\n\n";
			//sm_pc.release(); 	
			return;
	}
	//in
	

	produceProd(produtos_url,produtos_html,sm_pc,sm_producer);
	

}

void consumeProd(vector<string>& produtos_url,vector<string>& produtos_html,Semaphore& sm_pc,Semaphore& sm_consumer,int url_size,int& count_prod){
	
	if(count_prod >= url_size){	
		return;
	}		
	sm_pc.acquire();

	Produto produto;	

	sm_consumer.acquire();
	produto = create_prod(produtos_html.back());
	produtos_html.pop_back();
	count_prod ++;


	//retorna um objeto no formato json com informações do produto
	sm_consumer.release(); 	
	string json = produto.jsonGen();
	cout << json << "\n\n";
	//cout << "-------------C-------------\n";
	//cout << "url - " << produtos_url.size() << "\n";
	//cout << "html - " << produtos_html.size() << "\n";

	/*
	//marcação do tempo para cada produto
	end_prod = chrono::high_resolution_clock::now();
	chrono::duration<double> func2 = end_prod - start_prod;
	cout << "tempo p/ produto: " << func2.count() << "s\n";
	tempo_produto_total += func2.count();
	*/
	
	//contagem de produtos
	//cout << "numero do produto: " <<n_produto << "\n\n";
	
		

	cout << "----------//----------//----------//\n\n";
	
	consumeProd(produtos_url,produtos_html,sm_pc,sm_consumer,url_size,count_prod);

}


int main(int argc, char** argv) {

	//declarações	
	vector <string> urls,produtos_url,produtos_html;
	double tempo_ocioso = 0;
	double tempo_produto_total = 0;
	int n_produto = 0;
	chrono::time_point<chrono::high_resolution_clock> start, end, start_prod, end_prod, start_oci, end_oci;
	
	//inicio da contagem de tempo total do programa
	start = chrono::high_resolution_clock::now();

	//ṕegar url como entrada
	string url = "https://www.submarino.com.br/categoria/instrumentos-musicais/equipamento-para-dj";
	//string url = argv[1];

	//marcação de tempo ocioso
	start_oci = chrono::high_resolution_clock::now();

	//pegar o html da pagina inicial
	auto r = cpr::Get(cpr::Url{url});
	
	//marcação de tempo ocioso
	end_oci = chrono::high_resolution_clock::now();
	chrono::duration<double> func0 = end_oci - start_oci;
	tempo_ocioso += func0.count();

	//html da pagina inicial
	string html = r.text;

	int n_threads = 8;
	int n_consumer_threads = n_threads/2;
	int n_producer_threads = n_threads - n_consumer_threads;

	//passa por todas as páginas de produto
	while(url != "last_page"){

		cout << "-------------------------------------------\n";
		cout << "\n\n" << url<<"\n\n";

		produtos_url = getProd(html);

		thread consumer_threads[n_consumer_threads];
		thread producer_threads[n_producer_threads];

		Semaphore sm_pc, sm_producer, sm_producer2, sm_consumer;

		sm_pc.init(0);
		sm_producer.init(1);
		//ssm_producer2.init(1);
		sm_consumer.init(1);
		
		produtos_url = getProd(html);
		int url_size = produtos_url.size() - n_consumer_threads + 1;
		int count_prod = 0;

		for (int i = 0; i < n_producer_threads; ++i)
		{
			producer_threads[i] = thread(produceProd,ref(produtos_url),ref(produtos_html),ref(sm_pc),ref(sm_producer));
		}

		for (int j = 0; j < n_consumer_threads; ++j)
		{
			consumer_threads[j] = thread(consumeProd,ref(produtos_url),ref(produtos_html),ref(sm_pc),ref(sm_consumer),ref(url_size),ref(count_prod));
		}

		for (int i = 0; i < n_producer_threads; ++i)
		{
			producer_threads[i].join();
		}

		for (int j= 0; j < n_consumer_threads; ++j)
		{
			consumer_threads[j].join();
		}

		//proxima pagina
		urls.push_back(url);
		url = nextPage(html,url);
		auto r = cpr::Get(cpr::Url{url});
		html = r.text;

	

	}

	cout << "tempo médio por produto: " << tempo_produto_total/n_produto << "\n";
	cout << "tempo total ocioso: " << tempo_ocioso << "s\n";

	//marcação do tempo total para o programa
	end = chrono::high_resolution_clock::now();
    chrono::duration<double> func3 = end - start;
    cout << "tempo total: " << func3.count() << "s\n";

    
} 