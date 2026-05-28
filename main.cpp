#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>

// Завдання 1: три потоки, лямбда підносить число до квадрату

void task1() {
    std::cout << "=== Task 1: three threads, square ===" << std::endl;

    // Лямбда яка підносить n до квадрату і виводить результат
    // [n] — захоплюємо n за значенням (копія для кожного потоку)
    auto squareLambda = [](int n) {
        int result = n * n;
        std::cout << "Thread " << n << ": "
            << n << "^2 = " << result << std::endl;
        };

    // Створюємо три потоки, кожен виконує squareLambda зі своїм числом
    // std::thread t(функція, аргументи...)
    std::thread t1(squareLambda, 3);
    std::thread t2(squareLambda, 5);
    std::thread t3(squareLambda, 7);

    // join() — чекаємо поки кожен потік завершиться
    // без join — програма може закритись раніше ніж потоки виведуть результат
    t1.join();
    t2.join();
    t3.join();

    std::cout << std::endl;
}

int main() {
    task1();
    std::cin.get();
    return 0;
}