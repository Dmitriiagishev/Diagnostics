#ifndef PROC_H
#define PROC_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <deque>

struct PROCStats {
    std::string user, pid, cpu, mem, command;
};

std::string get_proc_usage_cpu_html();
std::string get_proc_usage_mem_html();
std::string get_proc_usage_pid_html();
std::string get_proc_usage_user_html();


std::vector<PROCStats> get_procs();
std::string get_proc_usage_html();

#endif