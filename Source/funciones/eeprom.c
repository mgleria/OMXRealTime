/*
 * Notas para revisar/corregir:
 * 1. La función MCHP_24LCxxx_Read_array() retorna siempre '2' cuando debería retornar '4'. El '2' significa
 * que saltó el TIMEOUT (500) asignado, pero de igual manera los datos son leídos correctamente.
 * El dato curioso es que para 15 datos o menos, esto no ocurre. Para 16 datos o más si ocurre. 
 * Debe ser una demora en el chip por algún cambio de página o similar. Aparentemente no es relativo a la posición
 * inicial de lectura.
 * 2. Según las pruebas que se realizaron, entre una operación de escritura y otra de lectura debe existir un delay.
 * No se estableció de cuanto debería ser. 
 * 3. Hay que definir los valores de retorno en el archivo de encabezado para utilizar siempre el mismo criterio.
 *  
 */

#include "i2c1.h"
#include "portmacro.h"
#include "perifericos/led.h"
#include "funciones/eeprom.h"

#define     initDataAddress         0x0000
#define     initBufferDataSize      16

// Initialize an EEPROM Device with some data
uint8_t MCHP_24LCxxx_Init_I2C1(uint8_t chip){
    
    uint8_t         sourceData[initBufferDataSize] = {0xA0, 0xA1, 0xA2, 0xA3, 
                                                      0xA4, 0xA5, 0xA6, 0xA7, 
                                                      0xA8, 0xA9, 0xAA, 0xAB, 
                                                      0xAC, 0xAD, 0xAE, 0xAF }; 
    
    return MCHP_24LCxxx_Write_array(chip,initDataAddress,sourceData,initBufferDataSize);
}  

uint8_t MCHP_24LCxxx_Write_byte(
                                    uint8_t     chip,    
                                    uint16_t    address,
                                    uint8_t     *pData)
{
    return MCHP_24LCxxx_Write_array(chip,address,pData,1);
}

//Devuelve siempre 2, pero funciona. Habria que determinar si es una error de 
//programación o algo del protocolo
uint8_t MCHP_24LCxxx_Write_array(
                                    uint8_t     chip,    
                                    uint16_t    address,
                                    uint8_t     *pData,
                                    uint16_t    nCount)
{
    uint8_t         writeBuffer[BUFFER_SIZE];
    uint16_t        timeOut, slaveTimeOut;

    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;

    // building writeBuffer
    // starting address of the EEPROM memory
    writeBuffer[0] = (address >> 8);                // high address
    writeBuffer[1] = (uint8_t)(address);            // low low address
    // data to be written
    int i;
    for(i=0;i<nCount;i++) writeBuffer[i+2]=pData[i];

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
                            chip,
                            &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C1_MESSAGE_PENDING)
        {
            // add some delay here

            // timeout checking
            // check for max retry and skip this byte
            if (slaveTimeOut == SLAVE_I2C_GENERIC_DEVICE_TIMEOUT){
                break;
            }
                
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
            break;
        }
        else
            timeOut++;
    }
    
    if(status == I2C1_MESSAGE_FAIL)
        return 0;
    else
        return 1;
}

uint8_t MCHP_24LCxxx_Read_byte(
                                uint8_t     chip,
                                uint16_t    address,
                                uint8_t     *pData)
{
    return MCHP_24LCxxx_Read_array(chip,address,pData,1);
}

uint8_t MCHP_24LCxxx_Read_array(
                                uint8_t     chip,
                                uint16_t    address,
                                uint8_t     *pData,
                                uint16_t    nCount)
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
                                    chip,
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
                                        chip,
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

