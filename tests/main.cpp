#include <gtest/gtest.h>

#include "tests/client_user_tests.h"
#include "tests/client_screen_oblects_tests.h"

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}