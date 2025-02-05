﻿#include"model.h"
using namespace  std;
Model::Model()
{
    //给数据成员分配空间
    _url=std::make_shared<std::string>();
    _dir=std::make_shared<std::string>();
    _htmltxt=std::make_shared<std::string>();

};

std::shared_ptr<std::string> Model::get_Url() {
    return this->_url;
}

std::shared_ptr<std::string> Model::get_Dir() {
    return this->_dir;
}

std::shared_ptr<std::string> Model::get_Htmltxt() {
    return this->_htmltxt;
}

bool Model::setUrl(const std::string& str) {
    *_url=str;
    return true;
}

bool Model::setDir(const std::string& str) {
    *_dir=str;
    return true;
}

bool Model::setType(const int &type) {
    this->type=type;
    return true;
}
//【by：田文杰】如果后续需要修改返回值类型，修改后的类型必须可以转换成bool

//【by:李智】下载的线程要求：1，外界能够查询是否正在下载 IsDownloading()
//                          2，download结束后，触发消息 Fire(TASK_SINGLE_FINISHED)
void Model::downLoad()
{

    //无法保证url和dir合法，先进行简单检查
    qDebug()<<"downLoadBegin";
    CURL *curl_handle;
    CURLcode res;
    MemoryStruct mem;
    string message, useragent;
    ofstream out;
    curl_global_init(CURL_GLOBAL_ALL);

    curl_handle = curl_easy_init();
    if(curl_handle)
    {
        useragent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.116 Safari/537.36";
        curl_easy_setopt(curl_handle, CURLOPT_URL, _url->c_str());
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&mem);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, useragent.c_str());

        res = curl_easy_perform(curl_handle);

        if(res == CURLE_OK)
        {
            qDebug()<<"CURLE_OK";

            switch(type) {
            case TYPE_HTML:
                out.open(_dir->c_str());
                out << mem.memory;
                out.close();
                message = to_string(mem.size);
                message += " bytes retrieved.\n";
                break;
            case TYPE_TEXT:
                out.open(_dir->c_str());
                out << txt_proc(mem);
                out.close();
                message = to_string(mem.size);
                message += " bytes retrieved.\n";
                break;
            case TYPE_PHOTO:
                if(image_proc(mem))
                    message = "Images are downloaded.\n";
                else
                    message = "Save images failed, check your path is whether valid.\n";
                break;
            }
        }
        else
        {
            message = curl_easy_strerror(res);
            message = "curl_easy_perform() failed: " + message + ".\n";
        }
    }
    else
        message = "curl handle cannot be initialized!\n";
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    //原本应当传递消息。但是Fire会导致报错:Widgets must be created in the GUI thread.
    qDebug()<<"downLoad end";
    Fire(TASK_SINGLE_SUCEEDED);
    return;
};


size_t Model::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  MemoryStruct* memp = (MemoryStruct*)userp;

  memp->memory.resize(memp->size + realsize + 1);
  memcpy(&memp->memory[memp->size], contents, realsize);
  memp->size += realsize;
  memp->memory[memp->size] = 0;

  return realsize;
}

