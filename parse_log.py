import re
import pandas as pd

def parse_ina219_log_to_excel(log_filename, excel_filename):
    pattern_data = re.compile(
        r'DATA: READ(\d+), Imax=(\d+) mA \(([\d.]+) A\), LSB=([\d.eE+-]+) A, Scale=(\d+), CAL=(\d+) \(0x[0-9A-Fa-f]+\), Bus=(\d+) mV, Current=(-?\d+) mA, Shunt=(\d+) mV'
    )

    data_list = []

    with open(log_filename, 'r', encoding='utf-8') as f:
        for line in f:
            match = pattern_data.search(line)
            if match:
                data_list.append({
                    'Imax_mA': int(match.group(2)),
                    'Imax_A': float(match.group(3)),
                    'READ': int(match.group(1)),
                    'LSB_A': float(match.group(4)),
                    'Scale': int(match.group(5)),
                    'Calibration': int(match.group(6)),
                    'Bus_Voltage_mV': int(match.group(7)),
                    'Current_mA': int(match.group(8)),
                    'Shunt_Voltage_mV': int(match.group(9)),
                })

    df = pd.DataFrame(data_list)
    df.to_excel(excel_filename, index=False)
    print(f"解析完成，数据已保存到: {excel_filename}")

if __name__ == '__main__':
    parse_ina219_log_to_excel('ina219_log.txt', 'ina219_results.xlsx')
