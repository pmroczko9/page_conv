#include <iostream>
#include <string>
//#include <algorithm>

using namespace std;

string string_to_lower(const string& str)
{
    string lower;
    for (size_t i=0; i<str.length();i++)
    {
        lower+=tolower(str.at(i));
    }
    return lower;
}

bool custom_find(string where, string what)
{
    string new_where = string_to_lower(where);
    string new_what = string_to_lower(what);
    if (new_where.find(new_what)==string::npos)
    return false;
    else return true;
}

int main()
{
    string a = "Head";
    string b = "";
    
    cout << a << endl;
    
    bool res = custom_find(a,"head");

    cout << res;
    
    //if (res == string::npos)
    //cout << " Not found";
    //else cout << " Found";
    
    cout << endl;
}