#include "pugixml.hpp"
#include <boost/tokenizer.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include "read_input.hpp"

typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

std::vector<std::string> read_xml_input(std::string &file_name)
{
    // std::cout << "Inicio" << "\n";
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(file_name.c_str());
    // std::cout << "Arquivo Lido" << "\n";
    std::vector<std::string> doc_list;
    if (!result) {
        std::cout << "Erro" << "\n";
        return doc_list; 
    }
    for (pugi::xml_node page: doc.child("mediawiki").children("page"))
    { 
        std::string text = page.child("revision").child_value("text");
        clean_text(text);
        std::vector<std::string> words;
        words = tokenize_text(text);
        doc_list.insert(doc_list.end(), words.begin(), words.end());
    }
    // std::cout << "Final" << "\n";
    return doc_list;
}

std::vector<std::string> tokenize_text(std::string& text){
    tokenizer tok{text};
    std::vector<std::string> words;
    for (tokenizer::iterator it = tok.begin(); it != tok.end(); ++it)
        words.push_back(*it);
    return words;
}

void clean_text(std::string& text){
    std::replace_if(text.begin(), text.end(),      
                        remove_special_characters,
                        ' '
                       );
    std::transform(text.begin(), text.end(), text.begin(), ::tolower);
}

bool remove_special_characters(char c){
    return !is_punct(c) && !std::isalpha(c);
}

bool is_punct(char c){
    return c == '.' || c == ',' || c == '!' || c == '?';
}