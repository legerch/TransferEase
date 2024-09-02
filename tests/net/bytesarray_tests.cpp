#include "gtest/gtest.h"

#include "testshelper.h"

TEST(BytesArrayTest, construction)
{
    BytesArray empty;
    EXPECT_EQ(empty.getSize(), 0);
    EXPECT_TRUE(empty.isEmpty());

    BytesArray sized(10);
    EXPECT_EQ(sized.getSize(), 10);
    EXPECT_FALSE(sized.isEmpty());

    BytesArray initialized(5, 0xFF);
    EXPECT_EQ(initialized.getSize(), 5);
    for (const auto& byte : initialized) {
        EXPECT_EQ(byte, 0xFF);
    }
}

TEST(BytesArrayTest, insertPopValues)
{
    BytesArray array;
    array.pushBack(0x01);
    array.pushBack(0x02);
    array.pushBack(0x03);

    EXPECT_EQ(array.getSize(), 3);
    EXPECT_EQ(array[0], 0x01);
    EXPECT_EQ(array[1], 0x02);
    EXPECT_EQ(array[2], 0x03);

    array.popBack();
    EXPECT_EQ(array.getSize(), 2);
    EXPECT_EQ(array[0], 0x01);
    EXPECT_EQ(array[1], 0x02);
}

TEST(BytesArrayTest, accessAndModify)
{
    BytesArray array(5, 0x00);
    EXPECT_EQ(array.getSize(), 5);

    array[0] = 0x01;
    array[1] = 0x02;
    array[2] = 0x03;
    array[3] = 0x04;
    array[4] = 0x05;

    EXPECT_EQ(array[0], 0x01);
    EXPECT_EQ(array[1], 0x02);
    EXPECT_EQ(array[2], 0x03);
    EXPECT_EQ(array[3], 0x04);
    EXPECT_EQ(array[4], 0x05);
}

TEST(BytesArrayTest, clearAndEmpty)
{
    BytesArray array{0x01, 0x02, 0x03};
    EXPECT_FALSE(array.isEmpty());
    EXPECT_EQ(array.getSize(), 3);

    array.clear();
    EXPECT_TRUE(array.isEmpty());
    EXPECT_EQ(array.getSize(), 0);
}

TEST(BytesArrayTest, iteration)
{
    const BytesArray array{0x01, 0x02, 0x03};
    const std::vector<uint8_t> expected = {0x01, 0x02, 0x03};

    size_t index = 0;
    for(auto it = array.cbegin(); it != array.cend(); ++it){
        EXPECT_EQ(*it, expected[index]);
        ++index;
    }

    EXPECT_EQ(index, expected.size());
}
