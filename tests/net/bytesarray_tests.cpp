#include "gtest/gtest.h"

#include "testshelper.h"

/*****************************/
/* Tests - Simple methods    */
/*****************************/

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

TEST(BytesArrayTest, insertBuffer)
{
    constexpr BytesArray::Byte bufferData[] = {0x01, 0x02, 0x03, 0x04};
    constexpr size_t bufferSize = sizeof(bufferData);
    constexpr int nbPushes = 2;

    BytesArray array;
    for(int i = 0; i < nbPushes; ++i){
        array.pushBack(bufferData, bufferSize);
    }

    EXPECT_EQ(bufferSize * nbPushes, array.getSize());
    int bufferIdx = 0;
    for(int i = 0; i < array.getSize(); ++i){
        EXPECT_EQ(bufferData[bufferIdx], array[i]);

        ++bufferIdx;
        if(bufferIdx >= bufferSize){
            bufferIdx = 0;
        }
    }
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

/*****************************/
/* Tests - Load from string  */
/*****************************/

struct DataBaStr
{
    std::string inData;
    std::vector<uint8_t> expData;
};

class TestBaFillFromStr : public ::testing::TestWithParam<DataBaStr>{};

TEST_P(TestBaFillFromStr, validateBaStr)
{
    const auto &params = GetParam();

    /* Fill bytes array */
    BytesArray ba;
    ba.setFromString(params.inData);

    /* Verify size of expected datas */
    ASSERT_EQ(ba.getSize(), params.expData.size());

    /* Verify each bytes */
    for(size_t i = 0; i < ba.getSize(); ++i){
        EXPECT_EQ(ba[i], params.expData[i]);
    }

    /* Verify conversion back */
    EXPECT_EQ(ba.toString(), params.inData);
}

INSTANTIATE_TEST_SUITE_P(
    validateBaStr, TestBaFillFromStr, ::testing::Values(
        DataBaStr{.inData = (const char *)u8"Hello world", .expData = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64}},
        DataBaStr{.inData = (const char *)u8"こんにちは", .expData = {0xE3, 0x81, 0x93, 0xE3, 0x82, 0x93, 0xE3, 0x81, 0xAB, 0xE3, 0x81, 0xA1, 0xE3, 0x81, 0xAF}}
    )
);

/*****************************/
/* Tests - Load from file    */
/*****************************/

TEST(BytesArrayTest, importExport)
{
    const std::string pathSampleIn = TestsHelper::getPathExternalRsc("samples/input/jaguar.bmp");
    const std::string pathSampleOut = TestsHelper::getPathExternalRsc("samples/output/jaguar.bmp");

    BytesArray baOriginal, baReloaded;

    /* Load and export input bytes array */
    ASSERT_TRUE(baOriginal.setFromFile(pathSampleIn));
    ASSERT_TRUE(baOriginal.toFile(pathSampleOut));

    /* Reload exported bytes array */
    ASSERT_TRUE(baReloaded.setFromFile(pathSampleOut));

    /* Verify that bytes array are equals */
    ASSERT_EQ(baOriginal, baReloaded);
}
