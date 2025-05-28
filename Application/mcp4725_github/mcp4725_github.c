/***************************************************************************************************/
/*
   This is a HAL based library for MCP4725, 12-bit Digital-to-Analog Converter with EEPROM

   NOTE:
   - operating/reference voltage 2.7v - 5.5v
   - add 100μF & 0.1 μF bypass capacitors within 4mm to Vdd
   - output voltage from 0 to operating voltage
   - maximum output current 25mA
   - output impedance 1 Ohm
   - maximum output load 1000pF/0.001μF in parallel with 5 kOhm
   - voltage settling time 6 μsec - 10 μsec
   - slew rate 0.55 V/μs
   - device has 14-bit EEPROM with on-chip charge pump circuit for fail-safe writing
   - estimated EEPROM endurance 1 million write cycles
   - if Vdd < 2v all circuits & output disabled, when Vdd
     increases above Vpor device takes a reset state & upload data from EEPROM

   ported by : Salman Motlaq
   sourse code: https://github.com/SMotlaq

   from an Arduino lib written by : enjoyneering79
   sourse code: https://github.com/enjoyneering

   GNU GPL license, all text above must be included in any redistribution,
   see link for details  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/

#include "mcp4725_github.h"
#include "i2c.h"

/**************************************************************************/
/*
    MCP4725_init()

    Constructor
*/
/**************************************************************************/
MCP4725 MCP4725_init(I2C_HandleTypeDef *hi2c, MCP4725Ax_ADDRESS addr, float refV)
{
  MCP4725 _MCP4725;

  _MCP4725._i2cAddress = (uint16_t)(addr << 1);
  _MCP4725.hi2c = hi2c;

  MCP4725_setReferenceVoltage(&_MCP4725, refV); // set _refVoltage & _bitsPerVolt variables

  return _MCP4725;
}

/**************************************************************************/
/*
    MCP4725_isConnected()

    Check the connection
*/
/**************************************************************************/
uint8_t MCP4725_isConnected(MCP4725 *_MCP4725)
{
  return HAL_I2C_IsDeviceReady(_MCP4725->hi2c, _MCP4725->_i2cAddress, 2, 100) == HAL_OK;
}

/**************************************************************************/
/*
    setReferenceVoltage()

    Set reference voltage
*/
/**************************************************************************/
void MCP4725_setReferenceVoltage(MCP4725 *_MCP4725, float value)
{
  if (value == 0)
    _MCP4725->_refVoltage = MCP4725_REFERENCE_VOLTAGE; // sanity check, avoid division by zero
  else
    _MCP4725->_refVoltage = value;

  _MCP4725->_bitsPerVolt = (float)MCP4725_STEPS / _MCP4725->_refVoltage; // TODO: check accuracy with +0.5
}

/**************************************************************************/
/*
    getReferenceVoltage()

    Return reference voltage
*/
/**************************************************************************/
float MCP4725_getReferenceVoltage(MCP4725 *_MCP4725)
{
  return _MCP4725->_refVoltage;
}

/**************************************************************************/
/*
    setValue()

    Set output voltage to a fraction of Vref

    NOTE:
    -  mode:
      - "MCP4725_FAST_MODE"...........writes 2-bytes, data & power type to
                                      DAC register & EEPROM is not affected
      - "MCP4725_REGISTER_MODE".......writes 3-bytes, data & power type to
                                      DAC register & EEPROM is not affected
      - "MCP4725_EEPROM_MODE".........writes 3-bytes, data & power type to
                                      DAC register & EEPROM
    - powerType:
      - "MCP4725_POWER_DOWN_OFF"......power down off, draws 0.40mA no load
                                      & 0.29mA maximum load
      - "MCP4725_POWER_DOWN_1KOHM"....power down on with 1 kOhm to ground,
                                      draws 60nA
      - "MCP4725_POWER_DOWN_100KOHM"..power down on with 100 kOhm to ground
      - "MCP4725_POWER_DOWN_500KOHM"..power down on with 500kOhm to ground
*/
/**************************************************************************/
uint8_t MCP4725_setValue(MCP4725 *_MCP4725, uint16_t value, MCP4725_COMMAND_TYPE mode, MCP4725_POWER_DOWN_TYPE powerType)
{
#ifndef MCP4725_DISABLE_SANITY_CHECK
  if (value > MCP4725_MAX_VALUE)
    value = MCP4725_MAX_VALUE; // make sure value never exceeds threshold
#endif

  return MCP4725_writeComand(_MCP4725, value, mode, powerType);
}

