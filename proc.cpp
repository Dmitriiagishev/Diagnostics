#include "proc.h"

int get_cpu_count() {
    std::ifstream file("/proc/stat");
    std::string line;
    int count = 0;
    
    while (std::getline(file, line)) {
        // Считаем строки, начинающиеся с "cpu" и имеющие цифру после "cpu"
        if (line.find("cpu") == 0 && line.size() > 3 && line[3] >= '0' && line[3] <= '9')
            count++;
        else if (line.find("cpu") != 0)
            break;
    }
    return count;
}
std::vector<PROCStats> get_procs_cpu(){
    std::vector<PROCStats> procs;
    FILE *stream = popen("ps -eo user,pid,%cpu,%mem,comm --no-headers --sort=-%cpu", "r");

    char *ptr = nullptr;
    size_t len = 0;
    bool flag = true;
    while (getline(&ptr, &len, stream) != -1) {

        PROCStats proc;
        std::string line(ptr);
        if (!line.empty() && line.back() == '\n')
            line.pop_back();

        std::istringstream ss(line);
        ss >> proc.user >> proc.pid >> proc.cpu >> proc.mem >> proc.command;

        
        procs.push_back(proc);
    }

    free(ptr);
    pclose(stream);
    return procs;
}

std::vector<PROCStats> get_procs_pid(){
    std::vector<PROCStats> procs;
    FILE *stream = popen("ps -eo user,pid,%cpu,%mem,comm --no-headers --sort=pid", "r");

    char *ptr = nullptr;
    size_t len = 0;
    bool flag = true;
    while (getline(&ptr, &len, stream) != -1) {

        PROCStats proc;
        std::string line(ptr);
        if (!line.empty() && line.back() == '\n')
            line.pop_back();

        std::istringstream ss(line);
        ss >> proc.user >> proc.pid >> proc.cpu >> proc.mem >> proc.command;

        
        procs.push_back(proc);
    }

    free(ptr);
    pclose(stream);
    return procs;
}

std::vector<PROCStats> get_procs_user(){
    std::vector<PROCStats> procs;
    FILE *stream = popen("ps -eo user,pid,%cpu,%mem,comm --no-headers --sort=user", "r");

    char *ptr = nullptr;
    size_t len = 0;
    bool flag = true;
    while (getline(&ptr, &len, stream) != -1) {

        PROCStats proc;
        std::string line(ptr);
        if (!line.empty() && line.back() == '\n')
            line.pop_back();

        std::istringstream ss(line);
        ss >> proc.user >> proc.pid >> proc.cpu >> proc.mem >> proc.command;

        
        procs.push_back(proc);
    }

    free(ptr);
    pclose(stream);
    return procs;
}

std::vector<PROCStats> get_procs_mem(){
    std::vector<PROCStats> procs;
    FILE *stream = popen("ps -eo user,pid,%cpu,%mem,comm --no-headers --sort=-%mem", "r");

    char *ptr = nullptr;
    size_t len = 0;
    bool flag = true;
    while (getline(&ptr, &len, stream) != -1) {

        PROCStats proc;
        std::string line(ptr);
        if (!line.empty() && line.back() == '\n')
            line.pop_back();

        std::istringstream ss(line);
        ss >> proc.user >> proc.pid >> proc.cpu >> proc.mem >> proc.command;

        
        procs.push_back(proc);
    }

    free(ptr);
    pclose(stream);
    return procs;
}

// Общая функция построения таблицы
std::string build_proc_table(std::vector<PROCStats> procs) {
    std::string table = "<table style='width:100%; border-collapse:collapse;'>";
    
    // Заголовок с кликабельными колонками
    table += "<tr style='border-bottom:2px solid #ddd;'>";
    table += "<td class='sort-link' onclick=\"loadProc('user')\" style='padding:8px 12px; font-size:18px; color:#888; font-weight:bold; cursor:pointer; width:120px;'>USER ▾</td>";
    table += "<td class='sort-link' onclick=\"loadProc('pid')\" style='padding:8px 12px; font-size:18px; color:#888; font-weight:bold; cursor:pointer; width:120px;'>PID ▾</td>";
    table += "<td class='sort-link' onclick=\"loadProc('cpu')\" style='padding:8px 12px; font-size:18px; color:#888; font-weight:bold; cursor:pointer; width:120px;'>CPU ▾</td>";
    table += "<td class='sort-link' onclick=\"loadProc('mem')\" style='padding:8px 12px; font-size:18px; color:#888; font-weight:bold; cursor:pointer; width:200px;'>MEM ▾</td>";
    table += "<td style='padding:8px 12px; font-size:18px; color:#888; font-weight:bold;'>COMMAND</td>";
    table += "</tr>";
    
    for (size_t i = 0; i < procs.size(); i++) {
        double cpu_val = std::stod(procs[i].cpu) / get_cpu_count();
        double mem_val = std::stod(procs[i].mem);
        
        std::string cpu_color = cpu_val > 50 ? "#d9484e" : 
                                 cpu_val > 20 ? "#e8843c" : "#41a4ad";
        std::string mem_color = mem_val > 50 ? "#d9484e" : 
                                 mem_val > 20 ? "#e8843c" : "#41a4ad";
        
        std::string bg = i % 2 == 0 ? "#fafafa" : "white";
        
        table += "<tr style='background:" + bg + ";'>";
        table += "<td style='padding:12px 18px;"
        + std::string("color:") + (procs[i].user == "root" ? "red;" : "black;") +
        "font-size:18 px;'>" + procs[i].user + "</td>";
        table += "<td style='padding:12px 18px; font-size:18 px; color:#888; font-family:monospace;'>" + procs[i].pid + "</td>";
        
        // CPU
        table += "<td style='padding:12px 16px;'>";
        table += "<div style='background:#eee; height:30px; border-radius:3px; position:relative;'>";
        table += "<div style='background:" + cpu_color + "; width:" + std::to_string(static_cast<int>(cpu_val)) 
      + "%; height:30px; border-radius:3px;'></div>";
        table += "<span style='position:absolute; top:0; left:4px; height:30px; line-height:30px; "
              + std::string("color:black;")
              + "font-size:18px; font-weight:bold;'>" + std::to_string(static_cast<int>(cpu_val)) + "%</span>";
        table += "</div></td>";
        
        // MEM
        table += "<td style='padding:12px 16px;'>";
        table += "<div style='background:#eee; height:30px; border-radius:3px; position:relative;'>";
        table += "<div style='background:" + mem_color + "; width:" + procs[i].mem 
              + "%; height:30px; border-radius:3px;'></div>";
        table += "<span style='position:absolute; top:0; left:4px; height:30px; line-height:30px; "
              + std::string("color:black;")
              + "font-size:18px; font-weight:bold;'>" + procs[i].mem + "%</span>";
        table += "</div></td>";
        
        std::string cmd = procs[i].command;
        if (cmd.length() > 40) cmd = cmd.substr(0, 37) + "...";
        table += "<td style='padding:12px 16px; font-size:30 px; font-family:monospace; color:#333;'>" 
              + cmd + "</td>";
        
        table += "</tr>";
    }
    
    table += "</table>";
    return table;
}

std::string get_proc_usage_cpu_html() {
    return build_proc_table(get_procs_cpu());
}

std::string get_proc_usage_mem_html() {
    return build_proc_table(get_procs_mem());
}

std::string get_proc_usage_pid_html() {
    return build_proc_table(get_procs_pid());
}

std::string get_proc_usage_user_html() {
    return build_proc_table(get_procs_user());
}
