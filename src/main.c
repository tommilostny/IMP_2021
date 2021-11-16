#include <driver/i2c.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#define VL53L0X_ADDRESS 0x29
#define VL53L0X_REG_SYSRANGE_START 0x00
#define VL53L0X_REG_SYSRANGE_MODE 0x01

void app_main()
{
    //setup esp32 i2c driver for VL53L0X
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
        .clk_flags = 0,
    };
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

    while(true)
    {
        //read data from VL53L0X and print it
        uint8_t data[2];
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (VL53L0X_ADDRESS << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
        i2c_master_write_byte(cmd, VL53L0X_REG_SYSRANGE_START, I2C_MASTER_ACK);
        i2c_master_write_byte(cmd, VL53L0X_REG_SYSRANGE_MODE, I2C_MASTER_ACK);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (VL53L0X_ADDRESS << 1) | I2C_MASTER_READ, I2C_MASTER_ACK);
        i2c_master_read_byte(cmd, data, I2C_MASTER_ACK);
        i2c_master_read_byte(cmd, data + 1, I2C_MASTER_NACK);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        printf("%u\n", data[0] << 8 | data[1]);
        
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
