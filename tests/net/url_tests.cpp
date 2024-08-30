#include "gtest/gtest.h"

#include "testshelper.h"

/*****************************/
/* Tests - Parse URL         */
/*****************************/

struct DataUrlParse
{
    std::string inputUrl;

    bool expIsValid;
    Url::IdScheme expIdScheme;
    std::string expHost;
    uint16_t expPort;
    std::string expPath;
};

class TestUrlParsing : public ::testing::TestWithParam<DataUrlParse>{};

TEST_P(TestUrlParsing, validateUrls)
{
    const auto &params = GetParam();

    const Url url(params.inputUrl);
    EXPECT_EQ(url.isValid(), params.expIsValid);

    if(params.expIsValid){
        EXPECT_EQ(url.getIdScheme(), params.expIdScheme);
        EXPECT_EQ(url.getHost(), params.expHost);
        EXPECT_EQ(url.getPort(), params.expPort);
        EXPECT_EQ(url.getPath(), params.expPath);

        EXPECT_EQ(url.getUrl(), params.inputUrl);

    }else{
        EXPECT_EQ(url.getIdScheme(), Url::SCHEME_UNK);
        EXPECT_EQ(url.getHost(), "");
        EXPECT_EQ(url.getPort(), 0);
        EXPECT_EQ(url.getPath(), "");
    }
}

INSTANTIATE_TEST_SUITE_P(
    validateUrls, TestUrlParsing, ::testing::Values(
        DataUrlParse{.inputUrl = "https://example.com:8080/path/to/resource", .expIsValid = true, .expIdScheme = Url::SCHEME_HTTPS, .expHost = "example.com", .expPort = 8080, .expPath = "/path/to/resource"},
        DataUrlParse{.inputUrl = "http://example.com:8080/path/to/resource", .expIsValid = true, .expIdScheme = Url::SCHEME_HTTP, .expHost = "example.com", .expPort = 8080, .expPath = "/path/to/resource"},
        DataUrlParse{.inputUrl = "https://example.com/path/to/resource", .expIsValid = true, .expIdScheme = Url::SCHEME_HTTPS, .expHost = "example.com", .expPort = 0, .expPath = "/path/to/resource"},
        DataUrlParse{.inputUrl = "http://example.com/path/to/resource", .expIsValid = true, .expIdScheme = Url::SCHEME_HTTP, .expHost = "example.com", .expPort = 0, .expPath = "/path/to/resource"},
        DataUrlParse{.inputUrl = "https://example.com", .expIsValid = false, .expIdScheme = Url::SCHEME_HTTPS, .expHost = "example.com", .expPort = 0, .expPath = ""},
        DataUrlParse{.inputUrl = "ftp://example.com:8080/path/to/resource.zip", .expIsValid = true, .expIdScheme = Url::SCHEME_FTP, .expHost = "example.com", .expPort = 8080, .expPath = "/path/to/resource.zip"},
        DataUrlParse{.inputUrl = "ftps://example.com:8080/path/to/resource.zip", .expIsValid = true, .expIdScheme = Url::SCHEME_FTPS, .expHost = "example.com", .expPort = 8080, .expPath = "/path/to/resource.zip"},
        DataUrlParse{.inputUrl = "not_an_url", .expIsValid = false, .expIdScheme = Url::SCHEME_UNK, .expHost = "", .expPort = 0, .expPath = ""}
    )
);

/*****************************/
/* Tests - Create URL        */
/*****************************/

struct DataUrlCreate
{
    Url::IdScheme inputIdScheme;
    std::string inputHost;
    uint16_t inputPort;
    std::string inputPath;

    bool expIsValid;
    std::string expUrl;
};

class TestUrlCreate : public ::testing::TestWithParam<DataUrlCreate>{};

TEST_P(TestUrlCreate, createUrls)
{
    const auto &params = GetParam();

    Url url;
    url.setIdScheme(params.inputIdScheme);
    url.setHost(params.inputHost);
    url.setPort(params.inputPort);
    url.setPath(params.inputPath);

    EXPECT_EQ(url.isValid(), params.expIsValid);
    if(params.expIsValid){
        EXPECT_EQ(url.getUrl(), params.expUrl);
    }else{
        EXPECT_EQ(url.getUrl(), "");
    }
}

INSTANTIATE_TEST_SUITE_P(
    createUrls, TestUrlCreate, ::testing::Values(
        DataUrlCreate{.inputIdScheme = Url::SCHEME_HTTPS, .inputHost = "example.com", .inputPort = 8081, .inputPath = "/path/to/resource.zip", .expIsValid = true, .expUrl = "https://example.com:8081/path/to/resource.zip"},
        DataUrlCreate{.inputIdScheme = Url::SCHEME_FTP, .inputHost = "example.com", .inputPort = 0, .inputPath = "/path/to/resource.zip", .expIsValid = true, .expUrl = "ftp://example.com/path/to/resource.zip"},

        DataUrlCreate{.inputIdScheme = Url::SCHEME_HTTPS, .inputHost = "", .inputPort = 0, .inputPath = "", .expIsValid = false, .expUrl = ""},
        DataUrlCreate{.inputIdScheme = Url::SCHEME_UNK, .inputHost = "example.com", .inputPort = 8081, .inputPath = "/path/to/resource.zip", .expIsValid = false, .expUrl = ""}
    )
);
