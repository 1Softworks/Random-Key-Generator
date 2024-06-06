#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <curl/curl.h>

std::string generateKey(int length, int dashInterval) {
    std::string key;
    for (int i = 0; i < length; ++i) {
        if (i > 0 && i % dashInterval == 0) {
            key += '-';
        }
        key += '0' + rand() % 10;
    }
    return key;
}

size_t handleResponse(char* ptr, size_t size, size_t nmemb, std::string* responseData) {
    size_t dataSize = size * nmemb;
    responseData->append(ptr, dataSize);
    return dataSize;
}

std::string checkKey(const std::string& key, const std::string& endpoint) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return "Error: Unable to initialize libcurl";
    }

    std::string url = endpoint + "?key=" + key;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handleResponse);

    std::string responseData;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return "Error: Unable to connect to the endpoint";
    }

    return responseData;
}

int main() {
    srand(time(0));

    int keyLength, dashInterval, numKeys;
    std::string endpoint;

    std::cout << "Enter the length of each key: ";
    std::cin >> keyLength;

    std::cout << "Enter the number of characters between each dash (-): ";
    std::cin >> dashInterval;

    std::cout << "Enter the number of keys to generate: ";
    std::cin >> numKeys;

    std::cout << "Enter the endpoint to check the keys: ";
    std::cin >> endpoint;

    std::ofstream outputFile("valid.txt");
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open valid.txt for writing!" << std::endl;
        return 1;
    }

    std::cout << "Generated Keys:" << std::endl;
    for (int i = 0; i < numKeys; ++i) {
        std::string key = generateKey(keyLength, dashInterval);
        std::cout << key;

        std::string response = checkKey(key, endpoint);
        std::cout << " - Response: " << response << std::endl;

        if (response.find("valid") != std::string::npos) {
            outputFile << key << std::endl;
        }
    }

    outputFile.close();
    std::cout << "Valid keys written to valid.txt" << std::endl;

    return 0;
}
