// RinggitTheSalesBell
// Helper functions to implement general utility REST API calls
// Rob Dobson 2018

void restAPI_setResultStr(String& rsltStr, bool rslt)
{
    rsltStr = String::format("{ \"rslt\": \"%s\" }", rslt? "ok" : "fail");
}

void restAPI_Reset(RestAPIEndpointMsg& apiMsg, String& retStr)
{
    System.reset();
}

void restAPI_DFU(RestAPIEndpointMsg& apiMsg, String& retStr)
{
    System.dfu();
}

void handleReceivedApiStr(const char* requestStr, String& rsltStr)
{
    restAPIEndpoints.handleApiRequest(requestStr, rsltStr);
    if (rsltStr.length() == 0)
        restAPI_setResultStr(rsltStr, false);
}
