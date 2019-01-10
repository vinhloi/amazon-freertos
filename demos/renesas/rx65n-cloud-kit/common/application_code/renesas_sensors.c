#include <stdio.h>
#include <string.h>

#include "renesas_sensors.h"
//#include "r_smc_entry.h"
#include "platform.h"
#include "r_riic_rx_if.h"

#include "FreeRTOS.h"

#define ISL29035_ADDRESS    (0x44)
#define BME680_ADDRESS      (0x76)
#define BME160_ADDRESS	     (0x68)

#define I2C_WRITE_MODE      (0x00)
#define I2C_READ_MODE       (0x01)

void swap_byte_order(uint8_t *byte);

void callback_master(void) {

}

static void acknowledge_polling(riic_info_t *iic_info_m) {

    riic_return_t ret;

    /* Set arguments for R_RIIC_MasterSend. */
    iic_info_m->p_data1st = (uint8_t *) FIT_NO_PTR; /* Pointer to the first data storage buffer */
    iic_info_m->cnt1st = 0; /* First data counter (number of bytes) */
    iic_info_m->p_data2nd = (uint8_t *) FIT_NO_PTR; /* Pointer to the second data storage buffer */
    iic_info_m->cnt2nd = 0; /* Second data counter (number of bytes) */

    do {
        /* Master send start. */

        ret = R_RIIC_MasterSend(iic_info_m);

        if (RIIC_SUCCESS == ret) {
            /* Waitting for R_RIIC_MasterSend completed. */
            while (RIIC_COMMUNICATION == iic_info_m->dev_sts) {
                /* do nothing */
            }
            /* Slave returns NACK. Set retry interval. */
            if (RIIC_NACK == iic_info_m->dev_sts) {
                /* Waitting for retry interval 100us. */
                R_BSP_SoftwareDelay(100, BSP_DELAY_MICROSECS);
            }
        } else {
            /* This software is for single master.
             Therefore, return value should be always 'RIIC_SUCCESS'. */
            while (1) {
                nop(); /* error */
            }
        }
    } while (RIIC_FINISH != iic_info_m->dev_sts);

} /* End of function acknowledge_polling() */

riic_return_t sync_iic_master_send(riic_info_t *iic_info_m,
        uint8_t *write_buffer, uint32_t size) {

    riic_return_t ret;
    riic_mcu_status_t iic_status_m;
    uint32_t i = 0;

    iic_info_m->cnt2nd = size;
    iic_info_m->cnt1st = 0;
    iic_info_m->p_data2nd = (uint8_t *)write_buffer;
    iic_info_m->p_data1st = (uint8_t *)FIT_NO_PTR;

    ret = R_RIIC_MasterSend(iic_info_m);

    if (RIIC_SUCCESS == ret) {
        /* Waitting for R_RIIC_MasterSend completed. */
        while (RIIC_COMMUNICATION == iic_info_m->dev_sts) {
            /* do nothing */
        }
        if (RIIC_NACK == iic_info_m->dev_sts) {
            /* Slave returns NACK. The slave address may not correct.
             Please check the macro definition value or hardware connection etc. */
            while (1) {
                nop(); /* error */
            }
        }
    } else {
        /* This software is for single master.
         Therefore, return value should be always 'RIIC_SUCCESS'. */
        while (1) {
            nop(); /* error */
        }
    }

    return ret;

}

riic_return_t sync_iic_master_receive(riic_info_t *iic_info_m,
        uint8_t *read_buffer, uint32_t size) {

    riic_return_t ret;
    riic_mcu_status_t iic_status_m;
    uint32_t i = 0;

    iic_info_m->cnt2nd = size;
    iic_info_m->cnt1st = 0;
    iic_info_m->p_data2nd = (uint8_t *)read_buffer;
    iic_info_m->p_data1st = (uint8_t *)FIT_NO_PTR;

    ret = R_RIIC_MasterReceive(iic_info_m);

    if (RIIC_SUCCESS == ret) {
        /* Waitting for R_RIIC_MasterSend completed. */
        while (RIIC_COMMUNICATION == iic_info_m->dev_sts) {
            /* do nothing */
        }
        if (RIIC_NACK == iic_info_m->dev_sts) {
            /* Slave returns NACK. The slave address may not correct.
             Please check the macro definition value or hardware connection etc. */
            while (1) {
                nop(); /* error */
            }
        }
    } else {
        /* This software is for single master.
         Therefore, return value should be always 'RIIC_SUCCESS'. */
        while (1) {
            nop(); /* error */
        }
    }

    return ret;

}

