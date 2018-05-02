#include "read_input.hpp"
#include "main.hpp"
#include <random>
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <mpi.h>

#define NGRAM_SIZE 3
#define SIZE_TEXT 300

#define MASTER 0

typedef std::vector<std::string> ngram;
typedef std::map<ngram, int> ngram_map;



int main(int argc, char** argv){

	MPI_Init(&argc,&argv);
	double start;
	double end;
	int size;
	int rank;

	start = MPI_Wtime();

	MPI_Comm_size(MPI_COMM_WORLD, &size); // recupera número de processos
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // recupera o indice (rank) do processo

    std::string file_name = "../wikipedia-small.xml";
    std::vector<std::string> input = read_xml_input(file_name);

    end = MPI_Wtime();
    std::cout << "Leitura de arquivos: " << end - start << "\n";
    start = MPI_Wtime();

    std::vector<ngram> input_ngrams = doc_to_ngram(input, NGRAM_SIZE);
   
    end = MPI_Wtime();
    std::cout << "Criar NGramas: " << end - start << "\n";
    start = MPI_Wtime();

    ngram_map ngrams_mapped = count_ngrams(input_ngrams);

    end = MPI_Wtime();
    std::cout << "Mapear NGramas: " << end - start << "\n";
    start = MPI_Wtime();

    Tree* tree = new Tree();
    tree->create_tree(ngrams_mapped, NGRAM_SIZE);

    end = MPI_Wtime();
    std::cout << "Criar Arvore: " << end - start << "\n";
    start = MPI_Wtime();

    std::string final_text = create_random_text(tree, SIZE_TEXT, NGRAM_SIZE);

    end = MPI_Wtime();
    std::cout << "Gerar Texto Final: " << end - start << "\n";
    start = MPI_Wtime();

    std::cout << final_text << "\n";

    MPI_Finalize();
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

