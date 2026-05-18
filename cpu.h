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
#include <cstdio>

struct CPUStats {
    long user, nice, system, idle, iowait, irq, softirq, steal;
};
double get_cpu_usage(CPUStats prev, CPUStats curr);
std::string cpu_name();
std::map<int, double> cpu_usage();
std::string get_all_cpu_usage();

#endif