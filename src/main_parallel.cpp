#include "read_input.hpp"
#include "main.hpp"
#include <random>
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <boost/mpi.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>

namespace mpi = boost::mpi;


#define NGRAM_SIZE 3
#define SIZE_TEXT 300
#define TAG 42

#define MASTER 0

typedef std::vector<std::string> ngram;
typedef std::map<ngram, int> ngram_map;



int main(){

	mpi::environment env;
	mpi::communicator world;

	mpi::timer timer;
	mpi::timer timer_full;

    if (world.rank() != MASTER){
	    std::string file_name = "files_mini_small/wikipedia-mini-small_" + std::to_string(world.rank()) + ".xml" ;
	    std::cout << file_name << "\n";
	    std::vector<std::string> input = read_xml_input(file_name);

	    std::cout << "Leitura de arquivos: " << timer.elapsed() << "\n";
	    timer.restart();

	    std::vector<ngram> input_ngrams = doc_to_ngram(input, NGRAM_SIZE);
	   
	    std::cout << "Criar NGramas: " << timer.elapsed() << "\n";
	    timer.restart();

    	ngram_map ngrams_mapped = count_ngrams(input_ngrams);
    	world.send(MASTER, TAG, ngrams_mapped);
    	std::cout << "Mandei" << "\n";
    } else{
    	std::vector<ngram_map> vector_ngram_map;
		ngram_map ngrams_mapped;
    	for (int i = 1; i < world.size(); i++){
    		world.recv(i, TAG, ngrams_mapped);
    		vector_ngram_map.push_back(ngrams_mapped);
    		std::cout << "Recebi" << "\n";
    	}
		ngrams_mapped = concatenate_ngram_maps(vector_ngram_map);
		std::cout << "Ngramas concatenados" << "\n";
	    Tree* tree = new Tree();
    	tree->create_tree(ngrams_mapped, NGRAM_SIZE);

    	std::cout << "Criar Arvore: " << timer.elapsed() << "\n";
    	timer.restart();

	    std::string final_text = create_random_text(tree, SIZE_TEXT, NGRAM_SIZE);

	    std::cout << "Gerar Texto Final: " << timer.elapsed() << "\n";
    	timer.restart();

    	std::cout << "\n" << final_text << "\n";

    	std::cout << "TEMPO TOTAL: " << timer_full.elapsed() << "\n";
    }
    
    return 0;
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

ngram_map concatenate_ngram_maps(std::vector<ngram_map> vector_ngram_map){
	ngram_map result;
	for(std::vector<ngram_map>::iterator vec_it = vector_ngram_map.begin(); vec_it != vector_ngram_map.end(); ++vec_it) {
		ngram_map aux = *vec_it;
		for (ngram_map::iterator map_it = aux.begin(); map_it != aux.end(); map_it++ )
			{
				if (result.count(map_it->first) == 0){
					result[map_it->first] = map_it->second;
				} else{
					result[map_it->first] += map_it->second;
				}
			}
	}
	return result;
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