/**************************************************************************/
/*
    setVoltage()

    Set output voltage to a fraction of Vref
*/
/**************************************************************************/
uint8_t MCP4725_setVoltage(MCP4725 *_MCP4725, float voltage, MCP4725_COMMAND_TYPE mode, MCP4725_POWER_DOWN_TYPE powerType)
{
  uint16_t value = 0;

/* convert voltage to DAC bits */
#ifndef MCP4725_DISABLE_SANITY_CHECK
  if (voltage >= _MCP4725->_refVoltage)
    value = MCP4725_MAX_VALUE; // make sure value never exceeds threshold
  else if (voltage <= 0)
    value = 0;
  else
    value = voltage * _MCP4725->_bitsPerVolt; // xx,xx,xx,xx,D11,D10,D9,D8 ,D7,D6,D4,D3,D2,D9,D1,D0
#else
  value = voltage * _MCP4725->_bitsPerVolt; // xx,xx,xx,xx,D11,D10,D9,D8 ,D7,D6,D4,D3,D2,D9,D1,D0
#endif

  return MCP4725_writeComand(_MCP4725, value, mode, powerType);
}

/**************************************************************************/
/*
    getValue()

    Read current DAC value from DAC register

    NOTE:
    - see MCP4725 datasheet on p.20
*/
/**************************************************************************/
uint16_t MCP4725_getValue(MCP4725 *_MCP4725)
{
  uint16_t value = MCP4725_readRegister(_MCP4725, MCP4725_READ_DAC_REG); // D11,D10,D9,D8,D7,D6,D5,D4, D3,D2,D1,D0,xx,xx,xx,xx

  if (value != MCP4725_ERROR)
    return value >> 4; // 00,00,00,00,D11,D10,D9,D8,  D7,D6,D5,D4,D3,D2,D1,D0
  return value;        // collision on i2c bus
}

/**************************************************************************/
/*
    getVoltage()

    Read current DAC value from DAC register & convert to voltage
*/
/**************************************************************************/
float MCP4725_getVoltage(MCP4725 *_MCP4725)
{
  float value = MCP4725_getValue(_MCP4725);

  if (value != MCP4725_ERROR)
    return value / _MCP4725->_bitsPerVolt;
  return value;
}

/**************************************************************************/
/*
    getStoredValue()

    Read DAC value from EEPROM

    NOTE:
    - see MCP4725 datasheet on p.20
*/
/**************************************************************************/
uint16_t MCP4725_getStoredValue(MCP4725 *_MCP4725)
{
  uint16_t value = MCP4725_readRegister(_MCP4725, MCP4725_READ_EEPROM); // xx,PD1,PD0,xx,D11,D10,D9,D8, D7,D6,D5,D4,D3,D2,D1,D0

  if (value != MCP4725_ERROR)
    return value & 0x0FFF; // 00,00,00,00,D11,D10,D9,D8,   D7,D6,D5,D4,D3,D2,D1,D0
  return value;            // collision on i2c bus
}

/**************************************************************************/
/*
    getStoredVoltage()

    Read stored DAC value from EEPROM & convert to voltage
*/
/**************************************************************************/
float MCP4725_getStoredVoltage(MCP4725 *_MCP4725)
{
  float value = MCP4725_getStoredValue(_MCP4725);

  if (value != MCP4725_ERROR)
    return value / _MCP4725->_bitsPerVolt;
  return value;
}

