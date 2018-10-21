#include <string>
#include <boost/regex.hpp>

using namespace std;

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

	    stringstream json_stream;

	    string replace = "\n 		";
	    boost::regex pattern("\n");
 		string descricao_fix = boost::regex_replace(descricao, pattern, replace);

	    json_stream << "{" << "\n" << 
	    			"   " << "\"nome\": \"" 				<< nome <<"\",\n"<<
					"   " << "\"descricao\": \"" 			<< descricao_fix <<"\",\n"<<
					"   " << "\"foto\": \"" 				<< foto << "\",\n" << 
					"   " << "\"preco\": \"" 				<< preco <<"\",\n"<<
					"   " << "\"preco_parcelado\": \"" 		<< preco_parcelado <<"\",\n"<<
					"   " << "\"preco_num_parcelas\": \"" 	<< preco_num_parcelas <<"\",\n"<<
					"   " << "\"categoria\": \"" 			<< categoria <<"\",\n" <<
					"   " << "\"url\": \""	 				<< url <<"\"\n"
			<< "}";

	    string json = json_stream.str();
	    
	    return json;
	}

};