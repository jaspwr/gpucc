#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "utils.h"

struct Benchmark {
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::string name;

    static std::vector<std::string> outputs; 
    
    static void print_all() {
        std::cout << "Benchmark results:" << std::endl;
        std::cout << "------------------" << std::endl;
        for (auto& output: outputs) {
            std::cout << output << std::endl;
        }
    }

    Benchmark(std::string name) {
        this->name = name;
        start = std::chrono::high_resolution_clock::now();
    }

    void finalise() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        
        // Big mess
        auto output = name + repeated_char(30 - name.length() + 1, ' ') + std::to_string(duration.count()) + "s";
        u32 bar_len = (u32)(duration.count() * 1e4);
        #define MAX_BAR_LEN 60
        bar_len = bar_len > MAX_BAR_LEN ? MAX_BAR_LEN : bar_len;
        u32 empty_len = MAX_BAR_LEN - (bar_len > MAX_BAR_LEN ? MAX_BAR_LEN : bar_len);
        u32 gap = 27 - (output.length() < 35 ? output.length() : 27);
        output += repeated_char(gap + 1, ' ') 
            + "[" + repeated_char(bar_len, '#') + repeated_char(empty_len, ' ') + "]";
        
        outputs.push_back(output);
    }
};

std::vector<std::string> Benchmark::outputs = std::vector<std::string>();
