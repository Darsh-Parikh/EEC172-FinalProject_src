#ifndef BUFFER_H
#define BUFFER_H

//--------------------------------
//         BUFFERS
//--------------------------------

#define BUFFER_SIZE 128

typedef struct {
    int pos;
    char buf[BUFFER_SIZE];
    int size;
    char terminator;
} Buffer_t;

Buffer_t BufferInternal = {0, {}, BUFFER_SIZE-1, '\0'};
Buffer_t BufferExternal = {0, {}, BUFFER_SIZE-1, '\0'};

void ClearBuffer(Buffer_t *buff) {
    buff->pos = 0;
    buff->buf[0] = buff->terminator;
    buff->size = BUFFER_SIZE - 1;
}

// overflow = 1 ==> buffer will be cleared and new entry will start from the beginning
// overflow = 0 ==> nothing will be added, and the return value will be 0
// otherwise return value is always 1
int AddToBuffer(Buffer_t *buff, char entry, int overflow) {
    if (buff->pos == buff->size) {
        if (overflow) {
            buff->pos = 0;
        } else {
            return 0;
        }
    }

    buff->buf[buff->pos++] = entry;
    buff->buf[buff->pos] = buff->terminator;

    return 1;
}

void EditLastInBuffer(Buffer_t *buff, char entry) {
    if ((buff->pos) != 0) {
        buff->buf[(buff->pos) - 1] = entry;
    }
}

// return 1 if the buffer is empty
int RemoveLastInBuffer(Buffer_t *buff) {
    if (buff->pos == 0) {
        return 1;
    }

    buff->pos -= 1;
    buff->buf[buff->pos] = buff->terminator;
    return !(buff->pos);
}

void PrintBuffer(Buffer_t *buff) {
    int i = 0;
    Report("{");
    while (buff->buf[i] != buff->terminator) {
        Report(" %c ,", (buff->buf[i]));
        i++;
    }
    Report("}\r\n");
}

//--------------------------------
//         COMMS
//--------------------------------

#define COMMS_UART_BAUD 115200
#define COMMS_UART      UARTA1_BASE
#define COMMS_PERIPH    PRCM_UARTA1

#define MESSAGE_END_CHAR '!'
volatile int new_message_recieved = 0;

void CommsUARTInterruptHandler() {
    if (UARTIntStatus(COMMS_UART, 0) & UART_INT_RX) {
        UARTIntClear(COMMS_UART, UART_INT_RX);

        char c = UARTCharGet(COMMS_UART);

        if (c == MESSAGE_END_CHAR) {
            new_message_recieved = 1;
        } else if (((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'Z'))) {
            AddToBuffer(&BufferExternal, c, 1);
            new_message_recieved = 0;
        }

        // MAP_UARTCharPut(UARTA0_BASE, c);
    }

    return;
}

void InitComm() {
  MAP_UARTConfigSetExpClk(
          COMMS_UART,MAP_PRCMPeripheralClockGet(COMMS_PERIPH),
          COMMS_UART_BAUD,
          (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE)
  );

  UARTEnable(COMMS_UART);

  UARTFIFOEnable(COMMS_UART);
  UARTFIFOLevelSet(COMMS_UART, UART_FIFO_TX7_8, UART_FIFO_RX1_8);

  UARTIntEnable(COMMS_UART, UART_INT_RX);
  UARTIntRegister(COMMS_UART, &CommsUARTInterruptHandler);
}

void CommsTransmitBuffer(Buffer_t *buff) {
    if (buff == NULL) {
        return;
    }

    int i = 0;
    for (i = 0; i < buff->size; i++) {
        char c = (char) buff->buf[i];
        if (c == buff->terminator) {
            break;
        }
        MAP_UARTCharPut(COMMS_UART, c);
    }
    MAP_UARTCharPut(COMMS_UART, MESSAGE_END_CHAR);
}

void CommsTransmitString(char *str) {
    int i = 0;
    int busy = UARTBusy(COMMS_UART);
    int poss = UARTSpaceAvail(COMMS_UART);
    while (str[i]) {
        MAP_UARTCharPut(COMMS_UART, str[i]);
        i++;
    }
}

#endif
