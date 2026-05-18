#ifndef CPU_H
#define CPU_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <map>
#include <deque>

// Структура для хранения данных CPU
struct CPUStats {
    long user, nice, system, idle, iowait, irq, softirq, steal;
};

// Чтение статистики CPU из /proc/stat
double get_cpu_usage(CPUStats prev, CPUStats curr);

std::string cpu_name();
// Вычисление процента загрузки CPU
std::map<int, double> cpu_usage();

std::string get_all_cpu_usage();

//std::string escape_json(const std::string& s);

#endif // CPU_H