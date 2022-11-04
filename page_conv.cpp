// reading a text file
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
#include "chunk.cpp"

#define STATUS_NORMAL 0
#define STATUS_INPUT_ERROR 1
#define STATUS_OUTPUT_ERROR 2

string argtostring(char* arg)
{
  int i=0;
  char c=0;
  string res="";
  while (*(arg+i))
  {
    res+=*(arg+i++);
  }
  return res;
}

int main (int argc, char* args[])
{
  string filename;
  filename = (argc>1)?(argtostring(args[1])):"";
  cout << filename << endl;
  
  //string tag;
  bool in_tag=false;
  bool tag_flag;
  //string parameter;
  //string big;
  char c;
  int tag_level = 0;
  int line = 0;
  int state = 0;
  chunk* map = new chunk;
  chunk* marker = map;
  int status = STATUS_NORMAL;
  ifstream infile (filename); //("..//html//M48en.htm");
  ofstream outfile ("output.txt");
  bool tag_first_time = true;
  
  // Read HTML file into the (chunk* map) structure

  if (infile.is_open())
  {
    while ( infile >> noskipws >> c )
    {
      switch(c)
      { case '<': state = state_tag;
                  in_tag = true;
                  if (!tag_first_time)
                  {
                  marker->next=new chunk;
                  marker=marker->next;
                  }
                  tag_first_time = false;
                  line++;
                  break;
        case '>': state = state_content;
                  in_tag = false;
                  break;
        case 0x0A:
        case 0x0D: //cout << line << ' ' << state << endl;
                  c = ' ';
        case ' ': if (state==state_tag)
                  {
                      state = state_param;
                      break;
                  };
       
        default:  marker->store(state, c);
      }
    }
    infile.close();
  }
  else status = status | STATUS_INPUT_ERROR; 

// *** File conversion ***

// meta tags

  chunk* meta_tag_result = map->search("meta","","",true); // look once
  chunk* backup_result = NULL;

  if (meta_tag_result!=NULL)
  {
    //found!
    cout << meta_tag_result << endl;
    //making it the UTF-8 tag
    meta_tag_result->param = "http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"";
    backup_result=meta_tag_result;
    meta_tag_result->next->search("meta","","",true); // look second time
    if (meta_tag_result!=NULL)
    {
      //found again!
      cout << meta_tag_result << endl;
      //making it the vieport tag
      meta_tag_result->param = "name=\"viewport\" content=\"width=device-width, initial-scale=1\"";
    }
    else
    {
      cout << "Second <Meta> tag not found, inserting" << endl;
      backup_result->insert(new chunk("meta", "name=\"viewport\" content=\"width=device-width, initial-scale=1\"", ""));
    }
  }
  else
  { 
    cout << "<Meta> tag not found!" << endl;
    meta_tag_result = map->search("head","","",true);
    cout << "Inserting charset tag" << endl;
    meta_tag_result = meta_tag_result->insert(new chunk("meta", "http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"", ""));
    cout << "Inserting viewport tag" << endl;
    meta_tag_result->insert(new chunk("meta", "name=\"viewport\" content=\"width=device-width, initial-scale=1\"", ""));
  }
  
// update css - link tags

chunk* link_tag_result = map->search("link","","",true); //look once
if (link_tag_result!=NULL)
{
  //found!
  cout << link_tag_result << endl;
  //making it the index2.css tag
  link_tag_result->param = "rel=\"STYLESHEET\" type=\"text/css\" href=\"index2.css\" title=\"Military\"";
  backup_result=link_tag_result;
  // look second time
  link_tag_result=link_tag_result->next->search("link","","",true);
  if (link_tag_result!=NULL)
  {
    //found again!
    cout << meta_tag_result << endl;
    //making it the lightbox tag
    link_tag_result->param = "href=\"lightbox2/dist/css/lightbox.css\" rel=\"stylesheet\"";
  }
  else
  {
    cout << "Second <link> tag not found, inserting" << endl;
    backup_result->insert(new chunk("link", "href=\"lightbox2/dist/css/lightbox.css\" rel=\"stylesheet\"", ""));
  }
}
else
{ 
  cout << "<link> tag not found!" << endl;
  link_tag_result = map->search("head","","",true);
  cout << "Inserting index2.css tag" << endl;
  link_tag_result = link_tag_result->insert(new chunk("link", "rel=\"STYLESHEET\" type=\"text/css\" href=\"index2.css\" title=\"Military\"", ""));
  cout << "Inserting lightbox.css tag" << endl;
  link_tag_result->insert(new chunk("link", "href=\"lightbox2/dist/css/lightbox.css\" rel=\"stylesheet\"", ""));
}

  
// div class="tabl" - get picture names and rewrite gallery

chunk* gallery_result = map->search("div","tabl","",false);
chunk* gallery_limit = NULL;
bool gal_first_time = true;
bool pic_first_time = true;
pic* pic_gallery = new pic();
pic_gallery->image="*";
pic_gallery->thumbnail="*";
pic* pic_ptr = pic_gallery;

  if (gallery_result!=NULL)
  {
     cout << gallery_result << endl;
     gallery_limit = map->search("div", "clear", "", false);
  }
  else cout << "Gallery not found!" << endl;


  if (gallery_result!=NULL && gallery_limit!=NULL)
  {
    chunk* gal_ptr = gallery_result;
    chunk* gal_delete = NULL;
    int gal_state = 0;
    bool res_tag = false;
    string new_tag = "";
    while (gal_ptr!=gallery_limit)
    {
      switch (gal_state)
      {
      case 0: //looking for <a>
              res_tag = ((gal_ptr->tag=="a") || (gal_ptr->tag=="A"));
              if (res_tag)
              {
                if (!pic_first_time)
                {
                  pic_ptr->next=new pic();
                  pic_ptr=pic_ptr->next;
                };
                pic_ptr->image=extract_filename(gal_ptr->param,"href="); //extract filename from param
                pic_first_time = false;
                gal_state = 1;
              };
              break;

      case 1: //looking for <img>
              new_tag = string_to_lower(gal_ptr->tag);
              res_tag = (new_tag=="img");
              if (res_tag)
              {
                //pic_ptr->next=new pic();
                pic_ptr->thumbnail=extract_filename(gal_ptr->param,"src="); //extract filename from param
                pic_ptr->alt_text=extract_filename(gal_ptr->param, "alt=");
                gal_state = 0;
              };
              break;

      }
      
      gal_delete = gal_ptr;
      gal_ptr=gal_ptr->next;
      if (!gal_first_time)
      {
        delete gal_delete;  
      };
      gal_first_time = false;
    }
    gallery_result->next = gallery_limit; //everything inbetween has been deleted
  
    //reformatting the gallery
  
    pic_ptr = pic_gallery;
    string lightbox_id = pic_gallery->alt_text;
    chunk* chunk_gallery = new chunk();
    gal_ptr = chunk_gallery;
    gal_first_time = true;

    while (pic_ptr!=NULL)
    {
      cout << pic_ptr->thumbnail << " " << pic_ptr->image << " " << pic_ptr->alt_text << endl;
      if (gal_first_time)
      {
        gal_ptr->tag = "a";
        gal_ptr->param = "href=\"" + pic_ptr->image + "\" data-lightbox=\"" + lightbox_id + "\"";
      }
      else
      {
        gal_ptr->next = new chunk("a",("href=\"" + pic_ptr->image + "\" data-lightbox=\"" + lightbox_id + "\""),"");
        gal_ptr=gal_ptr->next;
      }
      gal_ptr->next = new chunk("img",("alt=\"" + pic_ptr->alt_text + "\" src=\"" + pic_ptr->thumbnail + "\" class=\"cell\""),"");
      gal_ptr=gal_ptr->next;
      gal_ptr->next = new chunk("/a","","");
      gal_ptr=gal_ptr->next;
      pic_ptr = pic_ptr->next;
      gal_first_time=false;
    }
    gal_ptr->next = new chunk("/div","",""); // replace the </div> that got deleted
    gallery_result->insert(chunk_gallery);
  }
  else pic_gallery=NULL; //gallery not found


  // check for/add <script> lightbox tag;
  chunk* script_tag_result = map->search("script","lightbox","",false); // look
  if (script_tag_result==NULL)
  {
    cout << "<script> not found, inserting" << endl;
    script_tag_result = map->search("/body","","",true); // look for </body> instead
    if (script_tag_result!=NULL)
    {
      script_tag_result->tag = "script";
      script_tag_result->param = "type=\"text/javascript\" src=\"lightbox2/dist/js/lightbox-plus-jquery.js\"";
      script_tag_result=script_tag_result->insert(new chunk("/script","",""));
      script_tag_result->insert(new chunk("/body","",""));
    }
    else
    {
    cout << "Error finding </body>";
    }
  }
  else
  cout << script_tag_result << endl; //found, do nothing


  // tune the separators


  // inline pictures


  if (outfile.is_open())
  {
    //map->content = "***Test***";
    //outfile << "***Test***" << endl;
    //for (int i=0;i<MAP_ROWS;i++)
    //{    outfile << '<' << map[i][STATE_TAG] << (map[i][STATE_PARAM]==""?"":" ") << map [i][1] << '>' << map[i][2] << endl;
    //}
    chunk* j = map;
    if (map != NULL)
    {
      outfile << map << endl;
      while (j->next != NULL)
      {
        outfile << j->next << endl;
        j=j->next;
      }
    }
    
    outfile.close();
  }

  else status = status | STATUS_OUTPUT_ERROR; 

  if (status)
  {
    if (status & STATUS_INPUT_ERROR)
    {
        cout << "Unable to open input file";
    }
    if (status & STATUS_OUTPUT_ERROR)
    {
        cout << "Unable to open output file";
    }
  }
  cout << line << endl;
  return status;
}