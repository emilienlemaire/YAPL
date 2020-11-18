#include "gtest/gtest.h"
int somme(int a, int b) {
    return a + b;
}

TEST(SommeTest, HandleSomme) {
   EXPECT_EQ(somme(0, 0), 0);
   EXPECT_EQ(somme(2, 3), 5);
}
