#include "cpu.h"

static std::deque<double> cpu_history;  
static const int MAX_HISTORY = 180;
std::string cpuName = "";

std::string cpu_name(){
    std::ifstream file("/proc/cpuinfo");
    std::string line;
    while (std::getline(file, line)){   
        
        std::istringstream ss(line);
        if (line.find("model name") != 0) continue;
        file.close();
        return line.substr(line.find(":") + 2, line.length()-1);
    }
    return "model not found";
}

std::map<int, CPUStats> cpu_stats() {
    std::ifstream file("/proc/stat");
    std::string line;
    int cpu_num = -1;
    std::map<int, CPUStats> out;
    
    while (true){   
        CPUStats stats;
        std::getline(file, line);
        std::istringstream ss(line);
        std::string cpu;

        ss >> cpu;
        if (cpu.find("cpu") != 0) break;
        ss >> stats.user >> stats.nice >> stats.system >> stats.idle >> stats.iowait >> stats.irq >> stats.softirq >> stats.steal;
        out[cpu_num] = stats;
        cpu_num += 1;
    }
    return out;
}

// Вычисление процента загрузки CPU
double get_cpu_usage(CPUStats prev, CPUStats curr) {
    
    long prev_idle = prev.idle + prev.iowait;
    long curr_idle = curr.idle + curr.iowait;
    
    long prev_total = prev.user + prev.nice + prev.system + prev.idle + prev.iowait + prev.irq + prev.softirq + prev.steal;
    long curr_total = curr.user + curr.nice + curr.system + curr.idle + curr.iowait + curr.irq + curr.softirq + curr.steal;
    
    long total_diff = curr_total - prev_total;
    long idle_diff = curr_idle - prev_idle;
    
    if (total_diff == 0) return 0.0;
    return 100.0 * (total_diff - idle_diff) / total_diff;
}

std::map<int, double> cpu_usage() {
    std::map<int, CPUStats> prev = cpu_stats();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::map<int, CPUStats> curr = cpu_stats();
    std::map<int, double> out;
    for (auto it = prev.begin(); it != prev.end(); ++it) {
        out[it->first] = get_cpu_usage(prev[it->first], curr[it->first]);
    }
    return out;
}

std::string get_all_cpu_usage() {
    auto usage = cpu_usage();  // ОДИН вызов
    if (cpuName == ""){
        cpuName = cpu_name();
    }
    // === ТАБЛИЦА ===
    std::string table = "<h1 style=\"margin-top:0;\">";
    table += cpuName;
    table += "</h1><table style='width:100%; border-collapse:collapse;'>";
    
    for (const auto& [core, percent] : usage) {
        std::string current_core;
        if (core == -1) {
            current_core = "Общая нагрузка";
        } else {
            current_core = "Ядро " + std::to_string(core);
        }
        
        std::string color = percent > 80 ? "#d9484e" : 
                           percent > 50 ? "#e8843c" : "#41a4ad";
        
        table += "<tr>";
        table += "<td style='padding:6px 12px; white-space:nowrap; font-size:14px;'><b>" + current_core + "</b></td>";
        table += "<td style='padding:6px 12px; width:100%;'>";
        table += "<div style='background:#eee; width:100%; height:28px; border-radius:4px; position:relative;'>";
        table += "<div style='background:" + color + "; width:" + std::to_string(static_cast<int>(percent)) 
              + "%; height:28px; border-radius:4px; position:absolute; top:0; left:0; transition: width 0.4s;'></div>";
        table += "<span style='position:absolute; top:0; right:10px; height:28px; line-height:28px; "
              + std::string("color:black")
              + "font-weight:bold; font-size:13px; z-index:1;'>" 
              + std::to_string(static_cast<int>(percent)) + "%</span>";
        table += "</div></td>";
        table += "</tr>";
    }
    table += "</table>";
    
    // === ГРАФИК ===
    double total_load = usage[-1];
    
    static std::deque<double> history;
    history.push_back(total_load);
    if (history.size() > MAX_HISTORY) history.pop_front();
    
    std::string svg;
    
    if (history.size() < 2) {
        svg = "<svg viewBox='0 0 300 30' preserveAspectRatio='none' "
            "style='width:100%; height:300px;' xmlns='http://www.w3.org/2000/svg'>"
            "<rect width='300' height='300' fill='#fafafa' rx='4'/>"
            "</svg>";
    } else {
        const int W = 600, H = 300;
        const int padL = 18, padR = 18, padT = 8, padB = 8;
        const int gW = W - padL - padR;
        const int gH = H - padT - padB;
        
        svg = "<svg viewBox='0 0 " + std::to_string(W) + " " + std::to_string(H) 
            + "' preserveAspectRatio='none' "
            + "style='width:100%; height:300px;' xmlns='http://www.w3.org/2000/svg'>";
        
        // Фон
        svg += "<rect width='" + std::to_string(W) + "' height='" + std::to_string(H) 
            + "' fill='#fafafa' rx='4'/>";
        
        for (int pct = 0; pct <= 100; pct += 20) {
            int y = padT + gH - (pct * gH / 100);
            svg += "<line x1='" + std::to_string(padL) + "' y1='" + std::to_string(y)
                + "' x2='" + std::to_string(W - padR) + "' y2='" + std::to_string(y)
                + "' stroke='#989898' stroke-width='0.5'/>";
        }
        // Заливка и линия
        std::string path, areaPath;
        for (size_t i = 0; i < history.size(); i++) {
            double x = padL + (i * gW) / MAX_HISTORY - 1.0;
            double y = padT + gH - (history[i] * gH / 100.0);
            
            if (i == 0) {
                path += "M" + std::to_string(x) + "," + std::to_string(y);
                areaPath += "M" + std::to_string(x) + "," + std::to_string(padT + gH)
                         + " L" + std::to_string(x) + "," + std::to_string(y);
            } else {
                path += " L" + std::to_string(x) + "," + std::to_string(y);
                areaPath += " L" + std::to_string(x) + "," + std::to_string(y);
            }
        }
        
        
        svg += "<path d='" + path + "' fill='none' stroke='#21848d' stroke-width='2' stroke-linejoin='round' vector-effect='non-scaling-stroke'/>";
        svg += "</svg>";
    }
    
    // === СБОРКА: график сверху, таблица снизу ===
    std::string html = "<div style='display:flex; flex-direction:column; gap:20px;'>";

    // Верх: график
    html += "<div style='max-width:100%;'>";
    html += "<div style='font-size:13px; font-weight:bold; color:#555; margin-bottom:6px;'>""</div>";
    html += svg;
    html += "</div>";

    // Низ: таблица
    html += "<div>";
    html += table;
    html += "</div>";

    html += "</div>";

    return html;
}

