// RinggitTheSalesBell
// REST API Bell Control
// Rob Dobson 2018

void restAPI_Ring(RestAPIEndpointMsg& apiMsg, String& retStr)
{
    // Get command args
    String numRingsStr = RestAPIEndpoints::getNthArgStr(apiMsg._pArgStr, 0);
    int numRings = 1;
    if (numRingsStr.trim().length() > 0)
        numRings = numRingsStr.toInt();
    if (numRings > 10)
        numRings = 10;
    Log.info("ArgStr %s numRingsStr %s numRings %d", apiMsg._pArgStr, numRingsStr.c_str(), numRings);
    // Make the change
    bool rslt = pBellControl->ring(numRings);
    restAPI_setResultStr(retStr, rslt);
    // Record event on particle cloud
    if(pParticleCloud)
    {
        String evText = String::format("{\"rings\":%d}", numRings);
        pParticleCloud->recordEvent(evText);
    }
}

// Register REST API commands
void setupRestAPI_BellControl()
{
  restAPIEndpoints.addEndpoint("R", RestAPIEndpointDef::ENDPOINT_CALLBACK, restAPI_Ring, "", "");
}
