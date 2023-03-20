#include <stdio.h>

#include "../src/utils.h"
#include "../src/compiler.h"
#include "../src/gl.h"
#include "../src/exception.h"
#include "../src/preprocessor.h"

#include <string>
#include <iostream>
#include <filesystem>
#include <functional>

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
            std::string output = compile(job, shaders);
            if (output == load_file(expected_output_path.c_str())) {
                return SUCCESS;
            } else {
                std::cout << "Expected: " << load_file(expected_output_path.c_str()) << std::endl;
                std::cout << "Got: " << output << std::endl;
                return FAILURE;
            }
        }
};

class UnitTest : public Test {
    public:
        std::function<TestResult()> test;
        UnitTest(std::string name, std::function<TestResult()> test) {
            this->name = name;
            this->test = test;
        }
        TestResult run(Shaders& shaders) {
            return test();
        }
};

void run_test(Test* test, Shaders& shaders, i32& succeded, i32& failed) {
    TestResult res;

    try {
        res = test->run(shaders);
    } catch(Exception e) {
        e.print();
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

UnitTest create_preprocessor_test(std::string name, std::string source_path, std::string expected_output_path) {
    return UnitTest(name, [=]() {
        auto var_reg = VariableRegistry();
        std::string output = preprocess(source_path, var_reg);
        if (output == load_file(expected_output_path.c_str())) {
            return SUCCESS;
        } else {
            std::cout << "Expected:\n" << load_file(expected_output_path.c_str()) << std::endl;
            std::cout << "Got:\n" << output << std::endl;
            return FAILURE;
        }
    });
}

std::vector<UnitTest> get_unit_tests() {
    std::vector<UnitTest> tests;
    tests.push_back(create_preprocessor_test(
        "preprocessor_misc",
        TEST_DIRECTORY "/unit/preprocessor_misc/in.c",
        TEST_DIRECTORY "/unit/preprocessor_misc/out.c"
    ));
    return tests;
}

#define UNIT_TESTS

int main(int arcg, char** argv) {
    i32 succeded = 0;
    i32 failed = 0;

    Gl::init(false);
    Shaders shaders = Gl::compile_shaders();

    #ifdef INTERGRATION_TESTS
    std::cout << "\n\n\n---------- Intergration Tests ----------\n";
    auto intergration_tests = get_intergration_tests(TEST_DIRECTORY "/integration");
    for (auto test : intergration_tests) {
        run_test(&test, shaders, succeded, failed);
    }
    #endif

    #ifdef UNIT_TESTS
    std::cout << "\n\n\n---------- Unit Tests ----------\n";
    auto unit_tests = get_unit_tests();
    for (auto test : unit_tests) {
        run_test(&test, shaders, succeded, failed);
    }
    #endif

    std::cout << "\n\n\n";
    auto overall = failed == 0 ? SUCCESS : FAILURE;
    printf("%s Succeded: %d, Failed: %d\n", result_string(overall), succeded, failed);
    return 0;
}