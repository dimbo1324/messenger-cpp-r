#include <iostream>
#include <iomanip>
//#include <numeric>
#include <chrono>


class Data {
public:
    static const int DATA_SIZE = 2'000'000;
    int rand_data[DATA_SIZE];
    Data() {
        srand(static_cast<unsigned int>(time(NULL))); // use current time as seed for random generator
        int random_variable = rand();
        for (auto i = 0; i < DATA_SIZE; i++) {
           rand_data[i] = rand() % 11;
            //std::cout << rand_data[i] << "  ";
        }
        std::cout << std::endl;
    }
    long long sumData(Data d) {
        long long result {0};
        for (auto i = 0; i < DATA_SIZE; i++) {
            result += d.rand_data[i];
        }
        return result;
    }
    long long sumDataRef(Data& d) {
        long long result {0};
        for (auto i = 0; i < DATA_SIZE; i++) {
            result += d.rand_data[i];
        }
        return result;
    }
    long long sumDataRefConst(const Data& d) {
        long long result{ 0 };
        for (auto i = 0; i < DATA_SIZE; i++) {
            result += d.rand_data[i];
        }
        return result;
    }

};


int main() {
    

    Data data;

    std::cout << std::fixed << std::setprecision(9) << std::left;
    {
        long long sum {0};
        // record start time
        auto start = std::chrono::system_clock::now();
        for (int i = 0; i < 100; i++)
            sum = data.sumDataRef(data);
        // record end time
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end - start;
        std::cout << "Average value = " << static_cast<double>(sum) / data.DATA_SIZE << "\t Time sumDataRef = " << diff.count() << std::endl;
    }
    {
        long long sum{ 0 };
        // record start time
        auto start = std::chrono::system_clock::now();
        for (int i = 0; i < 100; i++)
            sum = data.sumDataRefConst(data);
        // record end time
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end - start;
        std::cout << "Average value = " << static_cast<double>(sum) / data.DATA_SIZE << "\t Time sumDataRefConst = " << diff.count() << std::endl;
    }
    {
        long long sum{ 0 };
        // record start time
        auto start = std::chrono::system_clock::now();
        for (int i = 0; i < 100; i++)
            sum = data.sumData(data);
        // record end time
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end - start;
        std::cout << "Average value = " << static_cast<double>(sum) / data.DATA_SIZE << "\t Time sumData = " << diff.count() << std::endl;
    }
}