uint16_t read_isl29035_light_sensor() {

    uint16_t lux_level;

    riic_info_t light_sensor_iic_info_m;
    riic_mcu_status_t light_sensor_iic_status_m;
    riic_return_t ret;
    uint8_t  light_sensor_addr[1] = {ISL29035_ADDRESS};

    uint8_t write_buffer[2];
    uint8_t read_buffer[2];

    memset(write_buffer, 0, sizeof(write_buffer));
    memset(read_buffer, 0, sizeof(read_buffer));

    light_sensor_iic_info_m.dev_sts = RIIC_NO_INIT;
    light_sensor_iic_info_m.ch_no = 0;
    light_sensor_iic_info_m.callbackfunc = &callback_master;
    light_sensor_iic_info_m.p_slv_adr = light_sensor_addr;

    ret = R_RIIC_Open(&light_sensor_iic_info_m);

    if (RIIC_SUCCESS != ret) {
        /* This software is for single master.
         Therefore, return value should be always 'RIIC_SUCCESS'. */
        while (1) {
            nop(); /* error */
        }
    }

    acknowledge_polling(&light_sensor_iic_info_m);

    /* configure the light sensor */

    write_buffer[0] = 0; /* register 0 - command 1 */
    write_buffer[1] = 0xa3; /* 0b1010 0011 measure ALS continuously, interrupts 16 integration cycles */

    ret = sync_iic_master_send(&light_sensor_iic_info_m, write_buffer, 2);

    acknowledge_polling(&light_sensor_iic_info_m);

    write_buffer[0] = 1; /* register 1 - command 2 */
    write_buffer[1] = 0x0; /* 0b0000 0000 - 16-bit ADC resolution, full scale resolution 1,000 Lux */

    ret = sync_iic_master_send(&light_sensor_iic_info_m, write_buffer, 2);

    acknowledge_polling(&light_sensor_iic_info_m);

//    LED0 = LED_ON;
//    LED1 = LED_OFF;

    /* flash the LEDs to show that we're running */
//    LED0 = !LED0;
//    LED1 = !LED0;

    /* read the light level */
    write_buffer[0] = 2; /* register 2 -> 3 - data */

    ret = sync_iic_master_send(&light_sensor_iic_info_m, write_buffer, 1);

    acknowledge_polling(&light_sensor_iic_info_m);

    ret = sync_iic_master_receive(&light_sensor_iic_info_m, read_buffer, 2);

    acknowledge_polling(&light_sensor_iic_info_m);

    ret = R_RIIC_Close(&light_sensor_iic_info_m);

    lux_level = read_buffer[1];
    lux_level <<= 8;
    lux_level |= read_buffer[0];

    return (lux_level);
}

