#include <curl/curl.h>
#include <string>
#include <iostream>
#include "rapidjson/include/rapidjson/document.h"
#include <unordered_set>
#include <vector>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <algorithm>

std::string kBaseUrl = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/";

// Constants for threading
const int MAX_THREADS = 8; 

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
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L); 

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res == CURLE_OK;
}

void crawl_parallel(const std::string& start_node, int max_depth) {
    std::unordered_set<std::string> visited;
    std::vector<std::string> current_level;

    current_level.push_back(start_node);
    visited.insert(start_node);

    for (int d = 0; d < max_depth; ++d) {
        std::vector<std::string> next_level;
        std::mutex next_lvl_mutex;
        std::mutex visited_mutex;

        int num_threads = std::min((int)current_level.size(), MAX_THREADS);
        std::vector<std::thread> workers;

        auto worker_func = [&](int thread_id) {
            for (size_t i = thread_id; i < current_level.size(); i += num_threads) {
                std::string buf;
                if (!fetch_neighbors(current_level[i], buf)) continue;

                rapidjson::Document doc;
                doc.Parse(buf.c_str());
                if (doc.HasParseError() || !doc.HasMember("neighbors")) continue;

                for (auto& n : doc["neighbors"].GetArray()) {
                    std::string next_url = kBaseUrl + clean_string(n.GetString());

                    std::lock_guard<std::mutex> lock(visited_mutex);
                    if (!visited.count(next_url)) {
                        visited.insert(next_url);
                        
                        std::lock_guard<std::mutex> next_lock(next_lvl_mutex);
                        next_level.push_back(next_url);
                    }
                }
            }
        };

        for (int i = 0; i < num_threads; ++i) {
            workers.emplace_back(worker_func, i);
        }

        for (auto& t : workers) {
            t.join();
        }

        current_level = std::move(next_level);
        if (current_level.empty()) break;
    }

    std::cout << "Total Unique Nodes Visited: " << visited.size() << std::endl;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " \"Actor Name\" depth\n";
        return 1;
    }

    std::string start_url = kBaseUrl + clean_string(argv[1]);
    int depth = std::stoi(argv[2]);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    auto start_time = std::chrono::steady_clock::now();

    crawl_parallel(start_url, depth);

    auto end_time = std::chrono::steady_clock::now();
    curl_global_cleanup();

    std::cout << "Runtime: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() 
              << "ms" << std::endl;

    return 0;
}