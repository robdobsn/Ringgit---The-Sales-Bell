// RinggitTheSalesBell
// Helper functions to implement REST API calls
// Rob Dobson 2018

unsigned long restHelper_ReportHealthHash()
{
    unsigned long hashVal = 0;
    int hashUsedBits = 0;
    hashUsedBits += restHelper_ReportHealth_System(hashUsedBits, &hashVal, NULL, NULL);
    hashUsedBits += restHelper_ReportHealth_Network(hashUsedBits, &hashVal, NULL);
    // Log.info("RepHealthHash %ld", hashVal);
    return hashVal;
}

void restHelper_ReportHealth(const char* pIdStr,
                String* pInitialContentJsonElementList, String& retStr)
{
    bool isRinging = false;
    pBellControl->getStatus(isRinging);
    String innerJsonStr = String::format("\"ringing\":%d", isRinging);
    int hashUsedBits = 0;
    String healthStrSystem;
    hashUsedBits += restHelper_ReportHealth_System(hashUsedBits, NULL, &healthStrSystem, NULL);
    if (innerJsonStr.length() > 0)
        innerJsonStr += ",";
    innerJsonStr += healthStrSystem;
    // Network information
    String healthStrNetwork;
    hashUsedBits += restHelper_ReportHealth_Network(hashUsedBits, NULL, &healthStrNetwork);
    if (innerJsonStr.length() > 0)
        innerJsonStr += ",";
    innerJsonStr += healthStrNetwork;
    // System information
    String idStrJSON = String::format("'$id':'%s%s%s'",
                pIdStr ? pIdStr : "", pIdStr ? "_" : "", System.deviceID().c_str());
    if (innerJsonStr.length() > 0)
        innerJsonStr += ",";
    String outStr = "{" + innerJsonStr + idStrJSON +
            (pInitialContentJsonElementList != NULL ? "," : "") +
            (pInitialContentJsonElementList != NULL ? *pInitialContentJsonElementList : "") + "}";
    retStr = outStr.replace('\'', '\"');
}

void restAPI_QueryStatus(RestAPIEndpointMsg& apiMsg, String& retStr)
{
    String initialContent = "'name': 'Ringgit'";
    restHelper_ReportHealth(NULL, &initialContent, retStr);
}

// Register REST API commands
void setupRestAPI_Helpers()
{
    // Query
    restAPIEndpoints.addEndpoint("Q", RestAPIEndpointDef::ENDPOINT_CALLBACK, restAPI_QueryStatus, "", "");

    // Reset device
    restAPIEndpoints.addEndpoint("RESET", RestAPIEndpointDef::ENDPOINT_CALLBACK, restAPI_Reset, "", "");

}
