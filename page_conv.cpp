// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <chrono>

using namespace std;
#include "chunk.cpp"

#define STATUS_NORMAL 0
#define STATUS_INPUT_ERROR 1
#define STATUS_OUTPUT_ERROR 2

void process_meta_tags(chunk* map);
void process_link_tags(chunk* map);
void process_separators(chunk* map);
void process_pictures(chunk* map);
void process_lightbox_tag(chunk* map);

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
  /*string working_dir = argtostring(args[0]);
  cout << working_dir <<endl;
  working_dir = working_dir.substr(0,(working_dir.find_last_of("/\\")+1));*/
  /* for (int j=0;j<argc;j++)
  {
    cout << argtostring(args[j]) << endl;
  }*/


  string filename_in;
  filename_in = (argc>1)?(argtostring(args[1])):"";
  cout << "Input filename: " << filename_in << endl;

  string filename_only = filename_in.substr(filename_in.find_last_of("/\\")+1);

  string directory_out;
  directory_out = (argc>2)?(argtostring(args[2])):"output\\";
  if (directory_out.at(directory_out.length()-1)!='\\') directory_out+='\\';
  //directory_out=working_dir + directory_out;
  if (CreateDirectory(directory_out.c_str(),NULL) || GetLastError() == ERROR_ALREADY_EXISTS) cout << directory_out << endl;

  string filename_out = directory_out + filename_only;
  cout << "Output written to: " << filename_out << endl;

  
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
  ifstream infile (filename_in); //("..//html//M48en.htm");
  ofstream outfile (filename_out);
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
        case 0x0D: //remove unnecessary line breaks
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

    // *** File conversion ***

    // meta tags

    process_meta_tags(map);

    // update css - link tags

    process_link_tags(map);

    // tune the separators
    process_separators(map);

    // rewrite the pictures for lightbox and convert the gallery
    process_pictures(map);

    // check for/add <script> lightbox tag;
    process_lightbox_tag(map);

  }
  else
  {
    status = status | STATUS_INPUT_ERROR;
    map=NULL;
    //goto finish;
  }

  //file write

  if (outfile.is_open() && map!=NULL)
  {
    chunk* j = map;
    if (map != NULL)
    {
      outfile << map << endl;
      while (j->next != NULL)
      {
        outfile << j->next; // no endl to avoid unnecessary whitespace
        j=j->next;
      }
    }
    
    outfile.close();
    cout << line << endl;
  }

  else status = status | STATUS_OUTPUT_ERROR; 

finish:
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
  return status;
}

void process_meta_tags(chunk* map)
{
  chunk* meta_tag_result = map->search("meta","","",true); // look once
  chunk* backup_result = NULL;

  if (meta_tag_result!=NULL)
  {
    //found!
    cout << meta_tag_result << endl;
    //making it the UTF-8 tag
    meta_tag_result->param = "http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"";
    backup_result=meta_tag_result;
    meta_tag_result = meta_tag_result->next->search("meta","","",true); // look second time
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
}

void process_link_tags(chunk* map)
{
  chunk* link_tag_result = map->search("link","","",true); //look once
  chunk* backup_result;
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
      cout << link_tag_result << endl;
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
}

void process_separators(chunk* map)
{
  chunk* sep_result = map->search("img","---","",false);
  while (sep_result!=NULL)
  {
    sep_result->param = "alt=\"---\" src=\"index_pliki/vn_sep.bmp\" class=\"sep\"";
    sep_result=sep_result->next;
    sep_result = sep_result->search("img","---","",false);
  }
}

void process_pictures(chunk* map)
{
  // div class="tabl" - get picture names and rewrite gallery
  chunk* gallery_result = map->search("div","tabl","",false);
  chunk* gallery_limit = NULL;
  bool gal_first_time = true;
  bool pic_first_time = true;
  pic* pic_gallery = new pic();
  //pic_gallery->image="*";
  //pic_gallery->thumbnail="*";
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
  else //gallery not found
  {
    pic_gallery=NULL;
    // inline pictures
    chunk* img_result = map->search("img","","",true);
    time_t nowtime = chrono::system_clock::to_time_t(chrono::system_clock::now());
    string hashname = ctime(&nowtime); //time used to create "unique" lightbox ID
    //cout << hashname << endl;
    string lightbox_id = hashname.substr(hashname.find_first_of(":")+1,2) + hashname.substr(hashname.find_last_of(":")+1,2);
    cout << "Lightbox ID used: " << lightbox_id << endl;
    while (img_result!=NULL)
    {
      if (img_result->param.find("class=\"sep\"")==string::npos)
      {
        cout << img_result << endl;
        string image=extract_filename(img_result->param,"src="); //extract filename from param
        string alttext = extract_filename(img_result->param, "alt=");
        img_result->tag = "a";
        img_result->param = "href=\"" + image +"\" data-lightbox=\"" + lightbox_id + "\"";
        img_result=img_result->insert(new chunk("img",("src=\"" + image + "\" alt=\"" + alttext + "\" class=\"articlepic\""),""));
        img_result=img_result->insert(new chunk("/a","",""));
        //img_result=img_result->next;
        //img_result=img_result->next;
        //img_result=img_result->next;
        //img_result=img_result->next;
      }
      else img_result=img_result->next;
      img_result = img_result->search("img","","",true);
    }
  }
}

void process_lightbox_tag(chunk* map)
{
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
}