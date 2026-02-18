#include <curl/curl.h>
#include <string>
#include <iostream>
#include "rapidjson/include/rapidjson/document.h"
#include <unordered_set>

std::string kBaseUrl = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/";

size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t total_size = size * nmemb;
    std::string* buf = static_cast<std::string*>(userp);

    buf->append(static_cast<char*>(contents), total_size);
    return total_size;
}

void clean_string(std::string& s) {
    size_t find = s.find(' ');
    while (find != std::string::npos) {
        s = s.substr(0, find) + "%20" + s.substr(find + 1);
        find = s.find(' ');
    }
}

int read_url(CURL* curl, std::string& url, size_t depth, std::unordered_set<std::string>& visited, std::unordered_set<std::string>& results, size_t tabs = 0)
{
    if (depth <= 0) {
        results.insert(url);
        return 0;
    }

    if (visited.count(url)) return 0;

    visited.insert(url);
    CURLcode res;
    std::string buf;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);
    
    res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        
        rapidjson::Document doc;
        doc.Parse(buf.c_str());

        if (doc.HasParseError()) {
            std::cout << buf << "\n";
            std::cout << "JSON parse error\n";
            return 1;
        }

        auto neighbors = doc["neighbors"].GetArray();
        std::string next_url = "";
        for (auto& n : neighbors) {
            std::cout << "> " << n.GetString() << std::endl;
            next_url = kBaseUrl + n.GetString();
            clean_string(next_url);
            for (size_t i = 0; i < tabs; i++) { std::cout << "\t"; }
            read_url(curl, next_url, depth - 1, visited, results, tabs + 1);
        }
        
    } else {
        std::cout << "Curl call failed: " << curl_easy_strerror(res) << std::endl;
    }

    return 0;
}


int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " \"Actor Name\" depth\n";
        return 1;
    }

    std::string name = argv[1];
    int depth = std::stoi(argv[2]);

    clean_string(name);

    std::string url = kBaseUrl + name;
    CURL* curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        std::unordered_set<std::string> visited;
        std::unordered_set<std::string> results;

        read_url(curl, url, depth, visited, results);

        std::cout << "\nVisited nodes:\n";
        for (const auto& r : results) {
            std::cout << r << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    return 0;
}
