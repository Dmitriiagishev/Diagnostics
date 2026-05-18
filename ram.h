#ifndef RAM_H
#define RAM_H

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <map>
#include <deque>

struct RAMStats {
    long memTotal, memFree, memAvailable, buffers, cached;
    long swapTotal, swapFree;
};

std::unordered_map<std::string, long> read_meminfo();
RAMStats get_ram_stats();
std::string get_ram_usage_html();

#endif