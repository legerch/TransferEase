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
struct enable_bitmask_operators<FlagEnum>{
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

/*****************************/
/* End                       */
/*****************************/
