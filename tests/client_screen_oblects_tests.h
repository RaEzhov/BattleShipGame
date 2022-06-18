#ifndef TESTS_CLIENT_SCREEN_OBLECTS_TESTS_H_
#define TESTS_CLIENT_SCREEN_OBLECTS_TESTS_H_

#include <ostream>
#include "client/screen_objects.h"

struct ShipDirectionState{
  friend std::ostream &operator<<(std::ostream &os,
                                  const ShipDirectionState &state) {
    os << "initial: " << state.initial << " afterAction: " << state.afterAction;
    return os;
  }
  ShipDirection initial;
  ShipDirection afterAction;
};

struct ShipDirectionTest : testing::Test, testing::WithParamInterface<ShipDirectionState> {
  ShipDirection direction;

  ShipDirectionTest(){
    direction = GetParam().initial;
  }
};


TEST_P(ShipDirectionTest, PreIncreaseShipDirection){
  EXPECT_EQ(++direction, GetParam().afterAction);
}

TEST_P(ShipDirectionTest, PostIncreaseShipDirection){
  EXPECT_EQ(direction++, GetParam().initial);
  EXPECT_EQ(direction, GetParam().afterAction);
}

INSTANTIATE_TEST_CASE_P(Default, ShipDirectionTest,
                        testing::Values(
                            ShipDirectionState{UP, RIGHT},
                            ShipDirectionState{RIGHT, DOWN},
                            ShipDirectionState{DOWN, LEFT},
                            ShipDirectionState{LEFT, UP}

                        ));



#endif  // TESTS_CLIENT_SCREEN_OBLECTS_TESTS_H_
