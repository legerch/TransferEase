#include "testshelper.h"

/*****************************/
/* Class documentations      */
/*****************************/

/*****************************/
/* Enum documentations       */
/*****************************/

/*****************************/
/* Structure documentations  */
/*****************************/

/*****************************/
/* Signals documentations    */
/*****************************/

/*****************************/
/* Macro definitions         */
/*****************************/
#if !defined(TEASE_TESTS_DIR_EXTERNAL_RSC)
#error "Macro 'TEASE_TESTS_DIR_EXTERNAL_RSC' is not defined, please set up !"
#endif

/*****************************/
/* Start namespace           */
/*****************************/

/*****************************/
/* Constants defintitions    */
/*****************************/

/*****************************/
/* Functions implementations */
/*****************************/

std::string TestsHelper::getPathExternalRsc(const std::string &filename)
{
    return TEASE_TESTS_DIR_EXTERNAL_RSC + filename;
}

/*****************************/
/* End namespace             */
/*****************************/

/*****************************/
/* End file                  */
/*****************************/