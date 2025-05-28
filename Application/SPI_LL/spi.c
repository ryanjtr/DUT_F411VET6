/*
 * spi.c
 *
 *  Created on: Mar 28, 2025
 *      Author: RyanDank
 */


#include "SPI_LL/spi.h"


bool spi_transmit(uint8_t *pData, uint8_t len, uint32_t timeout)
{
    uint32_t count = 0;
    uint8_t index = 0;

    // Chờ cờ BUSY tắt
    while (LL_SPI_IsActiveFlag_BSY(SPI2))
    {
        if (count > timeout)
        {
            return false;
        }
        else
        {
            count++;
        }
    }
    count = 0;

    // Bật ngoại vi SPI
    LL_SPI_Enable(SPI2);

    // Truyền dữ liệu
    while (index < len)
    {
        // Kiểm tra bộ đệm truyền có trống hay không
        if (LL_SPI_IsActiveFlag_TXE(SPI2))
        {
            LL_SPI_TransmitData8(SPI2, pData[index]);
            index++;
            count = 0;
        }
        else
        {
            if (count > timeout)
            {
                return false;
            }
            else
            {
                count++;
            }
        }
    }

    // Chờ cờ BUSY tắt
    while (LL_SPI_IsActiveFlag_BSY(SPI2))
    {
        if (count > timeout)
        {
            return false;
        }
        else
        {
            count++;
        }
    }
    count = 0;

    // Xóa cờ OVERRUN (giữ nguyên như CMSIS)
    (void)LL_SPI_ReceiveData8(SPI2); // Đọc DR

    return true;
}

bool spi_receive(uint8_t *pData, uint8_t len, uint32_t timeout)
{
    uint32_t count = 0;
    uint8_t index = 0;

    // Chờ cờ BUSY tắt
    while (LL_SPI_IsActiveFlag_BSY(SPI2))
    {
        if (count > timeout)
        {
            return false;
        }
        else
        {
            count++;
        }
    }
    count = 0;

    // Bật ngoại vi SPI
    LL_SPI_Enable(SPI2);
    bool isTransmit = 1;

    // Truyền trước sau đó nhận dữ liệu về
    while (index < len)
    {
        // Truyền dữ liệu rác trước
        // Kiểm tra bộ đệm truyền có trống hay không và isTransmit = 1 hay không
        if (LL_SPI_IsActiveFlag_TXE(SPI2) && isTransmit)
        {
            LL_SPI_TransmitData8(SPI2, 0xFF);
            isTransmit = 0;
        }

        // Nhận dữ liệu
        if (LL_SPI_IsActiveFlag_RXNE(SPI2))
        {
            pData[index] = LL_SPI_ReceiveData8(SPI2);
            index++;
            isTransmit = 1;
            count = 0;
        }
        else
        {
            if (count > timeout)
            {
                return false;
            }
            else
            {
                count++;
            }
        }
    }

    // Chờ cờ BUSY tắt
    while (LL_SPI_IsActiveFlag_BSY(SPI2))
    {
        if (count > timeout)
        {
            return false;
        }
        else
        {
            count++;
        }
    }
    count = 0;

    // Xóa cờ OVERRUN
    LL_SPI_ClearFlag_OVR(SPI2);
    return true;
}



bool spi_transmit_receive(uint8_t *txdata, uint8_t *rxdata, uint8_t len, uint32_t timeout)
{
    uint32_t count = 0;
    uint8_t index = 0;

    // Chờ cờ BUSY tắt
    while (LL_SPI_IsActiveFlag_BSY(SPI2))
    {
        if (count > timeout)
        {
            return false;
        }
        count++;
    }
    count = 0;

    // Bật ngoại vi SPI
    LL_SPI_Enable(SPI2);
    bool isTransmit = 1;

    while (index < len)
    {
        // Kiểm tra bộ đệm truyền trống và isTransmit = 1
        if (LL_SPI_IsActiveFlag_TXE(SPI2) && isTransmit)
        {
            LL_SPI_TransmitData8(SPI2, txdata[index]);
            isTransmit = 0;
        }
//        for(uint32_t i=0;i<100000;i++);
        // Nhận dữ liệu
        if (LL_SPI_IsActiveFlag_RXNE(SPI2))
        {
            rxdata[index] = LL_SPI_ReceiveData8(SPI2);
            index++;
            isTransmit = 1;
            count = 0;
        }
        else
        {
            if (count > timeout)
            {
                return false;
            }
            count++;
        }
    }

    // Chờ cờ BUSY tắt
    while (LL_SPI_IsActiveFlag_BSY(SPI2))
    {
        if (count > timeout)
        {
            return false;
        }
        count++;
    }
    count = 0;

    // Xóa cờ OVERRUN
    LL_SPI_ClearFlag_OVR(SPI2);

    return true;
}
