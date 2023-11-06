#include <string>
#include <simdjson.h>
#include <iostream>


int main() {

    simdjson::ondemand::parser parser;
    simdjson::padded_string json = simdjson::padded_string::load("config.json");
    simdjson::ondemand::document config = parser.iterate(json);
    std::string_view x = "";
    std::string_view input_device_name = config["io"]["input_device"];
    std::cout << input_device_name;
    std::cout << input_device_name.empty();
    std::cout << (int)x.empty();
    return 0;
}