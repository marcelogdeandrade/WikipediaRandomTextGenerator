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


typedef std::vector<std::string> ngram;
typedef std::map<ngram, int> ngram_map;


int main(){

	mpi::environment env;
	mpi::communicator world;
	std::vector<std::string> input_send;
	std::vector<std::string> input_recv;
	std::vector<ngram> input_ngrams;
	mpi::timer timer;
	if (world.rank() == 0){
		timer.restart();
	    std::string file_name = "../wikipedia-small.xml";
	    input_send = read_xml_input(file_name);
     	std::cout << "Leitura de arquivos: " << timer.elapsed() << "\n";
	    int slice = input_send.size() / (world.size() - 1);
	    int partition = 0;
		timer.restart();
	    for (int i = 1; i < world.size(); i++){
	    	std::vector<std::string> aux_input_send (input_send.begin() + partition, input_send.begin() + partition + slice);
	    	world.send(i, TAG, aux_input_send);
	    	partition += slice;
	    }
	    ngram_map all_ngrams_mapped;
	    for (int i = 1; i < world.size(); i++){
	    	ngram_map recv_ngrams_mapped;
	    	world.recv(i, TAG, recv_ngrams_mapped);
	    	for (auto it=recv_ngrams_mapped.begin(); it!=recv_ngrams_mapped.end(); ++it) {
			  if (all_ngrams_mapped[it->first])
			    all_ngrams_mapped[it->first] += it->second;
			  else
			    all_ngrams_mapped[it->first] = it->second;
			}
	    }

     	std::cout << "Criar + Mapear NGramas: " << timer.elapsed() << "\n";
     	timer.restart();

     	Tree* tree = new Tree();
     	tree->create_tree(all_ngrams_mapped, NGRAM_SIZE);

     	std::cout << "Criar Arvore: " << timer.elapsed() << "\n";
     	timer.restart();

     	std::string final_text = create_random_text(tree, SIZE_TEXT, NGRAM_SIZE);

     	std::cout << "Gerar Texto: " << timer.elapsed() << "\n";
     	timer.restart();

     	std::cout << final_text << "\n";
	} else {
		world.recv(0, TAG, input_recv);
		input_ngrams = doc_to_ngram(input_recv, NGRAM_SIZE);
     	ngram_map ngrams_mapped = count_ngrams(input_ngrams);
		world.send(0, TAG, ngrams_mapped);
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