/**************************************************************************/
/*
    getPowerType()

    Return current power type from DAC register

    NOTE:
    - "MCP4725_POWER_DOWN_OFF"
      PD1 PD0
      0,  0
    - "MCP4725_POWER_DOWN_1KOHM"
      PD1 PD0
      0,  1
    - "MCP4725_POWER_DOWN_100KOHM"
      1,  0
    - "MCP4725_POWER_DOWN_500KOHM"
      1,  1
    - in the power-down modes Vout is off
    - see MCP4725 datasheet on p.15
    - see MCP4725 datasheet on p.20
*/
/**************************************************************************/
uint16_t MCP4725_getPowerType(MCP4725 *_MCP4725)
{
  uint16_t value = MCP4725_readRegister(_MCP4725, MCP4725_READ_SETTINGS); // BSY,POR,xx,xx,xx,PD1,PD0,xx

  if (value != MCP4725_ERROR)
  {
    value &= 0x0006;   // 00,00,00,00,00,PD1,PD0,00
    return value >> 1; // 00,00,00,00,00,00,PD1,PD0
  }

  return value; // collision on i2c bus
}

/**************************************************************************/
/*
    getStoredPowerType()

    Return stored power type from EEPROM

    NOTE:
    - "MCP4725_POWER_DOWN_OFF"
      PD1 PD0
      0,  0
    - "MCP4725_POWER_DOWN_1KOHM"
      PD1 PD0
      0,  1
    - "MCP4725_POWER_DOWN_100KOHM"
      1,  0
    - "MCP4725_POWER_DOWN_500KOHM"
      1,  1
    - see MCP4725 datasheet on p.20
*/
/**************************************************************************/
uint16_t MCP4725_getStoredPowerType(MCP4725 *_MCP4725)
{
  uint16_t value = MCP4725_readRegister(_MCP4725, MCP4725_READ_EEPROM); // xx,PD1,PD0,xx,D11,D10,D9,D8,  D7,D6,D5,D4,D3,D2,D1,D0

  if (value != MCP4725_ERROR)
  {
    value = value << 1; // PD1,PD0,xx,D11,D10,D9,D8,D7  D6,D5,D4,D3,D2,D1,D0,00
    return value >> 14; // 00,00,00,00,00,00,00,00      00,00,00,00,00,00,PD1,PD0
  }

  return value; // collision on i2c bus
}

/**************************************************************************/
/*
    reset()

    Reset MCP4725 & upload data from EEPROM to DAC register

    NOTE:
    - use with caution, "general call" command may affect all slaves
      on i2c bus
    - if Vdd < 2v all circuits & output disabled, when the Vdd
      increases above Vpor device takes a reset state
*/
/**************************************************************************/
void MCP4725_reset(MCP4725 *_MCP4725)
{
  // Wire.beginTransmission(MCP4725_GENERAL_CALL_ADDRESS);
  // Wire.send(MCP4725_GENERAL_CALL_RESET);
  // Wire.endTransmission(true);

  uint8_t buffer[1] = {MCP4725_GENERAL_CALL_RESET};
  HAL_I2C_Master_Transmit(_MCP4725->hi2c, MCP4725_GENERAL_CALL_ADDRESS, buffer, 1, 1000);
}

/**************************************************************************/
/*
    wakeUP()

    Wake up & upload value from DAC register

    NOTE:
    - use with caution, "general call" command may affect all slaves
      on i2c bus
    - resets current power-down bits, EEPROM power-down bit are
      not affected
*/
/**************************************************************************/
void MCP4725_wakeUP(MCP4725 *_MCP4725)
{
  // Wire.beginTransmission(MCP4725_GENERAL_CALL_ADDRESS);
  // Wire.send(MCP4725_GENERAL_WAKE_UP);
  // Wire.endTransmission(true);

  uint8_t buffer[1] = {MCP4725_GENERAL_WAKE_UP};
  HAL_I2C_Master_Transmit(_MCP4725->hi2c, MCP4725_GENERAL_CALL_ADDRESS, buffer, 1, 1000);
}

/**************************************************************************/
/*
    getEepromBusyFlag()

    Return EEPROM writing status from DAC register

    NOTE:
    - any new write command including repeat bytes during EEPROM write mode
      is ignored
    - see MCP4725 datasheet on p.20
*/
/**************************************************************************/
uint8_t MCP4725_getEepromBusyFlag(MCP4725 *_MCP4725)
{
  uint16_t value = MCP4725_readRegister(_MCP4725, MCP4725_READ_SETTINGS); // BSY,POR,xx,xx,xx,PD1,PD0,xx

  if (value != MCP4725_ERROR)
    return (value & 0x80) == 0x80; // 1 - completed, 0 - incompleted
  return 0;                        // collision on i2c bus
}

