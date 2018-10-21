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

using namespace std;

//adquirir nome do produto
string nomeProd(string html){

	boost::regex nomeExp("<title>(.+?) no Submarino.com</title>");
	boost::smatch result;
	if (boost::regex_search(html, result, nomeExp)) {
		string submatch(result[1]);
		return submatch;
	}
	else{
		return "";
	}
}

//adquirir descricao do produto
string descricaoProd(string html){

	boost::regex descricaoExp("<noframes>(.+?)</noframes>");
	boost::smatch result;
	
	if (boost::regex_search(html, result, descricaoExp)) 
	{
		string submatch(result[1]);
		string replace = "\n";
		boost::regex pattern1("<Br />");
		boost::regex pattern2("<br/>");
		boost::regex pattern3("<BR>");
		
 		string treat1 = boost::regex_replace(submatch, pattern1, replace);
 		string treat2 = boost::regex_replace(treat1, pattern2, replace);
 		string treat3 = boost::regex_replace(treat2, pattern3, replace);
 		
 		return treat3;
	}
	
	else 
	{
		return "";
	}
}

//adquirir url da foto do produto
string fotoProd(string html){

	boost::regex fotoExp("Foto 1(.+?) src=\"(.+?)\"/>");
	boost::smatch result;
	if (boost::regex_search(html, result, fotoExp)) {
		string submatch(result[2]);
		return submatch;
	}
	else{
		return "";
	}
}

//adquirir preco do produto
double precoProd(string html){

	boost::regex precoExp("sales-price\">R[$](.*?)<");
	boost::smatch result;
	if (boost::regex_search(html, result, precoExp)) {
		
		string submatch(result[1]);
		
		boost::regex pattern1("[.]");
		string treat1 = boost::regex_replace(submatch, pattern1, "");
		
		boost::regex pattern2(",");
		string treat2 = boost::regex_replace(treat1, pattern2, ".");

		return stod(treat2);
	}
	else{
		return 0.0;
	}
}

//adquirir preco parcelado do produto
double parceladoProd(string html){

	boost::regex parceladoExp("x de R[$](.*?) s/ juros");
	boost::smatch result;
	if (boost::regex_search(html, result, parceladoExp)) {
		
		string submatch(result[1]);
		
		boost::regex pattern1("[.]");
		string treat1 = boost::regex_replace(submatch, pattern1, "");
		
		boost::regex pattern2(",");
		string treat2 = boost::regex_replace(treat1, pattern2, ".");

		return stod(treat2);
	}
	else{
		return 0.0;
	}
}

//adquirir numero de parcelas do produto
int parcelasProd(string html){

	boost::regex parcelasExp("payment-option-rate\">(.*?)x de R[$]");
	boost::smatch result;
	
	if (boost::regex_search(html, result, parcelasExp)) {
		
		string submatch(result[1]);
		if (submatch.size() > 3) {
			return 1;
		}

		else{
			return stoi(submatch);
		}	
	}
	else{
		return 0;
	}
}

//adquirir categoria do produto
string categoriaProd(string html){

	boost::regex categoriaExp("url\":\"\/categoria\/(.*?)\/(.*?)\"");
	boost::smatch result;
	if (boost::regex_search(html, result, categoriaExp)) {
		string submatch(result[2]);

		return submatch;
	}
	else{
		return "";
	}
}

//adquirir url da pagina do produto
string urlProd(string html){

	boost::regex urlExp("\"url\":\"https:\/\/(.*?)\",");
	boost::smatch result;
	if (boost::regex_search(html, result, urlExp)) {
		string submatch(result[1]);
		string full("https://"+submatch);

		return full;
	}
	else{
		return "";
	}
}

//adquirir a proxima pagina de produto
string nextPage(string html, string url){
	
	boost::regex urlExp("li class=\"active\"><a href=(.*?)<li class=\"(.*?)\">");
	boost::smatch check;

	if (boost::regex_search(html, check, urlExp)) {
		string submatch(check[2]);		
		if(submatch == "disabled"){
			return "last_page";
		}
	}

	boost::regex nexturlExp("li class=\"active\"><a href=\"(.*?)<a href=\"(.*?)\">");
	boost::smatch result;
	if (boost::regex_search(html, result, nexturlExp)) {
		string submatch(result[2]);
		boost::regex pattern("amp;");
		string treat = boost::regex_replace(submatch, pattern, "");
		return "https://www.submarino.com.br" + treat;
	}
	else{
		return "";
	}
}

//lista todos os produtos de uma pagina
vector<string> getProd(string html){

    boost::regex regex("<a class=\"card-product-url\" href=\"(.*?)\">");

    boost::sregex_token_iterator iter(html.begin(), html.end(), regex, 1);
    boost::sregex_token_iterator end;

    vector<string> produtos;

    for( ; iter != end; ++iter ) {
        string raw_url = *iter;
        boost::regex pattern("amp;");
		string treat = boost::regex_replace(raw_url, pattern, "");
		produtos.push_back("https://www.submarino.com.br" + treat);
    }
    return produtos;
}

int main(int argc, char** argv) {
	
	//ṕegar url como entrada
	string url = "https://www.submarino.com.br/categoria/suplementos-e-vitaminas/fitoterapicos-e-funcionais/oleo-de-coco?ordenacao=topSelling&origem=omega";
	//string url = argv[1];
	cout << url << "\n";
	//pegar o html da pagina inicial
	auto r = cpr::Get(cpr::Url{url});
	string html = r.text;
	nextPage(html,url);
	
	ofstream out("output.txt");
	out << html;
	out.close();

	vector <string> urls,produtos;
	int joj = 0;

	chrono::time_point<chrono::high_resolution_clock> start, end;
    start = chrono::high_resolution_clock::now();

	//passa por todas as páginas de produto
	while(url != "last_page"){

		//pega a lista de produtos da pagina
		produtos = getProd(html);		

		// gerar o json para cada produto
		for (int i = 0; i < produtos.size(); ++i)
		{
			auto r = cpr::Get(cpr::Url{produtos[i]});
			string html_prod = r.text;
			Produto produto;
			produto.nome = nomeProd(html_prod);
			produto.descricao = descricaoProd(html_prod);
			produto.foto = fotoProd(html_prod);
			produto.preco = precoProd(html_prod);
			produto.preco_parcelado = parceladoProd(html_prod);
			produto.preco_num_parcelas = parcelasProd(html_prod);
			produto.categoria = categoriaProd(html_prod);
			produto.url = urlProd(html_prod);
			joj++;
			string json = produto.jsonGen();
			cout << json << "\n\n";
			cout << joj << "\n\n";
			cout << "----------//----------//----------//\n\n";
		}

		//proxima pagina
		urls.push_back(url);
		url = nextPage(html,url);
		auto r = cpr::Get(cpr::Url{url});
		html = r.text;
	}

	end = chrono::high_resolution_clock::now();
    chrono::duration<double> func0 = end - start;
    cout << "time: " << func0.count() << "s\n";
    
} 