string Model::txt_proc(MemoryStruct& mem)
{
    vector<tag_node>tags;
    tag_node curnode;
    string res;

    curnode.txt_start = mem.memory.find("title>");
    curnode.txt_start += 6;
    mem.memory = mem.memory.substr(curnode.txt_start);
    curnode.txt_end = mem.memory.find("</");
    res += mem.memory.substr(0, curnode.txt_end) + "\n\n";
    //Min Heap + Switch + Enum
    //Insert first
    curnode.txt_end = mem.memory.find("</a>"); // aherf
    if(curnode.txt_end != -1)
    {
        curnode.tag_type = aherf;
        tags.push_back(curnode);
    }
    curnode.txt_end = mem.memory.find("title=\""); // title_eq !!This is the only forward find
    if(curnode.txt_end != -1)
    {
        curnode.tag_type = title_eq;
        tags.push_back(curnode);
    }
    curnode.txt_end = mem.memory.find("</span>"); // span
    if(curnode.txt_end != -1)
    {
        curnode.tag_type = span;
        tags.push_back(curnode);
    }
    curnode.txt_end = mem.memory.find("</p>"); // p_syntax
    if(curnode.txt_end != -1)
    {
        curnode.tag_type = p_syntax;
        tags.push_back(curnode);
    }
    curnode.txt_end = mem.memory.find("<br />"); // br_syntax
    if(curnode.txt_end != -1)
    {
        curnode.tag_type = br_syntax;
        tags.push_back(curnode);
    }
    curnode.txt_end = mem.memory.find("</h1>"); // h1
    if(curnode.txt_end != -1)
    {
        curnode.tag_type = h1;
        tags.push_back(curnode);
    }
    curnode.txt_end = mem.memory.find("</h2>"); // h2
    if(curnode.txt_end != -1)
    {
        curnode.tag_type = h2;
        tags.push_back(curnode);
    }
    curnode.txt_end = mem.memory.find("</h3>"); // h3
    if(curnode.txt_end != -1)
    {
        curnode.tag_type = h3;
        tags.push_back(curnode);
    }
    curnode.txt_end = mem.memory.find("</h4>"); // h4
    if(curnode.txt_end != -1)
    {
        curnode.tag_type = h4;
        tags.push_back(curnode);
    }
    curnode.txt_end = mem.memory.find("</h5>"); // h5
    if(curnode.txt_end != -1)
    {
        curnode.tag_type = h5;
        tags.push_back(curnode);
    }
    curnode.txt_end = mem.memory.find("</h6>"); // h6
    if(curnode.txt_end != -1)
    {
        curnode.tag_type = h6;
        tags.push_back(curnode);
    }
    make_heap(tags.begin(), tags.end(), ::greater{});
    while(!tags.empty())
    {
        pop_heap(tags.begin(), tags.end(), ::greater{});
        curnode = tags.back();
        tags.pop_back();
        
        switch (curnode.tag_type) {
        case aherf:
            curnode.txt_start = mem.memory.rfind('>', curnode.txt_end) + 1;
            if(curnode.txt_start == curnode.txt_end || mem.memory[curnode.txt_start] == '\n')
            {
                mem.memory = mem.memory.substr(curnode.txt_end + 4);
                for(auto& i : tags)
                    i.txt_end -= curnode.txt_end + 4;

                curnode.txt_end = mem.memory.find("</a>"); // aherf
                if(curnode.txt_end != -1)
                {
                    curnode.tag_type = aherf;
                    tags.push_back(curnode);
                }
                push_heap(tags.begin(), tags.end(), ::greater{});
                continue;
            }
            res += mem.memory.substr(curnode.txt_start, curnode.txt_end - curnode.txt_start) + "\n";
            mem.memory = mem.memory.substr(curnode.txt_end + 4);
            for(auto& i : tags)
                i.txt_end -= curnode.txt_end + 4;

            curnode.txt_end = mem.memory.find("</a>"); // aherf
            if(curnode.txt_end != -1)
            {
                curnode.tag_type = aherf;
                tags.push_back(curnode);
            }
            push_heap(tags.begin(), tags.end(), ::greater{});
            break;
        case title_eq:
            curnode.txt_start = curnode.txt_end + 7;
            curnode.txt_end = mem.memory.find('\"', curnode.txt_start);
            if(curnode.txt_start == curnode.txt_end || mem.memory[curnode.txt_start] == '\n')
            {
                mem.memory = mem.memory.substr(curnode.txt_end + 1);
                for(auto& i : tags)
                    i.txt_end -= curnode.txt_end + 1;

                curnode.txt_end = mem.memory.find("title=\""); // title_eq !!This is the only forward find
                if(curnode.txt_end != -1)
                {
                    curnode.tag_type = title_eq;
                    tags.push_back(curnode);
                }
                push_heap(tags.begin(), tags.end(), ::greater{});
                continue;
            }
            res += mem.memory.substr(curnode.txt_start, curnode.txt_end - curnode.txt_start) + "\n";
            mem.memory = mem.memory.substr(curnode.txt_end + 1);
            for(auto& i : tags)
                i.txt_end -= curnode.txt_end + 1;

            curnode.txt_end = mem.memory.find("title=\""); // title_eq !!This is the only forward find
            if(curnode.txt_end != -1)
            {
                curnode.tag_type = title_eq;
                tags.push_back(curnode);
            }
            push_heap(tags.begin(), tags.end(), ::greater{});
            break;
        case span:
            curnode.txt_start = mem.memory.rfind('>', curnode.txt_end) + 1;
            if(curnode.txt_start == curnode.txt_end || mem.memory[curnode.txt_start] == '\n')
            {
                mem.memory = mem.memory.substr(curnode.txt_end + 7);
                for(auto& i : tags)
                    i.txt_end -= curnode.txt_end + 7;

                curnode.txt_end = mem.memory.find("</span>"); // span
                if(curnode.txt_end != -1)
                {
                    curnode.tag_type = span;
                    tags.push_back(curnode);
                }
                push_heap(tags.begin(), tags.end(), ::greater{});
                continue;
            }
            res += mem.memory.substr(curnode.txt_start, curnode.txt_end - curnode.txt_start) + "\n";
            mem.memory = mem.memory.substr(curnode.txt_end + 7);
            for(auto& i : tags)
                i.txt_end -= curnode.txt_end + 7;
            
            curnode.txt_end = mem.memory.find("</span>"); // span
            if(curnode.txt_end != -1)
            {
                curnode.tag_type = span;
                tags.push_back(curnode);
            }
            push_heap(tags.begin(), tags.end(), ::greater{});
            break;
        case p_syntax:
            curnode.txt_start = mem.memory.rfind('>', curnode.txt_end) + 1;
            if(curnode.txt_start == curnode.txt_end || mem.memory[curnode.txt_start] == '\n')
            {
                mem.memory = mem.memory.substr(curnode.txt_end + 4);
                for(auto& i : tags)
                    i.txt_end -= curnode.txt_end + 4;

                curnode.txt_end = mem.memory.find("</p>"); // p_syntax
                if(curnode.txt_end != -1)
                {
                    curnode.tag_type = p_syntax;
                    tags.push_back(curnode);
                }
                push_heap(tags.begin(), tags.end(), ::greater{});
                continue;
            }
            res += mem.memory.substr(curnode.txt_start, curnode.txt_end - curnode.txt_start) + "\n";
            mem.memory = mem.memory.substr(curnode.txt_end + 4);
            for(auto& i : tags)
                i.txt_end -= curnode.txt_end + 4;

            curnode.txt_end = mem.memory.find("</p>"); // p_syntax
            if(curnode.txt_end != -1)
            {
                curnode.tag_type = p_syntax;
                tags.push_back(curnode);
            }
            push_heap(tags.begin(), tags.end(), ::greater{});
            break;
        case br_syntax:
            curnode.txt_start = mem.memory.rfind('>', curnode.txt_end) + 1;
            if(curnode.txt_start == curnode.txt_end || mem.memory[curnode.txt_start] == '\n')
            {
                mem.memory = mem.memory.substr(curnode.txt_end + 6);
                for(auto& i : tags)
                    i.txt_end -= curnode.txt_end + 6;
                
                curnode.txt_end = mem.memory.find("<br />"); // br_syntax
                if(curnode.txt_end != -1)
                {
                    curnode.tag_type = br_syntax;
                    tags.push_back(curnode);
                }
                push_heap(tags.begin(), tags.end(), ::greater{});
                continue;
            }
            res += mem.memory.substr(curnode.txt_start, curnode.txt_end - curnode.txt_start) + "\n";
            mem.memory = mem.memory.substr(curnode.txt_end + 6);
            for(auto& i : tags)
                i.txt_end -= curnode.txt_end + 6;

            curnode.txt_end = mem.memory.find("<br />"); // br_syntax
            if(curnode.txt_end != -1)
            {
                curnode.tag_type = br_syntax;
                tags.push_back(curnode);
            }
            push_heap(tags.begin(), tags.end(), ::greater{});
            break;
        case h1:
            curnode.txt_start = mem.memory.rfind('>', curnode.txt_end) + 1;
            if(curnode.txt_start == curnode.txt_end || mem.memory[curnode.txt_start] == '\n')
            {
                mem.memory = mem.memory.substr(curnode.txt_end + 5);
                for(auto& i : tags)
                    i.txt_end -= curnode.txt_end + 5;
                curnode.txt_end = mem.memory.find("</h1>"); // h1
                if(curnode.txt_end != -1)
                {
                    curnode.tag_type = h1;
                    tags.push_back(curnode);
                }
                push_heap(tags.begin(), tags.end(), ::greater{});
                continue;
            }
            res += mem.memory.substr(curnode.txt_start, curnode.txt_end - curnode.txt_start) + "\n";
            mem.memory = mem.memory.substr(curnode.txt_end + 5);
            for(auto& i : tags)
                i.txt_end -= curnode.txt_end + 5;

            curnode.txt_end = mem.memory.find("</h1>"); // h1
            if(curnode.txt_end != -1)
            {
                curnode.tag_type = h1;
                tags.push_back(curnode);
            }
            push_heap(tags.begin(), tags.end(), ::greater{});
            break;
        case h2:
            curnode.txt_start = mem.memory.rfind('>', curnode.txt_end) + 1;
            if(curnode.txt_start == curnode.txt_end || mem.memory[curnode.txt_start] == '\n')
            {
                mem.memory = mem.memory.substr(curnode.txt_end + 5);
                for(auto& i : tags)
                    i.txt_end -= curnode.txt_end + 5;
                
                curnode.txt_end = mem.memory.find("</h2>"); // h2
                if(curnode.txt_end != -1)
                {
                    curnode.tag_type = h2;
                    tags.push_back(curnode);
                }
                push_heap(tags.begin(), tags.end(), ::greater{});
                continue;
            }
            res += mem.memory.substr(curnode.txt_start, curnode.txt_end - curnode.txt_start) + "\n";
            mem.memory = mem.memory.substr(curnode.txt_end + 5);
            for(auto& i : tags)
                i.txt_end -= curnode.txt_end + 5;

            curnode.txt_end = mem.memory.find("</h2>"); // h2
            if(curnode.txt_end != -1)
            {
                curnode.tag_type = h2;
                tags.push_back(curnode);
            }
            push_heap(tags.begin(), tags.end(), ::greater{});
            break;
        case h3:
            curnode.txt_start = mem.memory.rfind('>', curnode.txt_end) + 1;
            if(curnode.txt_start == curnode.txt_end || mem.memory[curnode.txt_start] == '\n')
            {
                mem.memory = mem.memory.substr(curnode.txt_end + 5);
                for(auto& i : tags)
                    i.txt_end -= curnode.txt_end + 5;
                
                curnode.txt_end = mem.memory.find("</h3>"); // h3
                if(curnode.txt_end != -1)
                {
                    curnode.tag_type = h3;
                    tags.push_back(curnode);
                }
                push_heap(tags.begin(), tags.end(), ::greater{});
                continue;
            }
            res += mem.memory.substr(curnode.txt_start, curnode.txt_end - curnode.txt_start) + "\n";
            mem.memory = mem.memory.substr(curnode.txt_end + 5);
            for(auto& i : tags)
                i.txt_end -= curnode.txt_end + 5;
            
            curnode.txt_end = mem.memory.find("</h3>"); // h3
            if(curnode.txt_end != -1)
            {
                curnode.tag_type = h3;
                tags.push_back(curnode);
            }
            push_heap(tags.begin(), tags.end(), ::greater{});
            break;
        case h4:
            curnode.txt_start = mem.memory.rfind('>', curnode.txt_end) + 1;
            if(curnode.txt_start == curnode.txt_end || mem.memory[curnode.txt_start] == '\n')
            {
                mem.memory = mem.memory.substr(curnode.txt_end + 5);
                for(auto& i : tags)
                    i.txt_end -= curnode.txt_end + 5;

                curnode.txt_end = mem.memory.find("</h4>"); // h4
                if(curnode.txt_end != -1)
                {
                    curnode.tag_type = h4;
                    tags.push_back(curnode);
                }
                push_heap(tags.begin(), tags.end(), ::greater{});
                continue;
            }
            res += mem.memory.substr(curnode.txt_start, curnode.txt_end - curnode.txt_start) + "\n";
            mem.memory = mem.memory.substr(curnode.txt_end + 5);
            for(auto& i : tags)
                i.txt_end -= curnode.txt_end + 5;

            curnode.txt_end = mem.memory.find("</h4>"); // h4
            if(curnode.txt_end != -1)
            {
                curnode.tag_type = h4;
                tags.push_back(curnode);
            }
            push_heap(tags.begin(), tags.end(), ::greater{});
            break;
        case h5:
            curnode.txt_start = mem.memory.rfind('>', curnode.txt_end) + 1;
            if(curnode.txt_start == curnode.txt_end || mem.memory[curnode.txt_start] == '\n')
            {
                mem.memory = mem.memory.substr(curnode.txt_end + 5);
                for(auto& i : tags)
                    i.txt_end -= curnode.txt_end + 5;

                curnode.txt_end = mem.memory.find("</h5>"); // h5
                if(curnode.txt_end != -1)
                {
                    curnode.tag_type = h5;
                    tags.push_back(curnode);
                }
                push_heap(tags.begin(), tags.end(), ::greater{});
                continue;
            }
            res += mem.memory.substr(curnode.txt_start, curnode.txt_end - curnode.txt_start) + "\n";
            mem.memory = mem.memory.substr(curnode.txt_end + 5);
            for(auto& i : tags)
                i.txt_end -= curnode.txt_end + 5;

            curnode.txt_end = mem.memory.find("</h5>"); // h5
            if(curnode.txt_end != -1)
            {
                curnode.tag_type = h5;
                tags.push_back(curnode);
            }
            push_heap(tags.begin(), tags.end(), ::greater{});
            break;
        case h6:
            curnode.txt_start = mem.memory.rfind('>', curnode.txt_end) + 1;
            if(curnode.txt_start == curnode.txt_end || mem.memory[curnode.txt_start] == '\n')
            {
                mem.memory = mem.memory.substr(curnode.txt_end + 5);
                for(auto& i : tags)
                    i.txt_end -= curnode.txt_end + 5;
                
                curnode.txt_end = mem.memory.find("</h6>"); // h6
                if(curnode.txt_end != -1)
                {
                    curnode.tag_type = h6;
                    tags.push_back(curnode);
                }
                push_heap(tags.begin(), tags.end(), ::greater{});
                continue;
            }
            res += mem.memory.substr(curnode.txt_start, curnode.txt_end - curnode.txt_start) + "\n";
            mem.memory = mem.memory.substr(curnode.txt_end + 5);
            for(auto& i : tags)
                i.txt_end -= curnode.txt_end + 5;
                
            curnode.txt_end = mem.memory.find("</h6>"); // h6
            if(curnode.txt_end != -1)
            {
                curnode.tag_type = h6;
                tags.push_back(curnode);
            }
            push_heap(tags.begin(), tags.end(), ::greater{});
            break;
        }
    }
    return res;
};
bool Model::IsDownloading()
{
    return false;
}

