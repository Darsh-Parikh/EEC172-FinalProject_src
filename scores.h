#ifndef SCORES_H
#define SCORES_H

#include "buffers.h"

Buffer_t playerName;
int playerScore = 0;

Buffer_t scoreBoard1_Name;
int scoreBoard1_Score = 0;

Buffer_t scoreBoard2_Name;
int scoreBoard2_Score = 0;

Buffer_t scoreBoard3_Name;
int scoreBoard3_Score = 0;

//---------------------------------------

void IncreaseScore(int score) {
    playerScore += score;
}

void StoreScore() {
    // Find the lowest score on the scoreboard
    int *lowestScore = &scoreBoard1_Score;
    Buffer_t *lowestName = &scoreBoard1_Name;

    if (scoreBoard2_Score < *lowestScore) {
        lowestScore = &scoreBoard2_Score;
        lowestName = &scoreBoard2_Name;
    }

    if (scoreBoard3_Score < *lowestScore) {
        lowestScore = &scoreBoard3_Score;
        lowestName = &scoreBoard3_Name;
    }

    // Replace the lowest score if player's score is higher
    if (playerScore > *lowestScore) {
        *lowestScore = playerScore;
        CopyBuffer(lowestName, &playerName);
    }

    // Ensure scores remain sorted (highest first)
    if (scoreBoard1_Score < scoreBoard2_Score) {
        int tempScore = scoreBoard1_Score;
        scoreBoard1_Score = scoreBoard2_Score;
        scoreBoard2_Score = tempScore;

        Buffer_t tempName;
        CopyBuffer(&tempName, &scoreBoard1_Name);
        CopyBuffer(&scoreBoard1_Name, &scoreBoard2_Name);
        CopyBuffer(&scoreBoard2_Name, &tempName);
    }

    if (scoreBoard2_Score < scoreBoard3_Score) {
        int tempScore = scoreBoard2_Score;
        scoreBoard2_Score = scoreBoard3_Score;
        scoreBoard3_Score = tempScore;

        Buffer_t tempName;
        CopyBuffer(&tempName, &scoreBoard2_Name);
        CopyBuffer(&scoreBoard2_Name, &scoreBoard3_Name);
        CopyBuffer(&scoreBoard3_Name, &tempName);
    }

    if (scoreBoard1_Score < scoreBoard2_Score) {
        int tempScore = scoreBoard1_Score;
        scoreBoard1_Score = scoreBoard2_Score;
        scoreBoard2_Score = tempScore;

        Buffer_t tempName;
        CopyBuffer(&tempName, &scoreBoard1_Name);
        CopyBuffer(&scoreBoard1_Name, &scoreBoard2_Name);
        CopyBuffer(&scoreBoard2_Name, &tempName);
    }
}

//-----------------------------------------------------

#define SERVER_NAME           "a3a2u3sc27qhxv-ats.iot.us-east-1.amazonaws.com"
#define GOOGLE_DST_PORT       8443


#define POSTHEADER "POST /things/DPCS_Lab4_CC3200/shadow HTTP/1.1\r\n"
#define GETHEADER "GET /things/DPCS_Lab4_CC3200/shadow HTTP/1.1\r\n"
#define HOSTHEADER "Host: a3a2u3sc27qhxv-ats.iot.us-east-1.amazonaws.com\r\n"
#define CHEADER "Connection: Keep-Alive\r\n"
#define CTHEADER "Content-Type: application/json; charset=utf-8\r\n"
#define CLHEADER1 "Content-Length: "
#define CLHEADER2 "\r\n\r\n"

#define DATA_ST_DES_VAR "{"                                                 \
            "\"state\": {\r\n"                                              \
                "\"desired\" : {\r\n"                                       \
                    "\"var\" :\""                                           \

#define DATA_WRAP "\"\r\n"                                                  \
                "}"                                                         \
            "}"                                                             \
        "}\r\n\r\n"


int socketID;

//----------------------------------------------------------