uint32_t read_bme680_environment_sensor() {

    riic_info_t environment_sensor_iic_info_m;
    riic_mcu_status_t environment_sensor_iic_status_m;
    riic_return_t ret;
    uint8_t  environment_sensor_addr[1] = {BME680_ADDRESS};

    uint8_t write_buffer[3];
    uint8_t read_buffer[3];
    uint32_t temperature;
    uint32_t count = 0;

    memset(read_buffer, 0, sizeof(read_buffer));
    memset(write_buffer, 0, sizeof(write_buffer));

    environment_sensor_iic_info_m.dev_sts = RIIC_NO_INIT;
    environment_sensor_iic_info_m.ch_no = 0;
    environment_sensor_iic_info_m.callbackfunc = &callback_master;
    environment_sensor_iic_info_m.p_slv_adr = environment_sensor_addr;

    ret = R_RIIC_Open(&environment_sensor_iic_info_m);

    if (RIIC_SUCCESS != ret) {
        /* This software is for single master.
         Therefore, return value should be always 'RIIC_SUCCESS'. */
        while (1) {
            nop(); /* error */
        }
    }

    acknowledge_polling(&environment_sensor_iic_info_m);

    write_buffer[0] = 0xD0; /*Reset Device */
    //write_buffer[1] = 0xD0; /*Device ID register */

    ret = sync_iic_master_send(&environment_sensor_iic_info_m, write_buffer, 1);

    acknowledge_polling(&environment_sensor_iic_info_m);

    ret = sync_iic_master_receive(&environment_sensor_iic_info_m, read_buffer,
            1);

    acknowledge_polling(&environment_sensor_iic_info_m);

    /* configure the environment sensor per the "Quick start" in section 3.2.1 of the data sheet */
    write_buffer[0] = 0x72; /* osrs_h */
    write_buffer[1] = 0x01; /* humidity oversampling 1x 0b001 */

    ret = sync_iic_master_send(&environment_sensor_iic_info_m, write_buffer, 2);

    acknowledge_polling(&environment_sensor_iic_info_m);

    write_buffer[0] = 0x74; /* osrs_t , osrs_p */
    write_buffer[1] = 0x54; /* temperature oversampling 2x 0b010, pressure oversampling 16x 0b101 */

    ret = sync_iic_master_send(&environment_sensor_iic_info_m, write_buffer, 2);

    acknowledge_polling(&environment_sensor_iic_info_m);

    write_buffer[0] = 0x64; /* gas_wait_0 */
    write_buffer[1] = 0x59; /* 100ms heat up duration */

    ret = sync_iic_master_send(&environment_sensor_iic_info_m, write_buffer, 2);

    acknowledge_polling(&environment_sensor_iic_info_m);

    write_buffer[0] = 0x5a; /* res_heat_0 */
    write_buffer[1] = 0x80; /* heater set-point - TODO: what is the right value? */

    ret = sync_iic_master_send(&environment_sensor_iic_info_m, write_buffer, 2);

    acknowledge_polling(&environment_sensor_iic_info_m);

    write_buffer[0] = 0x71; /* nb_conv */
    write_buffer[1] = 0x0; /* previously defined settings */

    ret = sync_iic_master_send(&environment_sensor_iic_info_m, write_buffer, 2);

    acknowledge_polling(&environment_sensor_iic_info_m);

    write_buffer[0] = 0x71; /* run_gas_l */
    write_buffer[1] = 0x10; /* enable gas measurements */

    ret = sync_iic_master_send(&environment_sensor_iic_info_m, write_buffer, 2);

    acknowledge_polling(&environment_sensor_iic_info_m);

    write_buffer[0] = 0x74; /* osrs_t , osrs_p */
    write_buffer[1] = 0x55; /* temperature oversampling 2x 0b010, pressure oversampling 16x 0b101 */

    ret = sync_iic_master_send(&environment_sensor_iic_info_m, write_buffer, 2);

    acknowledge_polling(&environment_sensor_iic_info_m);

    /* read the 20-bit temperature value */
    write_buffer[0] = 0x22; /* register 22 temp_msb */

    ret = sync_iic_master_send(&environment_sensor_iic_info_m, write_buffer, 1);

    acknowledge_polling(&environment_sensor_iic_info_m);

    ret = sync_iic_master_receive(&environment_sensor_iic_info_m,
            &read_buffer[0], 1);

    acknowledge_polling(&environment_sensor_iic_info_m);

    temperature = read_buffer[0];
    temperature <<= 12;

    write_buffer[0] = 0x23; /* register 23 temp_lsb */

    ret = sync_iic_master_send(&environment_sensor_iic_info_m, write_buffer, 1);

    acknowledge_polling(&environment_sensor_iic_info_m);

    ret = sync_iic_master_receive(&environment_sensor_iic_info_m,
            &read_buffer[1], 1);

    acknowledge_polling(&environment_sensor_iic_info_m);

    temperature |= read_buffer[1];
    temperature <<= 4;

    write_buffer[0] = 0x24; /* register 24 temp_xlsb */

    ret = sync_iic_master_send(&environment_sensor_iic_info_m, write_buffer, 1);

    acknowledge_polling(&environment_sensor_iic_info_m);

    ret = sync_iic_master_receive(&environment_sensor_iic_info_m,
            &read_buffer[1], 1);

    acknowledge_polling(&environment_sensor_iic_info_m);

    temperature = 0;
    temperature = (read_buffer[0] << 16) | (read_buffer[1] << 8)
            | (read_buffer[2] >> 4);

    /* Read Callibration Data for temperature sensor */
    uint8_t calib[41] =
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    write_buffer[0] = 0x89;
    ret = sync_iic_master_send(&environment_sensor_iic_info_m, write_buffer, 1);

    acknowledge_polling(&environment_sensor_iic_info_m);

    ret = sync_iic_master_receive(&environment_sensor_iic_info_m, &calib[0],
            25);/* may need to set LSB of address */

    acknowledge_polling(&environment_sensor_iic_info_m);

    write_buffer[0] = 0x16;

    ret = sync_iic_master_send(&environment_sensor_iic_info_m, write_buffer, 1);

    acknowledge_polling(&environment_sensor_iic_info_m);

    ret = sync_iic_master_receive(&environment_sensor_iic_info_m, &calib[25],
            1);/* may need to set LSB of address */

    acknowledge_polling(&environment_sensor_iic_info_m);

    ret = R_RIIC_Close(&environment_sensor_iic_info_m);

    /* Temperature Sensor compensation parameters */
    uint16_t dig_t1;
    int16_t dig_t2;
    int8_t dig_t3;

    dig_t1 = (uint16_t) (((uint16_t) calib[34] << 8) | calib[33]);
    dig_t2 = (int16_t) (((int16_t) calib[2] << 8) | calib[1]);
    dig_t3 = (int8_t) (calib[3]);

    return temperature;
}