/**************************************************************************/
/*
    writeComand()

    Writes value to DAC register or EEPROM

    NOTE:
    - "MCP4725_FAST_MODE" bit format:
      15    14    13   12   11   10   9   8   7   6   5   4   3   2   1   0-bit
      C2=0, C1=0, PD1, PD0, D11, D10, D9, D8, D7, D6, D5, D4, D3, D2, D1, D0
    - "MCP4725_REGISTER_MODE" bit format:
      23    22    21    20   19   18   17   16  15   14   13  12  11  10  9   8   7   6    5   4  3   2   1   0-bit
      C2=0, C1=1, C0=0, xx,  xx,  PD1, PD0, xx, D11, D10, D9, D8, D7, D6, D5, D4, D3, D2, D1, D0, xx, xx, xx, xx
    - "MCP4725_EEPROM_MODE" bit format:
      23    22    21    20   19   18   17   16  15   14   13  12  11  10  9   8   7   6    5   4  3   2   1   0-bit
      C2=0, C1=1, C0=1, xx,  xx,  PD1, PD0, xx, D11, D10, D9, D8, D7, D6, D5, D4, D3, D2, D1, D0, xx, xx, xx, xx

    - "MCP4725_POWER_DOWN_OFF"
      PD1 PD0
      0,  0
    - "MCP4725_POWER_DOWN_1KOHM"
      PD1 PD0
      0,  1
    - "MCP4725_POWER_DOWN_100KOHM"
      1,  0
    - "MCP4725_POWER_DOWN_500KOHM"
      1,  1
*/
/**************************************************************************/
uint8_t MCP4725_writeComand(MCP4725 *_MCP4725, uint16_t value, MCP4725_COMMAND_TYPE mode, MCP4725_POWER_DOWN_TYPE powerType)
{
  uint8_t buffer[3];
  HAL_StatusTypeDef I2C_Stat;
  // Wire.beginTransmission(_i2cAddress);

  switch (mode)
  {
  case MCP4725_FAST_MODE: // see MCP4725 datasheet on p.18

    // Wire.send(mode | (powerType << 4)  | highByte(value));
    // Wire.send(lowByte(value));

    buffer[0] = mode | (powerType << 4) | highByte(value);
    buffer[1] = lowByte(value);

    I2C_Stat = HAL_I2C_Master_Transmit(_MCP4725->hi2c, _MCP4725->_i2cAddress, buffer, 2, 1000);
    //			I2C_Stat = HAL_I2C_Master_Transmit_DMA(_MCP4725->hi2c, _MCP4725->_i2cAddress, buffer, 2);

    break;

  case MCP4725_REGISTER_MODE:
  case MCP4725_EEPROM_MODE: // see MCP4725 datasheet on p.19
    value = value << 4;     // D11,D10,D9,D8,D7,D6,D5,D4,  D3,D2,D1,D0,xx,xx,xx,xx
    // Wire.send(mode  | (powerType << 1));
    // Wire.send(highByte(value));
    // Wire.send(lowByte(value));

    buffer[0] = mode | (powerType << 1);
    buffer[1] = highByte(value);
    buffer[2] = lowByte(value);

    I2C_Stat = HAL_I2C_Master_Transmit(_MCP4725->hi2c, _MCP4725->_i2cAddress, buffer, 3, 1000);

    break;
  }

  if (I2C_Stat != HAL_OK)
    return 0; // send data over i2c & check for collision on i2c bus

  if (mode == MCP4725_EEPROM_MODE)
  {
    if (MCP4725_getEepromBusyFlag(_MCP4725) == 1)
      return 1;                           // write completed, success!!!
    HAL_Delay(MCP4725_EEPROM_WRITE_TIME); // typical EEPROM write time 25 msec
    if (MCP4725_getEepromBusyFlag(_MCP4725) == 1)
      return 1;                           // write completed, success!!!
    HAL_Delay(MCP4725_EEPROM_WRITE_TIME); // maximum EEPROM write time 25 + 25 = 50 msec
  }

  return 1; // success!!!
}

