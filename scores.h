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


#define POSTHEADER "POST /things/SpaceShooter_HighScores/shadow HTTP/1.1\r\n"
#define GETHEADER "GET /things/SpaceShooter_HighScores/shadow HTTP/1.1\r\n"
#define HOSTHEADER "Host: a3a2u3sc27qhxv-ats.iot.us-east-1.amazonaws.com\r\n"
#define CHEADER "Connection: Keep-Alive\r\n"
#define CTHEADER "Content-Type: application/json; charset=utf-8\r\n"
#define CLHEADER1 "Content-Length: "
#define CLHEADER2 "\r\n\r\n"

#define STATE_START "{\r\n\"state\": {\r\n"
#define STATE_END "}\r\n}\r\n\r\n"

#define PLAYER_TEMPLATE "\"player%d\": { \"name\": \"%s\", \"score\": %d }"
#define PLAYER_SEPARATOR ",\r\n"

#define MAX_LEADERBOARD_SIZE 3

int socketID;

//----------------------------------------------------------

static int FetchScoresFromAWS() {
    char acSendBuff[512];
    char acRecvbuff[1460];          // make this larger?
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

    // Send request to AWS
    lRetVal = sl_Send(socketID, acSendBuff, strlen(acSendBuff), 0);
    if (lRetVal < 0) {
        UART_PRINT("GET failed. Error Number: %i\n\r", lRetVal);
        sl_Close(socketID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }

    // Receive data from AWS
    UART_PRINT("-+-+-+ Trying to Receive\r\n");
    lRetVal = sl_Recv(socketID, &acRecvbuff[0], sizeof(acRecvbuff), 0);
    if (lRetVal < 0) {
        UART_PRINT("Received failed. Error Number: %i\n\r", lRetVal);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    } else {
        acRecvbuff[lRetVal] = '\0';
        UART_PRINT(acRecvbuff);
        UART_PRINT("\n\r\n\r");
    }

    // Extracting scores from JSON response
    char* ptr = acRecvbuff;
    int extractedScores[MAX_LEADERBOARD_SIZE];
    Buffer_t extractedNames[MAX_LEADERBOARD_SIZE];

    int i = 0;
    for (i = 0; i < MAX_LEADERBOARD_SIZE; i++) {
        char name[BUFFER_SIZE] = {0};
        int score = 0;

        char searchKey[20];
        sprintf(searchKey, "\"player%d\": { \"name\": \"", i + 1);
        ptr = strstr(ptr, searchKey);
        if (!ptr) break;

        ptr += strlen(searchKey);
        sscanf(ptr, "%[^\"]", name); // Read player name

        ptr = strstr(ptr, "\"score\": ");
        if (!ptr) break;

        ptr += strlen("\"score\": ");
        sscanf(ptr, "%d", &score); // Read player score

        extractedScores[i] = score;
        ClearBuffer(&extractedNames[i]);
        int j = 0;
        for (j = 0; j < strlen(name); j++) {
            AddToBuffer(&extractedNames[i], name[j], 1);
        }
    }

    // Updating the local scoreboard
    CopyBuffer(&scoreBoard1_Name, &extractedNames[0]);
    scoreBoard1_Score = extractedScores[0];

    CopyBuffer(&scoreBoard2_Name, &extractedNames[1]);
    scoreBoard2_Score = extractedScores[1];

    CopyBuffer(&scoreBoard3_Name, &extractedNames[2]);
    scoreBoard3_Score = extractedScores[2];

    return 0;
}


static int SendScoreToAWS() {
    char acSendBuff[512];
    char acRecvbuff[1460];
    char cCLLength[200];
    char* pcBufHeaders;
    int lRetVal = 0;
    char data[(MAX_LEADERBOARD_SIZE+1) * BUFFER_SIZE];

    // Constructing the JSON-like state string
    snprintf(data, sizeof(data),
        STATE_START
        PLAYER_TEMPLATE PLAYER_SEPARATOR
        PLAYER_TEMPLATE PLAYER_SEPARATOR
        PLAYER_TEMPLATE
        STATE_END,
        1, scoreBoard1_Name.buf, scoreBoard1_Score,
        2, scoreBoard2_Name.buf, scoreBoard2_Score,
        3, scoreBoard3_Name.buf, scoreBoard3_Score
    );

    int dataLength = strlen(data);

    // Setting up HTTP headers
    pcBufHeaders = acSendBuff;
    strcpy(pcBufHeaders, POSTHEADER);
    pcBufHeaders += strlen(POSTHEADER);
    strcpy(pcBufHeaders, HOSTHEADER);
    pcBufHeaders += strlen(HOSTHEADER);
    strcpy(pcBufHeaders, CHEADER);
    pcBufHeaders += strlen(CHEADER);
    strcpy(pcBufHeaders, CTHEADER);
    pcBufHeaders += strlen(CTHEADER);
    strcpy(pcBufHeaders, CLHEADER1);
    pcBufHeaders += strlen(CLHEADER1);

    sprintf(cCLLength, "%d", dataLength);
    strcpy(pcBufHeaders, cCLLength);
    pcBufHeaders += strlen(cCLLength);
    strcpy(pcBufHeaders, CLHEADER2);
    pcBufHeaders += strlen(CLHEADER2);

    // Append the data
    strcpy(pcBufHeaders, data);
    pcBufHeaders += strlen(data);

    // Debugging output
    UART_PRINT(acSendBuff);

    // Send the packet to AWS
    lRetVal = sl_Send(socketID, acSendBuff, strlen(acSendBuff), 0);
    if (lRetVal < 0) {
        UART_PRINT("POST failed. Error Number: %i\n\r", lRetVal);
        sl_Close(socketID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }

    // Receive response from AWS
    lRetVal = sl_Recv(socketID, &acRecvbuff[0], sizeof(acRecvbuff), 0);
    if (lRetVal < 0) {
        UART_PRINT("Received failed. Error Number: %i\n\r", lRetVal);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    } else {
        acRecvbuff[lRetVal + 1] = '\0';
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

void TestAWS() {
    InitAWS();

    strcpy(scoreBoard1_Name.buf, "Player 1");
    scoreBoard1_Score = 100;

    strcpy(scoreBoard2_Name.buf, "Player 2");
    scoreBoard2_Score = 90;

    strcpy(scoreBoard3_Name.buf, "Player 3");
    scoreBoard3_Score = 80;

    SendScoreToAWS();

    ClearBuffer(&scoreBoard1_Name); scoreBoard1_Score = 0;
    ClearBuffer(&scoreBoard2_Name); scoreBoard2_Score = 0;
    ClearBuffer(&scoreBoard3_Name); scoreBoard3_Score = 0;

    FetchScoresFromAWS();

    Report("1st: %s , (%d)", scoreBoard1_Name.buf, scoreBoard1_Score);
    Report("2nd: %s , (%d)", scoreBoard2_Name.buf, scoreBoard2_Score);
    Report("3rd: %s , (%d)", scoreBoard3_Name.buf, scoreBoard3_Score);

    while (1) {

    }
}


#endif
