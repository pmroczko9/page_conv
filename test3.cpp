#include <string>
#include <iostream>
using namespace std;

int main(int argc, char* args[])
{
    string argum="";
    int i=0;
    int value=0;
    char c = 0;
    while (*(args[1]+i)!=0)
    {
        c=*(args[1]+i++);
        cout << c << endl;
        argum+=c;
        //cout << value << endl;
    };
    cout << argc << " " << argum << endl;
}