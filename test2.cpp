#include <iostream>
#include <string>
#include <regex>

using namespace std;

int main()
{
    /*cout << "Hello World" << endl;*/

    string a = "Head bang";
    regex pattern("//d");
    smatch base_match;
    
    string b = "111";

    cout << regex_match(a, base_match, pattern) << ' ' << base_match[0].str() << ' ' << regex_match(b, base_match, pattern) << ' ' << base_match[0].str() << endl;



    //cout <<(a.find("head"))<< ' ' << (a.find("")) << ' ' << (b.find('b'))<< ' ' << (b.find("")) << endl;

    /*const std::string fnames[] = {"foo.txt", "bar.txt", "baz.dat", "zoidberg"};
    const std::regex txt_regex("[a-z]+\\.txt");
 
    for (const auto &fname : fnames) {
        std::cout << fname << ": " << std::regex_match(fname, txt_regex) << '\n';
    }   
 
    // Extraction of a sub-match
    const std::regex base_regex("([a-z]+)\\.txt");
    std::smatch base_match;
 
    for (const auto &fname : fnames) {
        if (std::regex_match(fname, base_match, base_regex)) {
            // The first sub_match is the whole string; the next
            // sub_match is the first parenthesized expression.
            if (base_match.size() == 2) {
                std::ssub_match base_sub_match = base_match[1];
                std::string base = base_sub_match.str();
                std::cout << fname << " has a base of " << base << '\n';
            }
        }
    }
 
    // Extraction of several sub-matches
    const std::regex pieces_regex("([a-z]+)\\.([a-z]+)");
    std::smatch pieces_match;
 
    for (const auto &fname : fnames) {
        if (std::regex_match(fname, pieces_match, pieces_regex)) {
            std::cout << fname << '\n';
            for (size_t i = 0; i < pieces_match.size(); ++i) {
                std::ssub_match sub_match = pieces_match[i];
                std::string piece = sub_match.str();
                std::cout << "  submatch " << i << ": " << piece << '\n';
            }   
        }   
    }*/   
}