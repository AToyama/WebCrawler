#ifndef PRODUTO_H
#define PRODUTO_H

#include <string>
#include <boost/regex.hpp>

using namespace std;

string nomeProd(string);

string descricaoProd(string);

string fotoProd(string);

double precoProd(string);

double parceladoProd(string);

int parcelasProd(string);

string categoriaProd(string);

string urlProd(string);

class Produto {

public:

	string nome;
	string descricao;
	string foto;
	double preco;
	double preco_parcelado;
	int preco_num_parcelas;
	string categoria;
	string url;
	string json;

	string jsonGen(){

		string precof,preco_parceladof,preco_num_parcelasf;

		if (preco == 0){
			precof = "indisponivel";
		}
		else{
			precof = to_string(preco);
		}

		if (preco_parcelado == 0){
			preco_parceladof = "indisponivel";
			preco_num_parcelasf = "indisponivel";
		}
		else{
			preco_parceladof = to_string(preco_parcelado);
			preco_num_parcelasf = to_string(preco_num_parcelas);	
		}

	    stringstream json_stream;

	    string replace = "\n 		";
	    boost::regex pattern("\n");
 		string descricao_fix = boost::regex_replace(descricao, pattern, replace);

	    json_stream << "{" << "\n" << 
	    			"   " << "\"nome\": \"" 				<< nome <<"\",\n"<<
					"   " << "\"descricao\": \"" 			<< descricao_fix <<"\",\n"<<
					"   " << "\"foto\": \"" 				<< foto << "\",\n" << 
					"   " << "\"preco\": \"" 				<< precof <<"\",\n"<<
					"   " << "\"preco_parcelado\": \"" 		<< preco_parceladof <<"\",\n"<<
					"   " << "\"preco_num_parcelas\": \"" 	<< preco_num_parcelasf <<"\",\n"<<
					"   " << "\"categoria\": \"" 			<< categoria <<"\",\n" <<
					"   " << "\"url\": \""	 				<< url <<"\"\n"
			<< "}";

	    string json = json_stream.str();
	    
	    return json;
	}

};

Produto create_prod(string);

#endif