/**************************************************************************/
/*
    readRegister()

    Read DAC register via i2c bus

    NOTE:
    - read output bit format:
      39  38  37 36 35 34  33  32  31  30  29 28 27 26 25 24  23 22 21 20 19 18 17 16  15 14  13  12 11  10  9  8   7  6  5  4  3  2  1  0-bit
      BSY,POR,xx,xx,xx,PD1,PD0,xx, D11,D10,D9,D8,D7,D6,D5,D4, D3,D2,D1,D0,xx,xx,xx,xx, xx,PD1,PD0,xx,D11,D10,D9,D8, D7,D6,D5,D4,D3,D2,D1,D0
      ------ Settings data ------  ---------------- DAC register data ---------------  ------------------- EEPROM data --------------------
    - see MCP4725 datasheet on p.20
*/
/**************************************************************************/
uint16_t MCP4725_readRegister(MCP4725 *_MCP4725, MCP4725_READ_TYPE dataType)
{
  uint16_t value = dataType; // convert enum to integer to avoid compiler warnings
  uint16_t ret_val = 0;
  uint8_t buffer[dataType];
  HAL_StatusTypeDef I2C_Stat;

  I2C_Stat = HAL_I2C_Master_Receive(_MCP4725->hi2c, _MCP4725->_i2cAddress, buffer, dataType, 1000);

  if (I2C_Stat != HAL_OK)
    return MCP4725_ERROR;

  /* read data from buffer */
  switch (dataType)
  {
  case MCP4725_READ_SETTINGS:
    ret_val = buffer[0];

    break;

  case MCP4725_READ_DAC_REG:
  case MCP4725_READ_EEPROM:

    ret_val = buffer[value - 2];
    ret_val = (ret_val << 8) | buffer[value - 1];
    break;
  }

  return ret_val;
}

const uint16_t sineTable[128] =
    {
        2048, 2148, 2248, 2348, 2447, 2545, 2642, 2737, // sin middle
        2831, 2923, 3013, 3100, 3185, 3267, 3346, 3423,
        3495, 3565, 3630, 3692, 3750, 3804, 3853, 3898,
        3939, 3975, 4007, 4034, 4056, 4073, 4085, 4093,
        4095, 4093, 4085, 4073, 4056, 4034, 4007, 3975, // sin upper peak
        3939, 3898, 3853, 3804, 3750, 3692, 3630, 3565,
        3495, 3423, 3346, 3267, 3185, 3100, 3013, 2923,
        2831, 2737, 2642, 2545, 2447, 2348, 2248, 2148,
        2048, 1947, 1847, 1747, 1648, 1550, 1453, 1358, // sin middle
        1264, 1172, 1082, 995, 910, 828, 749, 672,
        600, 530, 465, 403, 345, 291, 242, 197,
        156, 120, 88, 61, 39, 22, 10, 2,
        0, 2, 10, 22, 39, 61, 88, 120, // sin lower peak
        156, 197, 242, 291, 345, 403, 465, 530,
        600, 672, 749, 828, 910, 995, 1082, 1172,
        1264, 1358, 1453, 1550, 1648, 1747, 1847, 1947 // sin middle
};

