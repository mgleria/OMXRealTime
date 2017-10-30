#include "i2c1.h"
#include "portmacro.h"
#include "perifericos/led.h"

//#define slaveDeviceAddress                  0x50
#define slaveDeviceAddress                  0x51 
#define SLAVE_I2C_GENERIC_RETRY_MAX         500
#define SLAVE_I2C_GENERIC_DEVICE_TIMEOUT    500

#define BUFFER_SIZE                         18      //2 más de la cantidad de datos que pretendo transmitir


uint8_t MCHP24AA512_Read_1(
                                uint16_t address,
                                uint8_t *pData,
                                uint16_t nCount)
{
    I2C1_MESSAGE_STATUS status;
    uint8_t     writeBuffer[3];
    uint16_t    retryTimeOut, slaveTimeOut;
    uint16_t    counter;
    uint8_t     *pD;

    pD = pData;
    status = I2C1_MESSAGE_PENDING;

    for (counter = 0; counter < nCount; counter++)
    {

        // build the write buffer first
        // starting address of the EEPROM memory
        writeBuffer[0] = (address >> 8);                // high address
        writeBuffer[1] = (uint8_t)(address);            // low low address

        // Now it is possible that the slave device will be slow.
        // As a work around on these slaves, the application can
        // retry sending the transaction
        retryTimeOut = 0;
        slaveTimeOut = 0;

        while(status != I2C1_MESSAGE_FAIL)
        {
            // write one byte to EEPROM (2 is the count of bytes to write)
            I2C1_MasterWrite(    writeBuffer,
                                    2,
                                    slaveDeviceAddress,
                                    &status);

            // wait for the message to be sent or status has changed.
            while(status == I2C1_MESSAGE_PENDING)
            {
                // add some delay here

                // timeout checking
                // check for max retry and skip this byte
                if (slaveTimeOut == SLAVE_I2C_GENERIC_DEVICE_TIMEOUT){
//                    vLedSetLED(0,1); //Enciende el LED1 (D3)
                    return (1);
                }
                else
                    slaveTimeOut++;
            }

            if (status == I2C1_MESSAGE_COMPLETE)
                break;

            // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
            //               or I2C1_DATA_NO_ACK,
            // The device may be busy and needs more time for the last
            // write so we can retry writing the data, this is why we
            // use a while loop here

            // check for max retry and skip this byte
            if (retryTimeOut == SLAVE_I2C_GENERIC_RETRY_MAX)
                break;
            else
                retryTimeOut++;
        }

        if (status == I2C1_MESSAGE_COMPLETE)
        {

            // this portion will read the byte from the memory location.
            retryTimeOut = 0;
            slaveTimeOut = 0;

            while(status != I2C1_MESSAGE_FAIL)
            {
                // read one byte from EEPROM (2 is the count of bytes to write)
                I2C1_MasterRead(        pD,
                                        1,
                                        slaveDeviceAddress,
                                        &status);

                // wait for the message to be sent or status has changed.
                while(status == I2C1_MESSAGE_PENDING)
                {
                    // add some delay here

                    // timeout checking
                    // check for max retry and skip this byte
                    if (slaveTimeOut == SLAVE_I2C_GENERIC_DEVICE_TIMEOUT){
//                        vLedSetLED(1,1); //Enciende el LED1 (D4)
                        return (2);
                    }
                    else
                        slaveTimeOut++;
                }

                if (status == I2C1_MESSAGE_COMPLETE)
//                    vLedSetLED(1,1); //Enciende el LED1 (D4)
                    break;

                // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
                //               or I2C1_DATA_NO_ACK,
                // The device may be busy and needs more time for the last
                // write so we can retry writing the data, this is why we
                // use a while loop here

                // check for max retry and skip this byte
                if (retryTimeOut == SLAVE_I2C_GENERIC_RETRY_MAX)
                    break;
                else
                    retryTimeOut++;
            }
        }

        // exit if the last transaction failed
        if (status == I2C1_MESSAGE_FAIL)
        {
//            vLedSetLED(2,1); //Enciende el LED1 (D5)
            return(3);
            break;
        }

        pD++;
        address++;

    }
//    vLedSetLED(3,1); //Enciende el LED1 (D6)
    return(4);

}

