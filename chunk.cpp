enum state_enum:char {state_tag, state_param, state_content};

string string_to_lower(const string& str)
{
    string lower;
    for (size_t i=0; i<str.length();i++)
    {
        lower+=tolower(str.at(i));
    }
    return lower;
}

string extract_filename(string src, string key)
{
  string res="";
  size_t pos = src.find(key);
  pos+=key.length()+1;
  int temp=0;
  char ch = 0;
  while (ch!='\"')
  {
    ch=src.at(pos+temp++);
    if (ch!='\"') res+=ch;
  };
  return res;
}

bool custom_find(string where, string what)
{
    //we're assuming the querry (what) will already be lowercase
    string new_where = string_to_lower(where);
    //string new_what = string_to_lower(what);
    if (new_where.find(what)==string::npos)
    return false;
    else return true;
}

//class chunk;

class chunk
{
    public:
    string tag;
    string param;
    string content;
    chunk *next;
    chunk();
    chunk(string newtag, string newparam, string newcontent);
    void store(char state, char ch);
    string toString(void) const;
    friend ostream& operator<<(ostream& os, const chunk* ptr);
    chunk* last(void);
    chunk* insert(chunk* what);
    chunk* search(string s_tag, string s_param, string s_content, bool switch_or);
};

chunk::chunk()
{
    tag = "";
    param = "";
    content = "";
    next = NULL;
}

chunk::chunk(string newtag, string newparam, string newcontent)
{
    tag = newtag;
    param = newparam;
    content = newcontent;
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

chunk* chunk::search(string s_tag, string s_param, string s_content, bool switch_or)
{
    chunk* ptr = this;
    bool result = false;
    bool res_tag = (s_tag == "")?(not switch_or):false;
    bool res_param = (s_param == "")?(not switch_or):false;
    bool res_content = (s_content == "")?(not switch_or):false;
    do
    {
      if (s_tag != "") res_tag = custom_find(ptr->tag, s_tag);
      if (s_param != "") res_param = custom_find(ptr->param, s_param);
      if (s_content != "") res_content = custom_find(ptr->content, s_content);
      if (switch_or)
      result = (res_tag || res_param || res_content);
      else result = (res_tag && res_param && res_content);
      if (!result) ptr=ptr->next;
    } while (result == false && ptr!=NULL);
    return ptr;
    
}

ostream& operator<< ( ostream& outs, const chunk* ptr ) {
	return outs << ptr->toString();
}

// class gallery

class pic
{
  public:
  string thumbnail;
  string image;
  string alt_text;
  pic* next;
  pic();
};

pic::pic()
{
  thumbnail = "";
  image = "";
  alt_text = "";
  next = NULL;
}