const uint16_t sineTable_512[512] = // PROGMEM saves variable to flash & keeps dynamic memory free
    {
        2048, 2073, 2098, 2123, 2148, 2174, 2199, 2224, // sin middle, 2048
        2249, 2274, 2299, 2324, 2349, 2373, 2398, 2423,
        2448, 2472, 2497, 2521, 2546, 2570, 2594, 2618,
        2643, 2667, 2690, 2714, 2738, 2762, 2785, 2808,
        2832, 2855, 2878, 2901, 2924, 2946, 2969, 2991,
        3013, 3036, 3057, 3079, 3101, 3122, 3144, 3165,
        3186, 3207, 3227, 3248, 3268, 3288, 3308, 3328,
        3347, 3367, 3386, 3405, 3423, 3442, 3460, 3478,
        3496, 3514, 3531, 3548, 3565, 3582, 3599, 3615,
        3631, 3647, 3663, 3678, 3693, 3708, 3722, 3737,
        3751, 3765, 3778, 3792, 3805, 3817, 3830, 3842,
        3854, 3866, 3877, 3888, 3899, 3910, 3920, 3930,
        3940, 3950, 3959, 3968, 3976, 3985, 3993, 4000,
        4008, 4015, 4022, 4028, 4035, 4041, 4046, 4052,
        4057, 4061, 4066, 4070, 4074, 4077, 4081, 4084,
        4086, 4088, 4090, 4092, 4094, 4095, 4095, 4095, // sin upper peak, 4095
        4095, 4095, 4095, 4095, 4094, 4092, 4090, 4088,
        4086, 4084, 4081, 4077, 4074, 4070, 4066, 4061,
        4057, 4052, 4046, 4041, 4035, 4028, 4022, 4015,
        4008, 4000, 3993, 3985, 3976, 3968, 3959, 3950,
        3940, 3930, 3920, 3910, 3899, 3888, 3877, 3866,
        3854, 3842, 3830, 3817, 3805, 3792, 3778, 3765,
        3751, 3737, 3722, 3708, 3693, 3678, 3663, 3647,
        3631, 3615, 3599, 3582, 3565, 3548, 3531, 3514,
        3496, 3478, 3460, 3442, 3423, 3405, 3386, 3367,
        3347, 3328, 3308, 3288, 3268, 3248, 3227, 3207,
        3186, 3165, 3144, 3122, 3101, 3079, 3057, 3036,
        3013, 2991, 2969, 2946, 2924, 2901, 2878, 2855,
        2832, 2808, 2785, 2762, 2738, 2714, 2690, 2667,
        2643, 2618, 2594, 2570, 2546, 2521, 2497, 2472,
        2448, 2423, 2398, 2373, 2349, 2324, 2299, 2274,
        2249, 2224, 2199, 2174, 2148, 2123, 2098, 2073,
        2048, 2023, 1998, 1973, 1948, 1922, 1897, 1872, // sin middle, 2048
        1847, 1822, 1797, 1772, 1747, 1723, 1698, 1673,
        1648, 1624, 1599, 1575, 1550, 1526, 1502, 1478,
        1453, 1429, 1406, 1382, 1358, 1334, 1311, 1288,
        1264, 1241, 1218, 1195, 1172, 1150, 1127, 1105,
        1083, 1060, 1039, 1017, 995, 974, 952, 931,
        910, 889, 869, 848, 828, 808, 788, 768,
        749, 729, 710, 691, 673, 654, 636, 618,
        600, 582, 565, 548, 531, 514, 497, 481,
        465, 449, 433, 418, 403, 388, 374, 359,
        345, 331, 318, 304, 291, 279, 266, 254,
        242, 230, 219, 208, 197, 186, 176, 166,
        156, 146, 137, 128, 120, 111, 103, 96,
        88, 81, 74, 68, 61, 55, 50, 44,
        39, 35, 30, 26, 22, 19, 15, 12,
        10, 8, 6, 4, 2, 1, 1, 0, // sin lower peak, 0
        0, 0, 1, 1, 2, 4, 6, 8,
        10, 12, 15, 19, 22, 26, 30, 35,
        39, 44, 50, 55, 61, 68, 74, 81,
        88, 96, 103, 111, 120, 128, 137, 146,
        156, 166, 176, 186, 197, 208, 219, 230,
        242, 254, 266, 279, 291, 304, 318, 331,
        345, 359, 374, 388, 403, 418, 433, 449,
        465, 481, 497, 514, 531, 548, 565, 582,
        600, 618, 636, 654, 673, 691, 710, 729,
        749, 768, 788, 808, 828, 848, 869, 889,
        910, 931, 952, 974, 995, 1017, 1039, 1060,
        1083, 1105, 1127, 1150, 1172, 1195, 1218, 1241,
        1264, 1288, 1311, 1334, 1358, 1382, 1406, 1429,
        1453, 1478, 1502, 1526, 1550, 1575, 1599, 1624,
        1648, 1673, 1698, 1723, 1747, 1772, 1797, 1822,
        1847, 1872, 1897, 1922, 1948, 1973, 1998, 2023 // sin middle
};

