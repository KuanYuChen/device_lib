#include "ssd1306.h"

void ssd1306_command(ssd1306_device *device, uint8_t command);

void ssd1306_command(ssd1306_device *device, uint8_t command) {
  device->tx[0] = 0x00; // command
  device->tx[1] = command;

  halt_on_error(
      i2c_master_write(device->dev.i2c, device->dev.address, device->tx, 2));
}

// Perform initialization
// Taken from: https://github.com/adafruit/Adafruit_SSD1306/
error_t ssd1306_init(ssd1306_device *device, uint32_t i2c) {

  if (!device) {
    return E_NULL_PTR;
  }

  device->dev.i2c = i2c;
  device->dev.address = SSD1306_ADDRESS;
  device->width = 128;
  device->height = 64;
  device->buffer = malloc(device->width * device->height / 8);

  device->tx[0] = 0x00;
  device->tx[1] = SSD1306_DISPLAY_OFF;
  device->tx[2] = SSD1306_SET_DISPLAY_CLOCK_DIVIDE;
  device->tx[3] = 0x80;
  device->tx[4] = SSD1306_SET_MULTIPLEX_RATIO;
  device->tx[5] = 64 - 1;
  halt_on_error(
      i2c_master_write(device->dev.i2c, device->dev.address, device->tx, 6));

  device->tx[1] = SSD1306_SET_DISPLAY_OFFSET;
  device->tx[2] = 0x00;
  device->tx[3] = SSD1306_SET_START_LINE | 0x00;
  device->tx[4] = SSD1306_CHARGE_PUMP;
  device->tx[5] = 0x14;

  halt_on_error(
      i2c_master_write(device->dev.i2c, device->dev.address, device->tx, 6));

  device->tx[1] = 0x20;
  device->tx[2] = 0x00;
  device->tx[3] = 0xA0 | 0x1;
  device->tx[4] = 0xC8;

  halt_on_error(
      i2c_master_write(device->dev.i2c, device->dev.address, device->tx, 5));

  device->tx[1] = 0xDA;
  device->tx[2] = 0x12;
  device->tx[3] = 0x81;
  device->tx[4] = 0xCF;

  halt_on_error(
      i2c_master_write(device->dev.i2c, device->dev.address, device->tx, 5));

  device->tx[1] = 0xD9;
  device->tx[2] = 0xF1;
  device->tx[3] = 0xDB;
  device->tx[4] = 0x40;
  device->tx[5] = 0xA4;
  device->tx[6] = 0xA6;
  device->tx[7] = 0x2E;
  device->tx[8] = 0xAF;
  device->tx[9] = 0xA4;

  halt_on_error(
      i2c_master_write(device->dev.i2c, device->dev.address, device->tx, 10));

  return E_SUCCESS;
}

// print buffer
error_t ssd1306_print_buffer(ssd1306_device *device) {
  device->tx[0] = 0x00;
  device->tx[1] = 0x22;
  device->tx[2] = 0x00;
  device->tx[3] = 0xFF;
  device->tx[4] = 0x21;
  device->tx[5] = 0x00;
  device->tx[6] = 128 - 1;

  check_error(
      i2c_master_write(device->dev.i2c, device->dev.address, device->tx, 7));

  uint16_t bytes_sent = 0;
  device->tx[0] = 0x40;
  while (bytes_sent < (device->width * device->height / 8)) {
    memcpy(&device->tx[1], &device->buffer[0 + bytes_sent], 32);
    check_error(i2c_master_write(device->dev.i2c, device->dev.address,
                                 device->tx, sizeof(device->tx)));
    bytes_sent += 32;
  }
  return E_SUCCESS;
}
