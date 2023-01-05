#include <stdio.h>

#include "../src/utils.h"
#include "../src/compiler.h"
#include "../src/gl.h"

#include <string>
#include <iostream>
#include <filesystem>

#define TEST_DIRECTORY "../tests"

enum TestResult {
    FAILURE,
    SUCCESS,
};

const char* result_string(TestResult status) {
    return status == SUCCESS
        ? "\033[1;32mPASS\033[0m"
        : "\033[1;31mFAIL\033[0m"
        ;
}

void print_result(TestResult status, std::string test_name) {
    printf("%s %s\n", result_string(status), test_name.c_str());
}

class Test {
    public:
        std::string name;
        virtual TestResult run(Shaders& shaders) { return FAILURE; };
};

class IntergrationTest : public Test {
    public:
        std::string source_path;
        std::string expected_output_path;

        IntergrationTest(std::string name, std::string source_path, std::string expected_output_path) {
            this->name = name;
            this->source_path = source_path;
            this->expected_output_path = expected_output_path;
        }
        
        TestResult run(Shaders& shaders) {
            Job job = Job(source_path);
            if (compile(job, shaders) == load_file(expected_output_path.c_str())) {
                return SUCCESS;
            } else {
                return FAILURE;
            }
        }
};

void run_test(Test* test, Shaders& shaders, i32& succeded, i32& failed) {
    TestResult res;

    try {
        res = test->run(shaders);
    } catch(char* e) {
        res = FAILURE;
    } catch (std::string e) {
        res = FAILURE;
    }

    print_result(res, test->name);
    if (res == SUCCESS) {
        succeded++;
    } else {
        failed++;
    }
}

std::vector<IntergrationTest> get_intergration_tests(const char* path) {
    std::vector<IntergrationTest> tests;
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        if (!entry.is_directory()) continue;
        std::string name = entry.path().filename().string();
        auto test = IntergrationTest(
            name,
            entry.path().string() + "/in.c",
            entry.path().string() + "/out.uir"
        );
        tests.push_back(test);
    }
    return tests;
}

int main(int arcg, char** argv) {
    Gl::init(false);
    Shaders shaders = Gl::compile_shaders();
    i32 succeded = 0;
    i32 failed = 0;
    std::cout << "\n\n\n";
    auto intergration_tests = get_intergration_tests(TEST_DIRECTORY);
    for (auto test : intergration_tests) {
        run_test(&test, shaders, succeded, failed);
    }
    std::cout << "\n\n\n";
    auto overall = failed == 0 ? SUCCESS : FAILURE;
    printf("%s Succeded: %d, Failed: %d\n", result_string(overall), succeded, failed);
}