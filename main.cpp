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

// Завдання 2: Data Race — небезпечне збільшення counter

// Глобальна змінна — спільна для всіх потоків
// DATA RACE: кілька потоків читають і пишуть її без захисту
int counter = 0;

void task2() {
    std::cout << "=== Task 2: Data Race demonstration ===" << std::endl;
    counter = 0;  // скидаємо перед тестом

    // Лямбда збільшує counter на thread_id
    // [thread_id] — захоплюємо id за значенням
    // DATA RACE тут: counter += thread_id — не атомарна операція
    // Крок 1: читаємо counter
    // Крок 2: додаємо thread_id
    // Крок 3: записуємо назад
    // Між кроком 1 і 3 інший потік може вже змінити counter
    auto unsafeIncrement = [](int thread_id) {
        for (int i = 0; i < 5; i++) {
            // sleep_for — штучна затримка щоб потоки "переплітались"
            // збільшує шанс побачити Data Race
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            counter += thread_id;  // ← DATA RACE: не захищено!

            std::cout << "Thread " << thread_id
                << " → counter = " << counter << std::endl;
        }
        };

    std::thread t1(unsafeIncrement, 1);
    std::thread t2(unsafeIncrement, 2);
    std::thread t3(unsafeIncrement, 3);

    t1.join();
    t2.join();
    t3.join();

    std::cout << "Final counter (expected " << (1 + 2 + 3) * 5 << "): "
        << counter << std::endl;
    std::cout << std::endl;
}

// Завдання 3: mutex захищає counter від Data Race

// Mutex — замок. Тільки один потік може його тримати одночасно
std::mutex counterMutex;

void task3() {
    std::cout << "=== Task 3: mutex protection, no Data Race ===" << std::endl;
    counter = 0;

    auto safeIncrement = [](int thread_id) {
        for (int i = 0; i < 5; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            // lock_guard — блокує mutex при створенні
            // автоматично розблоковує коли виходить зі scope (кінець if-блоку)
            // поки один потік тут — інші чекають перед цим рядком
            {
                std::lock_guard<std::mutex> lock(counterMutex);
                counter += thread_id;  // захищено — тільки один потік тут одночасно
                std::cout << "Thread " << thread_id
                    << " → counter = " << counter << std::endl;
            }
            // тут mutex вже розблоковано — інший потік може увійти
        }
        };

    std::thread t1(safeIncrement, 1);
    std::thread t2(safeIncrement, 2);
    std::thread t3(safeIncrement, 3);

    t1.join(); t2.join(); t3.join();

    std::cout << "Final counter (expected " << (1 + 2 + 3) * 5 << "): "
        << counter << std::endl;
    std::cout << std::endl;
}

// Завдання 4: Deadlock — два потоки, два mutex, різний порядок
std::mutex mutex1;
std::mutex mutex2;

void task4() {
    std::cout << "=== Task 4: Deadlock demonstration ===" << std::endl;
    std::cout << "WARNING: this will deadlock! Close manually." << std::endl;

    // Потік 1: спочатку бере mutex1, потім mutex2
    std::thread t1([]() {
        std::lock_guard<std::mutex> lock1(mutex1);  // бере mutex1
        std::cout << "Thread 1: got mutex1, waiting for mutex2..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // пауза щоб потік 2 встиг взяти mutex2
        std::lock_guard<std::mutex> lock2(mutex2);  // чекає mutex2 — але його тримає потік 2!
        std::cout << "Thread 1: got both mutexes" << std::endl;
        });

    // Потік 2: спочатку бере mutex2, потім mutex1
    std::thread t2([]() {
        std::lock_guard<std::mutex> lock2(mutex2);  // бере mutex2
        std::cout << "Thread 2: got mutex2, waiting for mutex1..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::lock_guard<std::mutex> lock1(mutex1);  // чекає mutex1 — але його тримає потік 1
        std::cout << "Thread 2: got both mutexes" << std::endl;
        });

    // DEADLOCK: t1 чекає mutex2 який тримає t2
    //           t2 чекає mutex1 який тримає t1

    t1.join();
    t2.join();

    std::cout << std::endl;
}

int main() {
    task1();
    task2();
    task3();
    // task4(); // викличе deadlock
    std::cin.get();
    return 0;
}