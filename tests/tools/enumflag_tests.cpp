#include "gtest/gtest.h"

#include "transferease/tools/enumflag.h"

/*****************************/
/* Namespace instructions    */
/*****************************/

/*****************************/
/* Define test classes       */
/*****************************/

enum class FlagEnum : std::uint8_t
{
    FE_FLAG_NONE    = 0,

    FE_FLAG_ONE     = 1 << 0,
    FE_FLAG_TWO     = 1 << 1,
    FE_FLAG_THREE   = 1 << 2,
    FE_FLAG_FOUR    = 1 << 3
};

template<>
struct tease_enable_flags<FlagEnum>{
    static constexpr bool enable = true;
};

/*****************************/
/* Defines test routines
 * (using TEST())            */
/*****************************/
TEST(EnumFlagTest, flagAssignDefault)
{
    FlagEnum flag = FlagEnum::FE_FLAG_NONE;
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_NONE);

    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_ONE);
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_TWO);
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_THREE);
}

TEST(EnumFlagTest, flagAssign)
{
    FlagEnum flag = FlagEnum::FE_FLAG_ONE;
    EXPECT_TRUE(flag & FlagEnum::FE_FLAG_ONE);
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_TWO);
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_THREE);

    flag = FlagEnum::FE_FLAG_ONE | FlagEnum::FE_FLAG_THREE;
    EXPECT_TRUE(flag & FlagEnum::FE_FLAG_ONE);
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_TWO);
    EXPECT_TRUE(flag & FlagEnum::FE_FLAG_THREE);

    flag = FlagEnum::FE_FLAG_TWO;
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_ONE);
    EXPECT_TRUE(flag & FlagEnum::FE_FLAG_TWO);
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_THREE);

    flag |= FlagEnum::FE_FLAG_ONE;
    EXPECT_TRUE(flag & FlagEnum::FE_FLAG_ONE);
    EXPECT_TRUE(flag & FlagEnum::FE_FLAG_TWO);
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_THREE);
}

TEST(EnumFlagTest, flagClear)
{
    FlagEnum flag = FlagEnum::FE_FLAG_ONE | FlagEnum::FE_FLAG_THREE;

    flag &= ~FlagEnum::FE_FLAG_THREE;
    EXPECT_TRUE(flag & FlagEnum::FE_FLAG_ONE);
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_TWO);
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_THREE);

    flag &= ~FlagEnum::FE_FLAG_TWO;
    EXPECT_TRUE(flag & FlagEnum::FE_FLAG_ONE);
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_TWO);
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_THREE);

    flag = FlagEnum::FE_FLAG_ONE | FlagEnum::FE_FLAG_TWO | FlagEnum::FE_FLAG_THREE;
    flag &= ~(FlagEnum::FE_FLAG_ONE | FlagEnum::FE_FLAG_TWO);
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_ONE);
    EXPECT_FALSE(flag & FlagEnum::FE_FLAG_TWO);
    EXPECT_TRUE(flag & FlagEnum::FE_FLAG_THREE);
}

TEST(EnumFlagTest, flagToString)
{
    /* Define list of strings only once */
    static const std::unordered_map<FlagEnum, std::string> MAP_FLAG_ENUM_TO_STRING =
    {
        {FlagEnum::FE_FLAG_NONE, "none"},

        {FlagEnum::FE_FLAG_ONE, "one"},
        {FlagEnum::FE_FLAG_TWO, "two"},
        {FlagEnum::FE_FLAG_THREE, "three"},
        {FlagEnum::FE_FLAG_FOUR, "four"}
    };

    /* Perform tests */
    FlagEnum flag = FlagEnum::FE_FLAG_NONE;
    EXPECT_EQ("none", flagEnumToString(flag, MAP_FLAG_ENUM_TO_STRING, '|'));

    flag |= FlagEnum::FE_FLAG_TWO;
    EXPECT_EQ("two", flagEnumToString(flag, MAP_FLAG_ENUM_TO_STRING, '|'));

    flag |= FlagEnum::FE_FLAG_FOUR;
    EXPECT_EQ("two|four", flagEnumToString(flag, MAP_FLAG_ENUM_TO_STRING, '|'));

    flag |= FlagEnum::FE_FLAG_ONE;
    EXPECT_EQ("one|two|four", flagEnumToString(flag, MAP_FLAG_ENUM_TO_STRING, '|'));

    flag &= ~FlagEnum::FE_FLAG_TWO;
    EXPECT_EQ("one|four", flagEnumToString(flag, MAP_FLAG_ENUM_TO_STRING, '|'));
}

/*****************************/
/* End                       */
/*****************************/
