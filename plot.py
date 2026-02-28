import pandas as pd
import matplotlib.pyplot as plt

def plot(file_path):
    try:
        df = pd.read_csv(file_path)

        plt.figure(figsize=(10, 6))

        plt.plot(df['size'], df['sequential'], marker='o', label='Послідовний (Sequential)')
        plt.plot(df['size'], df['mutex'], marker='s', label='Блокуючий (Mutex)')
        plt.plot(df['size'], df['atomic'], marker='^', label='Неблокуючий (Atomic CAS)')

        # Налаштування осей та заголовка
        plt.title('Порівняння часу виконання алгоритмів', fontsize=14)
        plt.xlabel('Кількість елементів (N)', fontsize=12)
        plt.ylabel('Час виконання (секунди)', fontsize=12)

        plt.xscale('log')
        plt.yscale('log')

        plt.grid(True, which="both", ls="-", alpha=0.5)
        plt.legend()

        plt.savefig('performance_graph.png')
        print("Графік успішно збережено як 'performance_graph.png'")

        # Показ графіка
        plt.show()

    except Exception as e:
        print(f"Помилка при побудові графіка: {e}")

if __name__ == "__main__":
    plot('cmake-build-debug/test_results.csv')