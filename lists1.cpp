#include <iostream>
#include <fstream>
#include <string>
using namespace std;
#include "chunk.cpp"

int main()
{
    string str;
    int num;
    cout << "abc" << endl;
    chunk* map = new chunk;
    map->tag="alfa";
    for (char ch=48; ch<58; ch++)
    map->store(state_content, ch);
    map->next = new chunk("bravo","","");
    map->next->next = new chunk("charlie","","");
    chunk* marker = map->search("charlie","","",true);
    chunk* result = marker->insert(new chunk("delta","",""));

    result = result->insert(new chunk("echo","",""));
    //chunk* marker3 = map->last();
    //marker3->next=marker;
    //map->param="b";
    //map->content="c";
    //cout << '<' << map->tag << ' ' << map->param << "> " << map->content << endl << endl;
    //str = map->tag;
    chunk* marker2 = map;
    while (marker2!=NULL)
    {
        chunk* j = marker2->next;
        cout << marker2 << endl;
        marker2 = j;
    };
    
    //cout << "**" << map << "**" << endl;
    return 0;

}