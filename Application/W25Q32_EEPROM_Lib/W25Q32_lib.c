/*
 * W25Q32_lib.c
 *
 *  Created on: Mar 28, 2025
 *      Author: RyanDank
 */


#include "W25Q32_lib.h"
#include "uart_log.h"
void W25Q32JV_Master_ReadJEDECID(uint8_t rxdata[4])
{
  //Dùng salae để coi ID W25Q32
  uint8_t txdata[4] = {0x9F,0x00,0x00,0x00};
  LL_GPIO_ResetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);
  spi_transmit_receive(txdata, rxdata, 4, 1000);
  LL_GPIO_SetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);

//  for(int i=0;i<1000;++i);//Delay
}



void W25Q32JV_Master_PageProgram(uint32_t Address, uint8_t *pdata, uint16_t size)
{
    uint8_t enable_write_instruction = 0x06;
    uint8_t write_page_instruction = 0x02;
    W25Q32JV_Master_EraseSector(Address);
    // Kéo CS xuống mức 0 để bắt đầu giao tiếp
    LL_GPIO_ResetOutputPin(SPI2_CSS_GPIO_Port,SPI2_CSS_Pin);
    // Bật tính năng ghi bằng lệnh Write Enable (0x06)
    spi_transmit(&enable_write_instruction, 1, 1000);
    // Kéo CS lên mức 1 để hoàn thành lệnh
    LL_GPIO_SetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);

    // Kéo CS xuống mức 0 để bắt đầu lệnh ghi trang
    LL_GPIO_ResetOutputPin(SPI2_CSS_GPIO_Port,SPI2_CSS_Pin);
    // Gửi lệnh ghi trang (0x02)
    spi_transmit(&write_page_instruction, 1, 1000);
    // Gửi địa chỉ 24-bit (A23-A0)
    uint8_t addr_bytes[3];
    addr_bytes[0] = (Address >> 16) & 0xFF; // A23-A16
    addr_bytes[1] = (Address >> 8) & 0xFF;  // A15-A8
    addr_bytes[2] = Address & 0xFF;         // A7-A0
    spi_transmit(addr_bytes, 3, 1000);
    // Gửi dữ liệu cần ghi
    spi_transmit(pdata, size, 1000);
    // Kéo CS lên mức 1 để hoàn thành lệnh
    LL_GPIO_SetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);


}

void W25Q32JV_Master_ReadData(uint32_t Address, uint8_t *pdata, uint16_t size)
{
    if (Address + size > 0x400000) { // Kiểm tra vượt quá 4MB
        return; // Hoặc xử lý lỗi
    }

    uint8_t read_page_instruction = 0x03;
    LL_GPIO_ResetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);

    if (!spi_transmit(&read_page_instruction, 1, 1000)) {
    	uart_printf("fail 1\r\n");
        LL_GPIO_SetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);
        return;
    }

    uint8_t addr_bytes[3] = {
        (Address >> 16) & 0xFF,
        (Address >> 8) & 0xFF,
        Address & 0xFF
    };
    if (!spi_transmit(addr_bytes, 3, 1000)) {
    	uart_printf("fail 2\r\n");
        LL_GPIO_SetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);
        return;
    }

    if (!spi_receive(pdata, size, 1000)) {
    	uart_printf("fail 3\r\n");
        LL_GPIO_SetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);
        return;
    }

    LL_GPIO_SetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);
    // Bỏ delay vì không cần thiết
}



void W25Q32JV_Master_EraseSector(uint32_t Address)
{
    uint8_t enable_write_instruction = 0x06;
    uint8_t erase_sector_instruction = 0x20;
    uint8_t addr_bytes[3];


    // Tách các byte từ địa chỉ
    addr_bytes[0] = (Address >> 16) & 0xFF;
    addr_bytes[1] = (Address >> 8) & 0xFF;
    addr_bytes[2] = Address & 0xFF;

    // Bật tính năng ghi bằng lệnh Write Enable (0x06)
    LL_GPIO_ResetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);
    spi_transmit(&enable_write_instruction, 1, 1000);
    LL_GPIO_SetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);

    // Gửi lệnh xóa sector (0x20)
    LL_GPIO_ResetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);
    spi_transmit(&erase_sector_instruction, 1, 1000);
    spi_transmit(addr_bytes, 3, 1000);
    LL_GPIO_SetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);


#if REAL_W25Q32
    uint8_t status;
    // Kiểm tra trạng thái lệnh xóa hoàn thành
    do {
        // Gửi lệnh đọc trạng thái (0x05)
        uint8_t read_status_instruction = 0x05;
        LL_GPIO_ResetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);
        spi_transmit(&read_status_instruction, 1, 1000);
        spi_receive(&status, 1, 1000); // Đọc trạng thái

        // Kéo CS lên mức 1 sau khi đọc
        LL_GPIO_SetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);

    } while (status & 0x01);  // Kiểm tra bit BUSY (bit 0 của Status Register)
#endif

    // Tùy chọn: Thêm một khoảng delay nếu cần thiết (nếu không sử dụng kiểm tra STATUS)
     for (int i = 0; i < 100000; ++i);  // Cũng có thể dùng để delay tùy chỉnh
}


void W25Q32JV_Master_Reset(void)
{
  uint8_t enable_reset_instruction = 0x66;
  uint8_t reset_instruction = 0x99;

  LL_GPIO_ResetOutputPin(SPI2_CSS_GPIO_Port,SPI2_CSS_Pin);
  spi_transmit(&enable_reset_instruction, 1, 1000);
  LL_GPIO_SetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);

  LL_GPIO_ResetOutputPin(SPI2_CSS_GPIO_Port,SPI2_CSS_Pin);
  spi_transmit(&reset_instruction, 1, 1000);
  LL_GPIO_SetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);
  for(int i=0;i<1000;i++);

}

void W25Q32JV_Master_EraseChip(void)
{
  uint8_t enable_write_instruction = 0x06;
  uint8_t chip_erase_instruction = 0x60;

  LL_GPIO_ResetOutputPin(SPI2_CSS_GPIO_Port,SPI2_CSS_Pin);
  spi_transmit(&enable_write_instruction, 1, 1000);
  LL_GPIO_SetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);

  LL_GPIO_ResetOutputPin(SPI2_CSS_GPIO_Port,SPI2_CSS_Pin);
  spi_transmit(&chip_erase_instruction, 1, 1000);
  LL_GPIO_SetOutputPin(SPI2_CSS_GPIO_Port, SPI2_CSS_Pin);

//  for(int32_t i=0;i<1000;i++);//Delay
}
