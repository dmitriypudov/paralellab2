#include <iostream>
#include <mpi.h>

// Главная функция программы
int main(int argc, char** argv)
{
    // Инициализируем MPI
    MPI_Init(&argc, &argv);

    // Определяем ранг и количество процессов
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Задаем размер векторов
    const int n = 10000000;

    // Объявляем два массива типа double
    double* x = 0;
    double* y = 0;

    // Происходит выделение памяти для больших векторов и их инициализация случайными числами
    if (rank == 0)
    {
        std::cout << "Processes : " << size << std::endl;
        x = new double[n];
        y = new double[n];

        for (int i = 0; i < n; ++i) {
            x[i] = double(rand()) / RAND_MAX;
            y[i] = double(rand()) / RAND_MAX;
        }
    }

    // Вычисляем размер локальных массивов для каждого процесса
    const int local_size = n / size;

    // Выделяем память для двух локальных массивов типа double
    double* local_x = new double[local_size];
    double* local_y = new double[local_size];

    // Рассылаем элементы массива x по процессам
    MPI_Scatter(x, local_size, MPI_DOUBLE,
        local_x, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Рассылаем элементы массива y по процессам
    MPI_Scatter(y, local_size, MPI_DOUBLE,
        local_y, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Запускаем таймер для измерения времени работы параллельной части программы
    double parallel_process_time = MPI_Wtime();

    // Вычисляем локальный результат скалярного произведения
    double local_result = 0;
    for (int i = 0; i < local_size; ++i) {
        local_result += local_x[i] * local_y[i];
    }

    // Собираем результаты скалярного произведения со всех процессов в глобальный результат
    double global_result;
    MPI_Reduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Останавливаем таймер и вычисляем время работы параллельной части программы
    double end_parallel_time = MPI_Wtime() - parallel_process_time;

    // Если ранг равен 0 (корневой процесс), то выводим результаты работы программы
    if (rank == 0) {
        std::cout << "Processes result : " << global_result << std::endl;
        std::cout << "Processes time : " << end_parallel_time << std::endl;

        // Запускаем таймер для измерения времени работы последовательной части программы
        double root_procces_time = MPI_Wtime();

        // Вычисляем результат скалярного произведения в последовательной части программы
        double sum = 0;
        for (int i = 0; i < n; ++i) {
            sum += x[i] * y[i];
        }
        std::cout << "Root result : " << sum << std::endl;

        // Останавливаем таймер и вычисляем время работы последовательной части программы
        double end_root_time = MPI_Wtime() - root_procces_time;
        std::cout << "Root time : " << end_root_time << std::endl;
        std::cout << "Speeding up : " << end_root_time / end_parallel_time << std::endl;

        // Освобождаем память, выделенную под массивы x и y
        delete[] x;
        delete[] y;
    }

    // Освобождаем память, выделенную под локальные массивы local_x и local_y
    delete[] local_x;
    delete[] local_y;

 
    MPI_Finalize();


    return 0;
}