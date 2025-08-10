// TheEngine/tests/unit/SanityCheck.cpp
#include "gtest/gtest.h"


TEST(SanityCheck, TrueIsTrue)
{

    ASSERT_TRUE(true) << "This fundamental truth should always hold.";
}

// Another basic arithmetic test.
TEST(SanityCheck, AdditionWorks)
{
    int sum = 5 + 3;
    // ASSERT_EQ asserts that two values are equal.
    ASSERT_EQ(sum, 8) << "Basic addition should work correctly.";
}

// Test a simple function 

int add(int a, int b)
{
    return a + b;
}

TEST(SanityCheck, SimpleFunctionTest)
{
    ASSERT_EQ(add(1, 2), 3) << "1 + 2 should equal 3.";
    ASSERT_EQ(add(-1, 1), 0) << "-1 + 1 should equal 0.";
}


TEST(SanityCheck, ExpectFalseToFailIfIncorrect)
{
  
    ASSERT_FALSE(false) << "This should always be false.";
}

// A test that shows how to provide a custom message
TEST(SanityCheck, CustomFailureMessage)
{
    int value = 12;
    int expected = 12;
    ASSERT_EQ(value, expected) << "The value " << value << " does match the expected " << expected << ".";
   
}