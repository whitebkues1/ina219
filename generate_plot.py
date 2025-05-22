import pandas as pd
import matplotlib.pyplot as plt

def plot_current_vs_calibration(excel_file):
    # 读取Excel
    df = pd.read_excel(excel_file)

    # 检查必须列是否存在
    required_columns = ['Calibration', 'Current_mA']
    for col in required_columns:
        if col not in df.columns:
            raise ValueError(f"缺少列: {col}")

    # 绘图
    plt.figure(figsize=(10, 6))
    plt.plot(df['Calibration'], df['Current_mA'], marker='o', linestyle='-', color='b')

    plt.xlabel('Calibration')
    plt.ylabel('Current (mA)')
    plt.title('Current vs Calibration')
    plt.grid(True)

    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    plot_current_vs_calibration('ina219_results.xlsx')
