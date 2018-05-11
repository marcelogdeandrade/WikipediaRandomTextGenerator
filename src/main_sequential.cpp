#include "read_input.hpp"
#include "main.hpp"
#include <random>
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <boost/mpi.hpp>

namespace mpi = boost::mpi;


#define NGRAM_SIZE 4
#define SIZE_TEXT 300

#define MASTER 0

typedef std::vector<std::string> ngram;
typedef std::map<ngram, int> ngram_map;



int main(){

	mpi::environment env;
	mpi::communicator world;

	mpi::timer timer;
	mpi::timer timer_full;

    std::string file_name = "files_mini_small/wikipedia-mini-small_full.xml";
    std::vector<std::string> input = read_xml_input(file_name);

    auto time_file_read = timer.elapsed();
    std::cout << "Leitura de arquivos: " << time_file_read << "\n";
    timer.restart();

    std::vector<ngram> input_ngrams = doc_to_ngram(input, NGRAM_SIZE);
   	
   	auto time_create_ngrams = timer.elapsed();
    std::cout << "Criar NGramas: " << time_create_ngrams << "\n";
    timer.restart();

    ngram_map ngrams_mapped = count_ngrams(input_ngrams);

    auto time_map_ngrams = timer.elapsed();
    std::cout << "Mapear NGramas: " << time_map_ngrams << "\n";
    timer.restart();

    int mem_usaga_map = get_mem_usage_map(ngrams_mapped);

    std::cout << "Tamanho dos ngramas: " << mem_usaga_map << "\n";


    Tree* tree = new Tree();
    tree->create_tree(ngrams_mapped, NGRAM_SIZE);

    auto time_create_tree = timer.elapsed();
    std::cout << "Criar Arvore: " << time_create_tree << "\n";
    timer.restart();

    std::string final_text = create_random_text(tree, SIZE_TEXT, NGRAM_SIZE);

    auto time_generate_text = timer.elapsed();
    std::cout << "Gerar Texto Final: " << time_generate_text << "\n";
    timer.restart();

    std::cout << "\n" << final_text << "\n";

    auto total_time = time_file_read +
    				  time_create_ngrams +
    				  time_map_ngrams +
    				  time_create_tree + 
    				  time_generate_text;

    std::cout << "\n" << "------------------------------------------" << "\n" << "Tempos Percentuais: " << "\n";
    std::cout << "Leitura de arquivos: " << (time_file_read * 100) / total_time << "%\n";
    std::cout << "Criação dos ngrams: " << (time_create_ngrams * 100) / total_time << "%\n";
    std::cout << "Mapear ngramas: " << (time_map_ngrams * 100) / total_time << "%\n";
    std::cout << "Criação da árvore: " << (time_create_tree * 100) / total_time << "%\n";
    std::cout << "Gera texto: " << (time_generate_text * 100) / total_time << "%\n";
    std::cout << "TEMPO TOTAL: " << timer_full.elapsed() << "\n";
    MPI_Finalize();
    return 0;
}

int get_mem_usage_map(ngram_map ngrams_mapped){
	int total_size = 0;
	for (ngram_map::iterator it = ngrams_mapped.begin(); it != ngrams_mapped.end(); it++ ){
		ngram ngram_vector = it->first;
		for (unsigned i = 0; i < ngram_vector.size(); i++){
			total_size += sizeof(ngram_vector[i]);
		}
		total_size += sizeof(it->second);
	}
	return total_size;
}

std::vector<ngram> doc_to_ngram(std::vector<std::string> &doc, int n){
	std::vector<ngram> doc_ngrams;
	for (unsigned int i = 0; i < doc.size() - n + 1; i ++){
		std::vector<std::string> tokens;
		for (int j = 0; j < n; j ++){
			tokens.push_back(doc[i + j]);
		}
		doc_ngrams.push_back(tokens);
	}
	return doc_ngrams;
}

// Desempenho disso está MUITO ruim
ngram_map count_ngrams(std::vector<ngram> ngrams){
	ngram_map result;
	for (unsigned int  i = 0; i < ngrams.size(); i++){
		if (result.count(ngrams[i]) == 0){
			// int num_occur = std::count(ngrams.begin(), ngrams.end(), ngrams[i]);
			result[ngrams[i]] = 1;
		} else {
			result[ngrams[i]] += 1;
		}
	}
	return result;
}

std::string create_random_text(Tree* tree,int length, int size_ngram){
	Node* node = tree->root;
	std::string result = "";
	std::vector<std::string> result_vector;
	// Primeiras N palavras (melhorar essa parte)
	for (int i = 0; i < size_ngram; i ++){
		std::vector<int> probs;
		std::vector<std::string> words;
		for (std::map<std::string, Node*>::iterator it = node->children.begin(); it != node->children.end(); it++ )
		{
			probs.push_back(it->second->count);
			words.push_back(it->first);
		}
		std::random_device rd;
		std::mt19937 gen(rd());
		std::discrete_distribution<int> d(probs.begin(), probs.end());
		int index = d(gen);
		result_vector.push_back(words[index]);
		node = node->children[words[index]];
	}
	for (int i = 0; i < length; i ++){
		node = tree->root;
		std::string key;
		for (int j = -size_ngram + 1; j < 0; j++){
			key = result_vector.end()[j];
			node = node->children[key];
		}
		std::vector<int> probs;
		std::vector<std::string> words;
		for (std::map<std::string, Node*>::iterator it = node->children.begin(); it != node->children.end(); it++ )
		{
			probs.push_back(it->second->count);
			words.push_back(it->first);
		}
		std::random_device rd;
		std::mt19937 gen(rd());
		std::discrete_distribution<int> d(probs.begin(), probs.end());
		int index = d(gen);
		result_vector.push_back(words[index]);
	}
	for (unsigned int i = 0; i < result_vector.size(); i++){
		result += " " + result_vector[i];
	}
	return result;
}

