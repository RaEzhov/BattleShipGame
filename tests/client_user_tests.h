#ifndef TESTS_CLIENT_USER_TESTS_H_
#define TESTS_CLIENT_USER_TESTS_H_

#include "client/user.h"

TEST(User, InitDefault){
  User user;
  EXPECT_STREQ(user.getNameStr().c_str(), "-");
  EXPECT_STREQ(user.getRatingStr().c_str(), "level: 0");
}

TEST(User, InitWithParams){
  User user;
  user.init("Hello", 123, 9999);
  EXPECT_STREQ(user.getNameStr().c_str(), "Hello");
  EXPECT_STREQ(user.getRatingStr().c_str(), "level: 99");
}

TEST(User, Reset){
  User user;
  user.init("hello", 2124, 512);
  user.reset();
  EXPECT_STREQ(user.getNameStr().c_str(), "-");
  EXPECT_STREQ(user.getRatingStr().c_str(), "level: 0");
}

#endif // TESTS_CLIENT_USER_TESTS_H_