bool Model::image_proc(MemoryStruct& mem)
{
    //find jpg, png, bmp, gif, jpeg, svg
    int start, end, count;
    int invalid;
    string img_url, filetype[6];
    string path, filesearch;
    filetype[0]=".jpg";
    filetype[1]=".png";
    filetype[2]=".bmp";
    filetype[3]=".gif";
    filetype[4]=".jpeg";
    filetype[5]=".svg";

    for(auto& i : filetype)
    {
        count = 1;
        filesearch = mem.memory;
        while(1)
        {
            end = filesearch.find(i);
            if(end == -1)
                break;
            start = filesearch.rfind("//", end) + 2;
            invalid = filesearch.rfind('\"', end);
            if(start > invalid)
            {
                img_url = filesearch.substr(start, end - start) + i;
                if(!image_download(img_url, *_dir + "\\pic" + to_string(count) + i, count))
                    return false;
            }
            filesearch = filesearch.substr(end + i.size());
        }
    }
    return true;
}

bool Model::image_download(string& img_url, string path, int& count)
{
    //curl thing
    FILE* output_img;
    CURL* image; 
    CURLcode imgresult;
    string useragent;
    int sz = 0;

    image = curl_easy_init(); 
    if(image)
    {
        // Open file 
        output_img = fopen(path.c_str(), "wb");
        useragent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.116 Safari/537.36";
        if(output_img == NULL)
        {
//            QMessageBox::information(nullptr,"Error", "File opened failed.");
            return false;
        }
        curl_easy_setopt(image, CURLOPT_URL, img_url.c_str()); 
        curl_easy_setopt(image, CURLOPT_WRITEFUNCTION, NULL); 
        curl_easy_setopt(image, CURLOPT_WRITEDATA, output_img);
        curl_easy_setopt(image, CURLOPT_USERAGENT, useragent.c_str());

        imgresult = curl_easy_perform(image);
        fseek(output_img, 0L, SEEK_END);
        sz = ftell(output_img);
//            QMessageBox::information(nullptr,"Error", "Image download failed.");
    }
    curl_easy_cleanup(image);
    fclose(output_img);
    if(imgresult || sz == 0)
        remove(path.c_str());
    else
        count++;
    return true;
}
