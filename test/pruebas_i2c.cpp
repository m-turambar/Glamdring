void leftovers()
{
    I2C i2c2(I2C::Peripheral::I2C1);
    i2c2.enable(I2C::Timing::Standard);

    MPU6050 mpu(i2c2); //instancia que representa a nuestro acelerómetro
    mpu.set_sampling_rate();

    uart2.transmitq((const uint8_t *)greetings, strlen((const char*)greetings));
    uart2 << greetings;
    uint8_t buf[16] = {0};
    float acc[3] = {0};

    while (1) 
    {
        if(glb_flag%2 == 1) 
        {
            mpu.posicionar_en_registro_ax();
            mpu.leer(buf, 6);
            mpu.convert_to_float(acc, buf, 3);

            std::sprintf(tx_buf, "ax=%.2f\t ay=%.2f\t az=%.2f\n\r", acc[0], acc[1], acc[2]);

            //uart2.transmitq(tx_buf, std::strlen((const char*)tx_buf));
                uart2 << tx_buf;
            glb_flag = 0;
        }

        if(blue_btn.read_input() == 0) 
        {
            t17.start();
        }

        if(uart2.available())
            uart3 << uart2.read_byte();
    }
}