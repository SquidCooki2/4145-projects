#include <curl/curl.h>
#include <string>
#include <iostream>
#include "rapidjson/include/rapidjson/document.h"
#include <unordered_set>
#include <queue>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

template <typename T>
class BlockingQueue {
    public:

        void push(const T& t)  {
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                q.push(t);
            }
            cv.notify_one();
        }

        bool pop(T& t) {
            std::unique_lock<std::mutex> lock(queue_mutex);
            cv.wait(lock, [this](){ return is_done || !q.empty(); });

            if (is_done && q.empty()) return false;
            t = q.front();
            q.pop();

            return true;
        }

        void finish() {
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                is_done = true;
            }
            cv.notify_all();
        }

        bool empty() {
            std::lock_guard<std::mutex> lock(queue_mutex);
            return q.empty();
        }
    private:
        bool is_done = false;
        std::mutex queue_mutex;
        std::condition_variable cv;
        std::queue<T> q;
};

std::string kBaseUrl = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/";
int kMaxThreads = 8;

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

std::unordered_set<std::string> crawl(const std::string& start_url, int depth) {
    std::mutex visited_mutex;
    std::mutex results_mutex;
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> results;
    BlockingQueue<std::pair<std::string,int>> q;
    std::atomic<int> active_tasks(1);
    q.push({start_url, 0});
    visited.insert(start_url);

    auto decrement_active_tasks = [&]() {
        if (--active_tasks == 0) {
            q.finish();
        }
    };

    auto worker = [&]() {
        std::pair<std::string,int> current;

        while (q.pop(current)) {
            auto [url, d] = current;

            if (d == depth) {
                std::lock_guard<std::mutex> lock(results_mutex);
                results.insert(url);
                decrement_active_tasks();
                continue;
            }
            std::string buf;
            if (!fetch_neighbors(url, buf)) {
                decrement_active_tasks();
                continue;
            }

            rapidjson::Document doc;
            doc.Parse(buf.c_str());
            if (doc.HasParseError() || !doc.HasMember("neighbors")) {
                decrement_active_tasks();
                continue;
            }
            auto neighbors = doc["neighbors"].GetArray();
            for (auto& n : neighbors) {
                std::string next_url = kBaseUrl + std::string(n.GetString());
                next_url = clean_string(next_url);
                {
                    std::lock_guard<std::mutex> lock(visited_mutex);
                    if (!visited.count(next_url)) {
                        visited.insert(next_url);
                        active_tasks++;
                        q.push({next_url, d + 1});
                    }
                }
            }

            decrement_active_tasks();
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < kMaxThreads; i++) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Total unique nodes visited: " << visited.size() << std::endl;
    return results;
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

    crawl(url, depth);

    curl_global_cleanup();
    std::cout << "Runtime: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::steady_clock::now() - start).count()
              << "ms" << std::endl;

    return 0;
}
