#include <iostream>
#include <map>
#include <thread>

#include <transferease/transfermanager.h>

/*****************************/
/* Example details           */
/*****************************/

/*
 * Simple example to download/upload a list of requests.
 * Note that default callback behaviour are used 
 * here to reduce example size. 
 */

/*****************************/
/* Macro definitions         */
/*****************************/

#define CFG_HOST        "0.0.0.0"
#define CFG_USERNAME    "myusername"
#define CFG_PASSWD      "mypasswd"
#define CFG_MAX_TRIALS  1

#define RUN_DL_ENABLE   true    // Only here to simplify example : set to "false" for upload

/*****************************/
/* Class aliases             */
/*****************************/

using BytesArray = tease::BytesArray;
using Manager = tease::TransferManager;
using Request = tease::Request;
using Url = tease::Url;

/*****************************/
/* Functions helpers         */
/*****************************/

static void prepareRequestsDl(Request::List &listReqs)
{
    /* Create server ressources path */
    const std::vector<std::string> listPaths =
    {
        "mypath/res/entity1.zip",
        "mypath/res/entity2.zip",
        "mypath/res/entity3.zip"
    };

    /* Prepare requests */
    for(auto it = listPaths.cbegin(); it != listPaths.cend(); ++it){
        Url url;
        url.setIdScheme(Url::SCHEME_FTP);
        url.setHost(CFG_HOST);
        url.setPath(*it);

        Request::PtrShared req = std::make_shared<Request>();
        req->configureDownload(url);

        listReqs.push_back(req);
    }
}

static void prepareRequestsUp(Request::List &listReqs)
{
    /* Create ressources path */
    const std::map<std::string, std::string> mapRrcs =
    {
        {"entity1.zip", "path/server/entity1.zip"},
        {"entity2.zip", "path/server/entity2.zip"},
    };

    /* Prepare requests */
    for(auto it = mapRrcs.cbegin(); it != mapRrcs.cend(); ++it){
        // Load ressources to upload
        BytesArray data;
        data.setFromFile(it->first);

        // Set destination URL
        Url url;
        url.setIdScheme(Url::SCHEME_FTP);
        url.setHost(CFG_HOST);
        url.setPath(it->second);

        // Create associated request
        Request::PtrShared req = std::make_shared<Request>();
        req->configureUpload(url, std::move(data));

        listReqs.push_back(req);
    }
}

/*****************************/
/* Main method               */
/*****************************/

int main(int argc, char *argv[])
{
    Manager manager;
    Request::List listReqs;

    /* Configure transfer manager */
    manager.setUserInfos(CFG_USERNAME, CFG_PASSWD);
    manager.setNbMaxTrials(CFG_MAX_TRIALS);

    /* Create requests and start transfer */
#if RUN_DL_ENABLE
    prepareRequestsDl(listReqs);
    Manager::IdError idErr = manager.startDownload(listReqs);
#else
    prepareRequestsUp(listReqs);
    Manager::IdError idErr = manager.startUpload(listReqs);
#endif

    if(idErr != Manager::ERR_NO_ERROR){
        std::cerr << "Failed to start transfer [id-err: " << idErr << "]" << std::endl;
        return 1;
    }

    /*
     * Placeholder loop used to wait for the transfer to finish before exiting application.
     * In a real application, prefer to use callbacks methods !
     */
    while(manager.transferIsInProgress()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Done" << std::endl;

    return 0;
}