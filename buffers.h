#ifndef BUFFER_H
#define BUFFER_H

//--------------------------------
//         NAME BUFFER
//--------------------------------

#define BUFFER_SIZE 1024

typedef struct {
    int pos;
    char buf[BUFFER_SIZE];
    int size;
    char terminator;
} Buffer_t;

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

void CopyBuffer(Buffer_t *dst, Buffer_t *src) {
    dst->pos = src->pos;
    dst->size = src->size;
    dst->terminator = src->terminator;

    int i = 0;
    for (;;i++) {
        dst->buf[i] = src->buf[i];

        if (src->buf[i] == src->terminator) {
            break;
        }
    }
}

//--------------------------------
//         CONTROLLER DATA
//--------------------------------

char DataBuffer[6]; // { Btn1, Btn2,  AccelX, AccelY, TVchar , TVstate }

void StoreControllerData(
        int button1State,
        int button2State,
        int accelX,
        int accelY,
        char tvBtn,
        int tvBtnSt
) {
    DataBuffer[0] = button1State;
    DataBuffer[1] = button2State;
    DataBuffer[2] = accelX;
    DataBuffer[3] = accelY;
    DataBuffer[4] = tvBtn;
    DataBuffer[5] = tvBtnSt; //
}

void ClearControllerData() {
    DataBuffer[0] = '0';
    DataBuffer[1] = '0';
    DataBuffer[2] = '0';
    DataBuffer[3] = '0';
    DataBuffer[4] = '1';
    DataBuffer[5] = '0';
}

//--------------------------------
//         COMMS
//--------------------------------

#define COMMS_UART_BAUD 115200
#define COMMS_UART      UARTA1_BASE
#define COMMS_PERIPH    PRCM_UARTA1

#define MESSAGE_END_CHAR '!'

void CommsUARTInterruptHandler() {
    if (UARTIntStatus(COMMS_UART, 0) & UART_INT_RX) {
        UARTIntClear(COMMS_UART, UART_INT_RX);

        char c;

        int i = 0;
        for (i = 0; i < 6; i++) {
            c = UARTCharGet(COMMS_UART);

            if (((c > '0') && (c < '9')) || ((c > 'A') && (c < 'Z'))) {
                DataBuffer[i] = c;
            } else {
                i--;
            }
        }
    }

    return;
}

void InitComm(int enableInterrupts) {
  MAP_UARTConfigSetExpClk(
          COMMS_UART,MAP_PRCMPeripheralClockGet(COMMS_PERIPH),
          COMMS_UART_BAUD,
          (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE)
  );

  UARTEnable(COMMS_UART);

  UARTFIFOEnable(COMMS_UART);

  if (enableInterrupts) {
      UARTFIFOLevelSet(COMMS_UART, UART_FIFO_TX7_8, UART_FIFO_RX1_8);

      UARTIntEnable(COMMS_UART, UART_INT_RX);
      UARTIntRegister(COMMS_UART, &CommsUARTInterruptHandler);
  }
}

void CommsTransferControllerData() {
    int i = 0;
    for (i = 0; i < 6; i++) {
        MAP_UARTCharPut(COMMS_UART, DataBuffer[i]);
    }
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

#endif
