#include <iostream>
#include <fstream>
#include <string>
using namespace std;

enum state_enum:char {state_tag, state_param, state_content};

//class chunk;

class chunk
{
    public:
    string tag;
    string param;
    string content;
    chunk *prev;
    chunk *next;
    chunk();
    chunk(string newtag, string newparam, string newcontent);
    void store(char state, char ch);
    string toString(void) const;
    friend ostream& operator<<(ostream& os, const chunk* ptr);
    chunk* last(void);
    chunk* insert(chunk* what);
    chunk* remove(chunk* what);
};

chunk::chunk()
{
    tag = "";
    param = "";
    content = "";
    prev = NULL;
    next = NULL;
}

chunk::chunk(string newtag, string newparam, string newcontent)
{
    tag = newtag;
    param = newparam;
    content = newcontent;
    prev = NULL;
    next = NULL;
}

void chunk::store(char state, char ch)
{
    switch(state)
    {
    case state_tag:     tag += ch;
                        break;
    case state_param:   param += ch;
                        break;
    case state_content: content += ch;
    }
    return;
}

string chunk::toString(void) const
{
    string str = '<' + tag;
    if (param == "") str += '>'; else str += (' ' + param + '>');
    if (content != "") str += (' ' + content);
    return str;
}

chunk* chunk::last(void)
{
    chunk* j = this;
    chunk* k = NULL;
    while (j->next != NULL)
    {
        k=j->next;
        j=k;
    }
    return j;
}

chunk* chunk::insert(chunk* what)
{
    chunk* marker = next;
    next = what;
    chunk* ptr_last = what->last();
    ptr_last->next=marker;
    return ptr_last;
}

chunk* chunk::remove(chunk* what) //
{
    chunk* ptr;

}

ostream& operator<< ( ostream& outs, const chunk* ptr ) {
	return outs << ptr->toString();
}

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
    chunk* marker = new chunk("delta","","");
    chunk* result = map->insert(marker);

    marker = new chunk("echo","","");
    result->insert(marker);
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