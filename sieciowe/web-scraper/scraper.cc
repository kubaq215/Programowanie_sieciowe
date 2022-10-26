#include <stdio.h>
#include <iostream>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

struct url_data 
{
    size_t size;
    char* data;
};

size_t write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data) 
{
    size_t index = data->size;
    size_t n = (size * nmemb);
    char* tmp;

    data->size += (size * nmemb);

    tmp = (char*) realloc(data->data, data->size + 1);

    if(tmp) 
    {
        data->data = tmp;
    } 
    else 
    {
        if(data->data) 
        {
            free(data->data);
        }
        fprintf(stderr, "Failed to allocate memory.\n");
        return 0;
    }

    memcpy((data->data + index), ptr, n);
    data->data[data->size] = '\0';

    return size * nmemb;
}

string handle_url(const char* url) 
{
    CURL *curl;

    struct url_data data;
    data.size = 0;
    data.data = (char*) malloc(4096);
    if(NULL == data.data) 
    {
        fprintf(stderr, "Failed to allocate memory.\n");
        return NULL;
    }

    data.data[0] = '\0';

    CURLcode res;

    curl = curl_easy_init();
    if (curl) 
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) 
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",  
                    curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    
    string a = data.data;
    return a;
}

int main(int argc, char* argv[])
{
    string url = "https://www.nbp.pl/Kursy/KursyA.html";
    string data;
    string tofind = "<td class=\"right\">1 EUR</td>";
    data = handle_url(url.c_str());
    long n = data.find(tofind, 0);
    for(int i=n+69; i<n+75; i++)
        cout << data[i];
}