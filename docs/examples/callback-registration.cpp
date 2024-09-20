/* Using lambda */
transferManager.setCbFailed([](tease::Request::TypeTransfer typeTransfer, tease::TransferManager::IdError idErr){
    std::cerr << "Failed to perform transfer [type: " << typeTransfer << ", id-err: " << idErr << "]" << std::endl;
});

/* Using class methods : static or member */
class CustomClass
{
public:
    CustomClass()
    {
        m_transferManager.setCbFailed(myMethodStaticForFailure); // Register static method
        m_transferManager.setCbFailed(std::bind(&CustomClass::myMethodMemberForFailure, this, std::placeholders::_1, std::placeholders::_2)); // Register member method
    }

private:
    void myMethodMemberForFailure(tease::Request::TypeTransfer typeTransfer, tease::TransferManager::IdError idErr){...}

private:
    static void myMethodStaticForFailure(tease::Request::TypeTransfer typeTransfer, tease::TransferManager::IdError idErr){...}

private:
    tease::TransferManager m_transferManager;
}