#include <curl/curl.h>
#include <string>
#include <iostream>
#include "rapidjson/include/rapidjson/document.h"
#include <unordered_set>
#include <queue>
#include <chrono>

std::string kBaseUrl = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/";

size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total_size = size * nmemb;
    std::string* buf = static_cast<std::string*>(userp);
    buf->append(static_cast<char*>(contents), total_size);
    return total_size;
}

std::string clean_string(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == ' ') out += "%20";
        else out += c;
    }
    return out;
}

bool fetch_neighbors(const std::string& url, std::string& buf) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
}

void crawl(const std::string& start_url, int depth, std::unordered_set<std::string>& visited, std::unordered_set<std::string>& results) {
    std::queue<std::pair<std::string,int>> q;
    q.push({start_url, 0});
    visited.insert(start_url);

    while (!q.empty()) {
        auto [url, d] = q.front();
        q.pop();

        if (d == depth) {
            results.insert(url);
            continue;
        }

        std::string buf;
        if (!fetch_neighbors(url, buf)) continue;

        rapidjson::Document doc;
        doc.Parse(buf.c_str());
        if (doc.HasParseError()) continue;

        auto neighbors = doc["neighbors"].GetArray();
        for (auto& n : neighbors) {
            std::string next_url = kBaseUrl + std::string(n.GetString());
            next_url = clean_string(next_url);

            if (!visited.count(next_url)) {
                visited.insert(next_url);
                q.push({next_url, d + 1});
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " \"Actor Name\" depth\n";
        return 1;
    }

    std::string name = clean_string(argv[1]);
    int depth = std::stoi(argv[2]);

    auto start = std::chrono::steady_clock::now();

    curl_global_init(CURL_GLOBAL_DEFAULT);

    std::string url = kBaseUrl + name;
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> results;

    crawl(url, depth, visited, results);

    curl_global_cleanup();

    std::cout << "Runtime: "
              << std::chrono::duration_cast<std::chrono::microseconds>(
                     std::chrono::steady_clock::now() - start).count()
              << "μs" << std::endl;

    return 0;
}