static int FetchScoresFromAWS() {
    char acSendBuff[512];
    char acRecvbuff[1460];
    char cCLLength[200];
    char* pcBufHeaders;
    int lRetVal = 0;

    pcBufHeaders = acSendBuff;
    strcpy(pcBufHeaders, GETHEADER);
    pcBufHeaders += strlen(GETHEADER);
    strcpy(pcBufHeaders, HOSTHEADER);
    pcBufHeaders += strlen(HOSTHEADER);
    strcpy(pcBufHeaders, CHEADER);
    pcBufHeaders += strlen(CHEADER);
    strcpy(pcBufHeaders, "\r\n\r\n");

    //
    // Recieve the packet to the server */
    //
    lRetVal = sl_Send(socketID, acSendBuff, strlen(acSendBuff), 0);
    if(lRetVal < 0) {
        UART_PRINT("GET failed. Error Number: %i\n\r",lRetVal);
        sl_Close(socketID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }
    UART_PRINT("-+-+-+ Trying to Receive\r\n",lRetVal);
    lRetVal = sl_Recv(socketID, &acRecvbuff[0], sizeof(acRecvbuff), 0);
    if(lRetVal < 0) {
        UART_PRINT("Received failed. Error Number: %i\n\r",lRetVal);
        //sl_Close(iSSLSockID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
           return lRetVal;
    }
    else {
        acRecvbuff[lRetVal+1] = '\0';
        UART_PRINT(acRecvbuff);
        UART_PRINT("\n\r\n\r");
    }

    return 0;
}

static int SendScoreToAWS() {
    char data[10];

    char acSendBuff[512];
    char acRecvbuff[1460];
    char cCLLength[200];
    char* pcBufHeaders;
    int lRetVal = 0;

    pcBufHeaders = acSendBuff;
    strcpy(pcBufHeaders, POSTHEADER);
    pcBufHeaders += strlen(POSTHEADER);
    strcpy(pcBufHeaders, HOSTHEADER);
    pcBufHeaders += strlen(HOSTHEADER);
    strcpy(pcBufHeaders, CHEADER);
    pcBufHeaders += strlen(CHEADER);
    strcpy(pcBufHeaders, "\r\n\r\n");

    int dataLength = strlen(data) + strlen(DATA_ST_DES_VAR) + strlen(DATA_WRAP);

    strcpy(pcBufHeaders, CTHEADER);
    pcBufHeaders += strlen(CTHEADER);
    strcpy(pcBufHeaders, CLHEADER1);

    pcBufHeaders += strlen(CLHEADER1);
    sprintf(cCLLength, "%d", dataLength);

    strcpy(pcBufHeaders, cCLLength);
    pcBufHeaders += strlen(cCLLength);
    strcpy(pcBufHeaders, CLHEADER2);
    pcBufHeaders += strlen(CLHEADER2);

    strcpy(pcBufHeaders, DATA_ST_DES_VAR);
    pcBufHeaders += strlen(DATA_ST_DES_VAR);

    strcpy(pcBufHeaders, data);
    pcBufHeaders += strlen(data);

    strcpy(pcBufHeaders, DATA_WRAP);
    pcBufHeaders += strlen(DATA_WRAP);

    int testDataLength = strlen(pcBufHeaders);

    UART_PRINT(acSendBuff);

    // Send the packet to the server */
    lRetVal = sl_Send(socketID, acSendBuff, strlen(acSendBuff), 0);
    if(lRetVal < 0) {
        UART_PRINT("POST failed. Error Number: %i\n\r",lRetVal);
        sl_Close(socketID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }
    lRetVal = sl_Recv(socketID, &acRecvbuff[0], sizeof(acRecvbuff), 0);
    if(lRetVal < 0) {
        UART_PRINT("Received failed. Error Number: %i\n\r",lRetVal);
        //sl_Close(iSSLSockID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
           return lRetVal;
    }
    else {
        acRecvbuff[lRetVal+1] = '\0';
        UART_PRINT(acRecvbuff);
        UART_PRINT("\n\r\n\r");
    }

    sl_Stop(SL_STOP_TIMEOUT);
    return 0;
}

void InitAWS() {
    long lRetVal = -1;

    // initialize global default app configuration
    g_app_config.host = SERVER_NAME;
    g_app_config.port = GOOGLE_DST_PORT;

    //Connect the CC3200 to the local access point
    lRetVal = connectToAccessPoint();
    //Set time so that encryption can be used
    lRetVal = set_time();
    if(lRetVal < 0) {
        UART_PRINT("Unable to set time in the device");
        LOOP_FOREVER();
    }
    //Connect to the website with TLS encryption
    lRetVal = tls_connect();
    if(lRetVal < 0) {
        ERR_PRINT(lRetVal);
    }

    socketID = lRetVal;
}


#endif
