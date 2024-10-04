#include "gtest/gtest.h"

#include "testshelper.h"

TEST(Semver, getSemverPropertiesValid)
{
    Semver semver(2, 3, 4);

    EXPECT_TRUE(semver.isValid());

    EXPECT_EQ(2, semver.getField(Semver::SEMVER_MAJOR));
    EXPECT_EQ(3, semver.getField(Semver::SEMVER_MINOR));
    EXPECT_EQ(4, semver.getField(Semver::SEMVER_PATCH));
}

TEST(Semver, getSemverPropertiesInvalid)
{
    Semver semver(2, 3, 4);
    semver.clear();

    EXPECT_FALSE(semver.isValid());

    EXPECT_EQ(0, semver.getField(Semver::SEMVER_MAJOR));
    EXPECT_EQ(0, semver.getField(Semver::SEMVER_MINOR));
    EXPECT_EQ(0, semver.getField(Semver::SEMVER_PATCH));
}

TEST(Semver, formatToString)
{
    Semver semver(2, 10, 4);

    EXPECT_EQ("02.10.04", semver.toString('.', 2));
    EXPECT_EQ("2.10.4", semver.toString('.', 1));

    EXPECT_EQ("02-10-04", semver.toString('-', 2));
    EXPECT_EQ("2-10-4", semver.toString('-', 1));
}

TEST(Semver, parseFromStringValid)
{
    EXPECT_TRUE(Semver("02.10.04", '.').isValid());
    EXPECT_TRUE(Semver("2.10.4", '.').isValid());
    EXPECT_TRUE(Semver("2-10-4", '-').isValid());

    EXPECT_TRUE(Semver("2.10.4.366", '.').isValid());
}

TEST(Semver, parseFromStringInvalid)
{
    EXPECT_FALSE(Semver("", '.').isValid());
    EXPECT_FALSE(Semver("anytext", '.').isValid());
    EXPECT_FALSE(Semver("2.text.36", '.').isValid());

    EXPECT_FALSE(Semver("2.10.4", '-').isValid());
    EXPECT_FALSE(Semver("2-10-4", '.').isValid());

    EXPECT_FALSE(Semver("2.10", '.').isValid());
}

TEST(Semver, areEquals)
{
    EXPECT_EQ(Semver(2, 10, 4), Semver("2.10.4", '.'));
    EXPECT_NE(Semver(2, 10, 5), Semver("2.10.4", '.'));
}

TEST(Semver, isHigher)
{
    Semver semver0(2, 10, 4);
    Semver semver1(2, 10, 5), semver2(2, 11, 4), semver3(3, 10, 4), semver4(2, 10, 4);

    EXPECT_TRUE(semver1 > semver0);
    EXPECT_TRUE(semver2 > semver0);
    EXPECT_TRUE(semver3 > semver0);
    EXPECT_FALSE(semver4 > semver0);

    EXPECT_TRUE(semver1 >= semver0);
    EXPECT_TRUE(semver2 >= semver0);
    EXPECT_TRUE(semver3 >= semver0);
    EXPECT_TRUE(semver4 >= semver0);

    EXPECT_FALSE(semver1 < semver0);
    EXPECT_FALSE(semver2 < semver0);
    EXPECT_FALSE(semver3 < semver0);
    EXPECT_FALSE(semver4 < semver0);

    EXPECT_FALSE(semver1 <= semver0);
    EXPECT_FALSE(semver2 <= semver0);
    EXPECT_FALSE(semver3 <= semver0);
    EXPECT_TRUE(semver4 <= semver0);
}

TEST(Semver, isLower)
{
    Semver semver0(2, 10, 4);
    Semver semver1(2, 10, 3), semver2(2, 9, 4), semver3(1, 10, 4), semver4(2, 10, 4);

    EXPECT_TRUE(semver1 < semver0);
    EXPECT_TRUE(semver2 < semver0);
    EXPECT_TRUE(semver3 < semver0);
    EXPECT_FALSE(semver4 < semver0);

    EXPECT_TRUE(semver1 <= semver0);
    EXPECT_TRUE(semver2 <= semver0);
    EXPECT_TRUE(semver3 <= semver0);
    EXPECT_TRUE(semver4 <= semver0);

    EXPECT_FALSE(semver1 > semver0);
    EXPECT_FALSE(semver2 > semver0);
    EXPECT_FALSE(semver3 > semver0);
    EXPECT_FALSE(semver4 > semver0);

    EXPECT_FALSE(semver1 >= semver0);
    EXPECT_FALSE(semver2 >= semver0);
    EXPECT_FALSE(semver3 >= semver0);
    EXPECT_TRUE(semver4 >= semver0);
}

TEST(Semver, verifyCompatibility)
{
    Semver semverNext(3, 0, 0);
    Semver semver1(2, 99, 99), semver2(2, 0, 0), semver3(3, 0, 0);

    EXPECT_TRUE(semver1 < semverNext);
    EXPECT_TRUE(semver2 < semverNext);
    EXPECT_FALSE(semver3 < semverNext);

    EXPECT_TRUE(semver1 <= semverNext);
    EXPECT_TRUE(semver2 <= semverNext);
    EXPECT_TRUE(semver3 <= semverNext);

    EXPECT_FALSE(semver1 > semverNext);
    EXPECT_FALSE(semver2 > semverNext);
    EXPECT_FALSE(semver3 > semverNext);

    EXPECT_FALSE(semver1 >= semverNext);
    EXPECT_FALSE(semver2 >= semverNext);
    EXPECT_TRUE(semver3 >= semverNext);
}

TEST(Semver, verifyEdgeCases)
{
    Semver semver0(2, 2, 0), semver1(0, 2, 2), semver2(2, 0, 2), semver3(2, 2, 2);

    EXPECT_FALSE(semver0 < semver1);
    EXPECT_FALSE(semver0 < semver2);
    EXPECT_TRUE(semver0 < semver3);
    EXPECT_TRUE(semver0 > semver1);
    EXPECT_TRUE(semver0 > semver2);
    EXPECT_FALSE(semver0 > semver3);

    EXPECT_TRUE(semver1 < semver2);
    EXPECT_TRUE(semver1 < semver3);
    EXPECT_FALSE(semver1 > semver2);
    EXPECT_FALSE(semver1 > semver3);

    EXPECT_TRUE(semver2 < semver3);
    EXPECT_FALSE(semver2 > semver3);
}
