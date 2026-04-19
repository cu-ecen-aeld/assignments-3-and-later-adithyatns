#include "CppUTest/TestHarness.h"

TEST_GROUP(DummyGroup)
{
};

TEST(DummyGroup, DummyTest)
{
    STRCMP_EQUAL("hello", "hello");
}
