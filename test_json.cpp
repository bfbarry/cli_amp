#include <string>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

int main() {

    nlohmann::json j;

    // Parse JSON from a file
    std::ifstream input_file("config.json");
    if (input_file.is_open()) {
        input_file >> j;
    }
    std::cout << j["io"]["input_device"];

    j["k"] = "hey";
    std::ofstream output_file("config.json");
    if (output_file.is_open()) {
        output_file << j.dump(4);
    }
    return 0;


}