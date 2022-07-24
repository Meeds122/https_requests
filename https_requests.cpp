// https_requests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

// Stuff I added to make winHTTP work. 
#include "Windows.h"
#include "Winhttp.h"
#pragma comment(lib, "winhttp.lib") // Tells VS to include the acutal files -shrug-
// Stuff I added to make the acutall programming work
#include <string>
using namespace std;



// Derived from the MS documentation
// https://docs.microsoft.com/en-us/windows/win32/winhttp/winhttp-sessions-overview#downloading-resources-from-the-web
// LPCWSTR == L"somestr"

/*
* void accessApi(LPCWSTR api, LPCWSTR resource, string *results)
*   Usage:
*       Reaches out to the site located at <api> in the form of subdomain.domain.tld
*       Access the resource in <resource> in the form of /dir/dir/file
*       e.g. https://someapi.com/data/access becomes
*           api = someapi.com
*           resource = /data/access
*       <*results> is a pointer to a C++ string object provided by the calling function.
*/
void accessApi(LPCWSTR api, LPCWSTR resource, string *results)
{
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    BOOL  bResults = FALSE;
    HINTERNET  hSession = NULL,
        hConnect = NULL,
        hRequest = NULL;

    // Use WinHttpOpen to obtain a session handle.
    hSession = WinHttpOpen(L"NISTifyAD/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    // Specify an HTTPS server.
    if (hSession)
        hConnect = WinHttpConnect(hSession, api,
            INTERNET_DEFAULT_HTTPS_PORT, 0);

    // Create an HTTP request handle.
    // ! Resource requested might be inserted here
    if (hConnect)
        hRequest = WinHttpOpenRequest(hConnect, L"GET", resource,
            NULL, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE);

    // Send a request.
    if (hRequest)
        bResults = WinHttpSendRequest(hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            WINHTTP_NO_REQUEST_DATA, 0,
            0, 0);


    // End the request.
    if (bResults)
        bResults = WinHttpReceiveResponse(hRequest, NULL);

    // Keep checking for data until there is nothing left.
    if (bResults)
    {
        do
        {
            // printf(".");
            // Check for available data.
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
                printf("Error %u in WinHttpQueryDataAvailable.\n",
                    GetLastError());

            // Allocate space for the buffer.
            pszOutBuffer = new char[dwSize + 1];
            if (!pszOutBuffer)
            {
                printf("Out of memory\n");
                dwSize = 0;
            }
            else
            {
                // Read the data.
                ZeroMemory(pszOutBuffer, dwSize + 1);

                if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
                    dwSize, &dwDownloaded))
                    printf("Error %u in WinHttpReadData.\n", GetLastError());
                else
                    //printf("%s", pszOutBuffer);
                    results->append(string(pszOutBuffer));

                // Free the memory allocated to the buffer.
                delete[] pszOutBuffer;
            }
        } while (dwSize > 0);
    }


    // Report any errors.
    if (!bResults)
        printf("Error %d has occurred.\n", GetLastError());

    // Close any open handles.
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
}

int main()
{
    string results;
    accessApi(L"api.pwnedpasswords.com", L"/range/734AD", &results);
    // For some reason, the string is empty after accessApi goes away...
    // I think it has something to do with the updated version of the results variable
    // being on the accessApi's stack rather than the main() stack...
    cout << results << endl;

    return 0;
}