uint8_t MCHP24AA512_Read_2(
                                uint16_t address,
                                uint8_t *pData,
                                uint16_t nCount)
{
    I2C1_MESSAGE_STATUS status;
    uint8_t     writeBuffer[3];
    uint16_t    retryTimeOut, slaveTimeOut;
    uint16_t    counter;
    uint8_t     *pD;

    pD = pData;
    status = I2C1_MESSAGE_PENDING;

    //Se mantiene el ciclo for para la utilización de las sentencias 'break'
    for (counter = 0; counter < 1; counter++)
    {

        // build the write buffer first
        // starting address of the EEPROM memory
        writeBuffer[0] = (address >> 8);                // high address
        writeBuffer[1] = (uint8_t)(address);            // low low address

        // Now it is possible that the slave device will be slow.
        // As a work around on these slaves, the application can
        // retry sending the transaction
        retryTimeOut = 0;
        slaveTimeOut = 0;

        while(status != I2C1_MESSAGE_FAIL)
        {
            // write one byte to EEPROM (2 is the count of bytes to write)
            I2C1_MasterWrite(    writeBuffer,
                                    2,
                                    slaveDeviceAddress,
                                    &status);

            // wait for the message to be sent or status has changed.
            while(status == I2C1_MESSAGE_PENDING)
            {
                // add some delay here

                // timeout checking
                // check for max retry and skip this byte
                if (slaveTimeOut == SLAVE_I2C_GENERIC_DEVICE_TIMEOUT){
//                    vLedSetLED(0,1); //Enciende el LED1 (D3)
                    return (1);
                }
                else
                    slaveTimeOut++;
            }

            if (status == I2C1_MESSAGE_COMPLETE)
                break;

            // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
            //               or I2C1_DATA_NO_ACK,
            // The device may be busy and needs more time for the last
            // write so we can retry writing the data, this is why we
            // use a while loop here

            // check for max retry and skip this byte
            if (retryTimeOut == SLAVE_I2C_GENERIC_RETRY_MAX)
                break;
            else
                retryTimeOut++;
        }

        if (status == I2C1_MESSAGE_COMPLETE)
        {

            // this portion will read the byte from the memory location.
            retryTimeOut = 0;
            slaveTimeOut = 0;

            while(status != I2C1_MESSAGE_FAIL)
            {
                // read one byte from EEPROM (2 is the count of bytes to write)
                I2C1_MasterRead(        pD,
                                        nCount,
                                        slaveDeviceAddress,
                                        &status);

                // wait for the message to be sent or status has changed.
                while(status == I2C1_MESSAGE_PENDING)
                {
                    // add some delay here

                    // timeout checking
                    // check for max retry and skip this byte
                    if (slaveTimeOut == SLAVE_I2C_GENERIC_DEVICE_TIMEOUT){
//                        vLedSetLED(1,1); //Enciende el LED1 (D4)
                        return (2);
                    }
                    else
                        slaveTimeOut++;
                }

                if (status == I2C1_MESSAGE_COMPLETE)
//                    vLedSetLED(1,1); //Enciende el LED1 (D4)
                    break;

                // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
                //               or I2C1_DATA_NO_ACK,
                // The device may be busy and needs more time for the last
                // write so we can retry writing the data, this is why we
                // use a while loop here

                // check for max retry and skip this byte
                if (retryTimeOut == SLAVE_I2C_GENERIC_RETRY_MAX)
                    break;
                else
                    retryTimeOut++;
            }
        }

        // exit if the last transaction failed
        if (status == I2C1_MESSAGE_FAIL)
        {
//            vLedSetLED(2,1); //Enciende el LED1 (D5)
            return(3);
            break;
        }

        pD++;
        address++;

    }
//    vLedSetLED(3,1); //Enciende el LED1 (D6)
    return(4);

}

// Initialize an EEPROM Device with some data
void MCHP24AA512_Init_I2C1(){
    
    uint16_t        dataAddress;
    uint8_t         sourceData[16] = {  0xA0, 0xA1, 0xA2, 0xA3, 
                                        0xA4, 0xA5, 0xA6, 0xA7, 
                                        0xA8, 0xA9, 0xAA, 0xAB, 
                                        0xAC, 0xAD, 0xAE, 0xAF }; 
    uint8_t         *pData;

    uint8_t         writeBuffer[BUFFER_SIZE];
    uint8_t         *pD;
    uint16_t        counter, timeOut, slaveTimeOut;

    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;

    dataAddress = 0x10;             // starting EEPROM address 
    pD = sourceData;                // initialize the source of the data

    // build the write buffer first
    // starting address of the EEPROM memory
    writeBuffer[0] = (dataAddress >> 8);                // high address
    writeBuffer[1] = (uint8_t)(dataAddress);            // low low address

    // data to be written
    int i;
    for(i=0;i<BUFFER_SIZE-2;i++) writeBuffer[i+2]=sourceData[i];
//        writeBuffer[2] = *pD++;
//        writeBuffer[3] = *pD++;

    // Now it is possible that the slave device will be slow.
    // As a work around on these slaves, the application can
    // retry sending the transaction
    timeOut = 0;
    slaveTimeOut = 0;

    while(status != I2C1_MESSAGE_FAIL)
    {
        // write one byte to EEPROM (3 is the number of bytes to write)
        I2C1_MasterWrite(   writeBuffer,
                            BUFFER_SIZE,
                            slaveDeviceAddress,
                            &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C1_MESSAGE_PENDING)
        {
            // add some delay here

            // timeout checking
            // check for max retry and skip this byte
            if (slaveTimeOut == SLAVE_I2C_GENERIC_DEVICE_TIMEOUT)
                break;
            else
                slaveTimeOut++;
        } 
        if ((slaveTimeOut == SLAVE_I2C_GENERIC_DEVICE_TIMEOUT) || 
            (status == I2C1_MESSAGE_COMPLETE))
            break;

        // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
        //               or I2C1_DATA_NO_ACK,
        // The device may be busy and needs more time for the last
        // write so we can retry writing the data, this is why we
        // use a while loop here

        // check for max retry and skip this byte
        if (timeOut == SLAVE_I2C_GENERIC_RETRY_MAX){
//                vLedSetLED(0,1); //Enciende el LED1 (D3)
            break;
        }
        else
            timeOut++;
    }

//    if (status == I2C1_MESSAGE_FAIL)
//    {
////            vLedSetLED(1,1); //Enciende el LED1 (D4)
//        break;
//    }
}  
