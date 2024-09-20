#include <iostream>
#include <thread>

#include <transferease/transfermanager.h>

/*****************************/
/* Example details           */
/*****************************/

/**
 * Simple example to download a list of requests.
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

/*****************************/
/* Class aliases             */
/*****************************/

using Manager = tease::TransferManager;
using Request = tease::Request;
using Url = tease::Url;

/*****************************/
/* Functions helpers         */
/*****************************/

static void prepareRequestDl(const std::string &pathRes, Request::List &listReqs)
{
    Url url;
    url.setIdScheme(Url::SCHEME_FTP);
    url.setHost(CFG_HOST);
    url.setPath(pathRes);

    Request::PtrShared req = std::make_shared<Request>();
    req->configureDownload(url);

    listReqs.push_back(req);
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

    /* Create requests */
    const std::vector<std::string> listPaths =
    {
        "mypath/res/entity1.zip",
        "mypath/res/entity2.zip",
        "mypath/res/entity3.zip"
    };

    for(auto it = listPaths.cbegin(); it != listPaths.cend(); ++it){
        prepareRequestDl(*it, listReqs);
    }

    /* Start download */
    Manager::IdError idErr = manager.startDownload(listReqs);
    if(idErr != Manager::ERR_NO_ERROR){
        std::cerr << "Failed to start download [id-err: " << idErr << "]" << std::endl;
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