uint16_t sineTable_400[400] = {2048, 2176, 2304, 2431, 2557, 2680, 2801, 2919, 3034, 3144, 3251, 3352, 3449, 3540, 3625, 3704, 3776, 3841, 3900, 3951, 3994, 4030, 4058, 4078, 4090, 4095, 4090, 4078, 4058, 4030, 3994, 3951, 3900, 3841, 3776, 3704, 3625, 3540, 3449, 3352, 3251, 3144, 3034, 2919, 2801, 2680, 2557, 2431, 2304, 2176, 2048, 1919, 1791, 1664, 1538, 1415, 1294, 1176, 1061, 951, 844, 743, 646, 555, 470, 391, 319, 254, 195, 144, 101, 65, 37, 17, 5, 1, 5, 17, 37, 65, 101, 144, 195, 254, 319, 391, 470, 555, 646, 743, 844, 951, 1061, 1176, 1294, 1415, 1538, 1664, 1791, 1919, 2047, 2176, 2304, 2431, 2557, 2680, 2801, 2919, 3034, 3144, 3251, 3352, 3449, 3540, 3625, 3704, 3776, 3841, 3900, 3951, 3994, 4030, 4058, 4078, 4090, 4095, 4090, 4078, 4058, 4030, 3994, 3951, 3900, 3841, 3776, 3704, 3625, 3540, 3449, 3352, 3251, 3144, 3034, 2919, 2801, 2680, 2557, 2431, 2304, 2176, 2048, 1919, 1791, 1664, 1538, 1415, 1294, 1176, 1061, 951, 844, 743, 646, 555, 470, 391, 319, 254, 195, 144, 101, 65, 37, 17, 5, 1, 5, 17, 37, 65, 101, 144, 195, 254, 319, 391, 470, 555, 646, 743, 844, 951, 1061, 1176, 1294, 1415, 1538, 1664, 1791, 1919, 2047, 2176, 2304, 2431, 2557, 2680, 2801, 2919, 3034, 3144, 3251, 3352, 3449, 3540, 3625, 3704, 3776, 3841, 3900, 3951, 3994, 4030, 4058, 4078, 4090, 4095, 4090, 4078, 4058, 4030, 3994, 3951, 3900, 3841, 3776, 3704, 3625, 3540, 3449, 3352, 3251, 3144, 3034, 2919, 2801, 2680, 2557, 2431, 2304, 2176, 2048, 1919, 1791, 1664, 1538, 1415, 1294, 1176, 1061, 951, 844, 743, 646, 555, 470, 391, 319, 254, 195, 144, 101, 65, 37, 17, 5, 1, 5, 17, 37, 65, 101, 144, 195, 254, 319, 391, 470, 555, 646, 743, 844, 951, 1061, 1176, 1294, 1415, 1538, 1664, 1791, 1919, 2047, 2176, 2304, 2431, 2557, 2680, 2801, 2919, 3034, 3144, 3251, 3352, 3449, 3540, 3625, 3704, 3776, 3841, 3900, 3951, 3994, 4030, 4058, 4078, 4090, 4095, 4090, 4078, 4058, 4030, 3994, 3951, 3900, 3841, 3776, 3704, 3625, 3540, 3449, 3352, 3251, 3144, 3034, 2919, 2801, 2680, 2557, 2431, 2304, 2176, 2048, 1919, 1791, 1664, 1538, 1415, 1294, 1176, 1061, 951, 844, 743, 646, 555, 470, 391, 319, 254, 195, 144, 101, 65, 37, 17, 5, 1, 5, 17, 37, 65, 101, 144, 195, 254, 319, 391, 470, 555, 646, 743, 844, 951, 1061, 1176, 1294, 1415, 1538, 1664, 1791, 1919};
uint16_t sineTable_800[800] = {2048, 2073, 2098, 2123, 2148, 2173, 2198, 2223, 2248, 2273, 2298, 2323, 2348, 2373, 2397, 2422, 2447, 2471, 2496, 2520, 2545, 2569, 2593, 2618, 2642, 2666, 2690, 2713, 2737, 2761, 2784, 2808, 2831, 2854, 2877, 2900, 2923, 2945, 2968, 2990, 3012, 3035, 3056, 3078, 3100, 3121, 3143, 3164, 3185, 3206, 3226, 3247, 3267, 3287, 3307, 3327, 3346, 3365, 3385, 3403, 3422, 3441, 3459, 3477, 3495, 3513, 3530, 3547, 3564, 3581, 3598, 3614, 3630, 3646, 3661, 3677, 3692, 3707, 3721, 3735, 3750, 3763, 3777, 3790, 3803, 3816, 3829, 3841, 3853, 3864, 3876, 3887, 3898, 3909, 3919, 3929, 3939, 3948, 3957, 3966, 3975, 3983, 3991, 3999, 4006, 4014, 4020, 4027, 4033, 4039, 4045, 4050, 4055, 4060, 4064, 4069, 4072, 4076, 4079, 4082, 4085, 4087, 4089, 4091, 4092, 4093, 4094, 4094, 4095, 4094, 4094, 4093, 4092, 4091, 4089, 4087, 4085, 4082, 4079, 4076, 4072, 4069, 4064, 4060, 4055, 4050, 4045, 4039, 4033, 4027, 4020, 4014, 4006, 3999, 3991, 3983, 3975, 3966, 3957, 3948, 3939, 3929, 3919, 3909, 3898, 3887, 3876, 3864, 3853, 3841, 3829, 3816, 3803, 3790, 3777, 3763, 3750, 3735, 3721, 3707, 3692, 3677, 3661, 3646, 3630, 3614, 3598, 3581, 3564, 3547, 3530, 3513, 3495, 3477, 3459, 3441, 3422, 3403, 3385, 3365, 3346, 3327, 3307, 3287, 3267, 3247, 3226, 3206, 3185, 3164, 3143, 3121, 3100, 3078, 3056, 3035, 3012, 2990, 2968, 2945, 2923, 2900, 2877, 2854, 2831, 2808, 2784, 2761, 2737, 2713, 2690, 2666, 2642, 2618, 2593, 2569, 2545, 2520, 2496, 2471, 2447, 2422, 2397, 2373, 2348, 2323, 2298, 2273, 2248, 2223, 2198, 2173, 2148, 2123, 2098, 2073, 2048, 2022, 1997, 1972, 1947, 1922, 1897, 1872, 1847, 1822, 1797, 1772, 1747, 1722, 1698, 1673, 1648, 1624, 1599, 1575, 1550, 1526, 1502, 1477, 1453, 1429, 1405, 1382, 1358, 1334, 1311, 1287, 1264, 1241, 1218, 1195, 1172, 1150, 1127, 1105, 1083, 1060, 1039, 1017, 995, 974, 952, 931, 910, 889, 869, 848, 828, 808, 788, 768, 749, 730, 710, 692, 673, 654, 636, 618, 600, 582, 565, 548, 531, 514, 497, 481, 465, 449, 434, 418, 403, 388, 374, 360, 345, 332, 318, 305, 292, 279, 266, 254, 242, 231, 219, 208, 197, 186, 176, 166, 156, 147, 138, 129, 120, 112, 104, 96, 89, 81, 75, 68, 62, 56, 50, 45, 40, 35, 31, 26, 23, 19, 16, 13, 10, 8, 6, 4, 3, 2, 1, 1, 1, 1, 1, 2, 3, 4, 6, 8, 10, 13, 16, 19, 23, 26, 31, 35, 40, 45, 50, 56, 62, 68, 75, 81, 89, 96, 104, 112, 120, 129, 138, 147, 156, 166, 176, 186, 197, 208, 219, 231, 242, 254, 266, 279, 292, 305, 318, 332, 345, 360, 374, 388, 403, 418, 434, 449, 465, 481, 497, 514, 531, 548, 565, 582, 600, 618, 636, 654, 673, 692, 710, 730, 749, 768, 788, 808, 828, 848, 869, 889, 910, 931, 952, 974, 995, 1017, 1039, 1060, 1083, 1105, 1127, 1150, 1172, 1195, 1218, 1241, 1264, 1287, 1311, 1334, 1358, 1382, 1405, 1429, 1453, 1477, 1502, 1526, 1550, 1575, 1599, 1624, 1648, 1673, 1698, 1722, 1747, 1772, 1797, 1822, 1847, 1872, 1897, 1922, 1947, 1972, 1997, 2022};
