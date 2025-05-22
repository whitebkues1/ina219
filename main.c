/**
 * @brief Perform calibration sweep for INA219 across a range of expected maximum currents.
 *
 * This function iterates over a range of expected maximum current values and calculates
 * the corresponding Current_LSB and calibration register values for the INA219 current sensor.
 * It writes the calibration to the device, reads bus voltage, current, and shunt voltage,
 * and prints the result in a structured format suitable for log analysis.
 *
 * @param[in] ina219    Pointer to INA219 instance structure
 * @param[in] rshunt    Shunt resistor value in ohms
 * @param[in] imin      Minimum expected current (mA) to start calibration from (e.g., 205)
 * @param[in] imax      Maximum expected current (mA) to stop calibration at (e.g., 3200)
 * @param[in] istep     Step size for sweeping max current values in mA (e.g., 5)
 *
 * @note The calibration register must not exceed 65535. Any computed calibration value greater
 *       than this limit will be skipped with a warning message.
 * @note This function assumes all INA219 read/write functions are already implemented correctly.
 */
void calculate_and_configure_calibration(INA219_t *ina219, double rshunt, int imin, int imax, int istep)
{
    for (int max_current_ma = imin; max_current_ma <= imax; max_current_ma += istep)
    {
        double max_current_a = max_current_ma / 1000.0;  // 转换为安培单位

        double current_lsb = max_current_a / 32768.0;

        printf("=== Imax=%d mA (%.3f A) ===\n", max_current_ma, max_current_a);

        for (int scale = 1; scale <= 8; ++scale)
        {
            double scaled_lsb = current_lsb * scale;
            uint16_t calibration = (uint16_t)(0.04096 / (scaled_lsb * rshunt));

            if (calibration > 65535)
            {
                printf("SKIP: Imax=%d mA ×%d LSB → Calibration value overflow: %u\n", max_current_ma, scale, calibration);
                continue;
            }

            INA219_Reset(ina219);
            HAL_Delay(1);
            uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
                INA219_CONFIG_GAIN_8_320MV | INA219_CONFIG_BADCRES_12BIT |
                INA219_CONFIG_SADCRES_12BIT_1S_532US |
                INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
            INA219_setCalibration(ina219, calibration);
            INA219_setConfig(ina219, config);
            HAL_Delay(1);

            for (int i = 1; i <= 3; ++i)
            {
                uint16_t bus_voltage_mv   = INA219_ReadBusVoltage(ina219);
                int16_t  current_ma       = INA219_ReadCurrent(ina219);
                uint16_t shunt_voltage_mv = INA219_ReadShuntVolage(ina219);

                printf("DATA: READ%d, Imax=%d mA (%.3f A), LSB=%.9f A, Scale=%d, CAL=%u (0x%04X), Bus=%u mV, Current=%d mA, Shunt=%u mV\n",
                    i, max_current_ma, max_current_a, scaled_lsb, scale, calibration, calibration,
                    bus_voltage_mv, current_ma, shunt_voltage_mv);
            }
        }

        printf("--- END Imax=%d mA (%.3f A) ---\n\n", max_current_ma, max_current_a);
    }
}


int main(void) {
    HAL_Init();
    INA219_t ina219;

    double rshunt = 0.1;
    int imin   = 205;   // 205 mA
    int imax   = 3200;  // 3200 mA (3.2 A)
    int istep  = 5;     // 5 mA 步进

    calculate_and_configure_calibration(&ina219, rshunt, imin, imax, istep);

    while (1);
}