uint32_t read_bmi160_inertial_sensor() {

    riic_info_t inertial_sensor_iic_info_m;
    riic_mcu_status_t inertial_sensor_iic_status_m;
    riic_return_t ret;
    uint8_t  inertial_sensor_addr[1] = {BME160_ADDRESS};

    uint8_t write_buffer[2];
    uint8_t read_buffer[3];
    uint8_t chip_id;
    uint32_t sensor_time = 0;

    memset(write_buffer, 0, sizeof(write_buffer));
    memset(read_buffer, 0, sizeof(read_buffer));

    inertial_sensor_iic_info_m.dev_sts = RIIC_NO_INIT;
    inertial_sensor_iic_info_m.ch_no = 0;
    inertial_sensor_iic_info_m.callbackfunc = &callback_master;
    inertial_sensor_iic_info_m.p_slv_adr = inertial_sensor_addr;

    ret = R_RIIC_Open(&inertial_sensor_iic_info_m);

    if (RIIC_SUCCESS != ret) {
        /* This software is for single master.
         Therefore, return value should be always 'RIIC_SUCCESS'. */
        while (1) {
            nop(); /* error */
        }
    }

    acknowledge_polling(&inertial_sensor_iic_info_m);

    /* read the device ID */
    write_buffer[0] = 0x0; /* register 0 chip ID */
    ret = sync_iic_master_send(&inertial_sensor_iic_info_m, write_buffer, 1);

    acknowledge_polling(&inertial_sensor_iic_info_m);

    ret = sync_iic_master_receive(&inertial_sensor_iic_info_m, read_buffer, 1); /* may need to set LSB of address */

    acknowledge_polling(&inertial_sensor_iic_info_m);

    /* chip ID should be 0xd1 */
    chip_id = read_buffer[0];
    if (0xd1 != chip_id) {
        nop();
    }

    memset(read_buffer, 0, sizeof(read_buffer));

    /* read the sensor time */
    write_buffer[0] = 0x18; /* register 0 chip ID */
    ret = sync_iic_master_send(&inertial_sensor_iic_info_m, write_buffer, 1);

    acknowledge_polling(&inertial_sensor_iic_info_m);

    ret = sync_iic_master_receive(&inertial_sensor_iic_info_m, read_buffer, 3); /* may need to set LSB of address */

    acknowledge_polling(&inertial_sensor_iic_info_m);

    ret = R_RIIC_Close(&inertial_sensor_iic_info_m);

    /* compose the 24-bit sensor time value */
    sensor_time = read_buffer[2];
    sensor_time <<= 8;
    sensor_time |= read_buffer[1];
    sensor_time <<= 8;
    sensor_time |= read_buffer[0];

    return sensor_time;
}

void swap_byte_order(uint8_t *byte) {

    /* Get the first nibble */
    uint8_t nibble1 = *byte & 0x0F;
    uint8_t nibble2 = *byte & 0xF0;

    *byte = (nibble1 << 4) | (nibble2 >> 4);
}
