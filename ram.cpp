#include "ram.h"

static std::deque<double> history_total;  
static std::deque<double> history_swap;  
static const int MAX_HISTORY = 180;

std::unordered_map<std::string, long> read_meminfo() {
    std::ifstream file("/proc/meminfo");
    std::string line;
    std::unordered_map<std::string, long> out;
    
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string key;
        long value;
        std::string unit;
        
        ss >> key >> value >> unit;

        if (!key.empty() && key.back() == ':')
            key.pop_back();
        
        out[key] = value;
    }
    return out;
}

RAMStats get_ram_stats() {
    auto info = read_meminfo();
    RAMStats stats;
    stats.memTotal     = info["MemTotal"];
    stats.memFree      = info["MemFree"];
    stats.memAvailable = info["MemAvailable"];
    stats.buffers      = info["Buffers"];
    stats.cached       = info["Cached"];
    stats.swapTotal    = info["SwapTotal"];
    stats.swapFree     = info["SwapFree"];
    return stats;
}



std::string get_ram_usage_html() {
    RAMStats ram = get_ram_stats();
    
    long used = ram.memTotal - ram.memAvailable;
    long buffCache = ram.buffers + ram.cached;
    int usagePercent = static_cast<int>(100.0 * used / ram.memTotal);
    int swapPercent = 0;
    if (ram.swapTotal > 0)
        swapPercent = static_cast<int>(100.0 * (ram.swapTotal - ram.swapFree) / ram.swapTotal);
    
    auto toMB = [](long kb) { return kb / 1024; };
    
    std::string color = usagePercent > 80 ? "#d9484e" : 
                        usagePercent > 50 ? "#e8843c" : "#41a4ad";
    
    std::string table = "<table style='width:100%; border-collapse:collapse;'>";
    
    // Использовано
    table += "<tr>";
    table += "<td style='padding:6px 12px; white-space:nowrap; font-size:14px;'>";
    table += "<span style='background:#21848d; color:white; padding:6px 14px; border-radius:6px; font-weight:bold;'>Использовано</span></td>";
    table += "<td style='padding:6px 12px; width:100%;'>";
    table += "<div style='background:#eee; width:100%; height:28px; border-radius:4px; position:relative;'>";
    table += "<div style='background:" + color + "; width:" + std::to_string(usagePercent) 
         + "%; height:28px; border-radius:4px; position:absolute; top:0; left:0; transition: width 0.4s;'></div>";
    table += "<span style='position:absolute; top:0; right:10px; height:28px; line-height:28px; "
          + std::string("color:black;")
          + "font-weight:bold; font-size:13px; z-index:1;'>" 
          + std::to_string(toMB(used)) + " / " + std::to_string(toMB(ram.memTotal)) + " MB (" 
          + std::to_string(usagePercent) + "%)</span>";
    table += "</div></td>";
    table += "</tr>";
    
    // Swap
    if (ram.swapTotal > 0) {
        std::string swapColor = swapPercent > 50 ? "#d9484e" : "#41a4ad";
        
        table += "<tr>";
        table += "<td style='padding:6px 12px; white-space:nowrap; font-size:14px;'>";
        table += "<span style='background:#e9c46a; color:white; padding:6px 14px; border-radius:6px; font-weight:bold;'>Swap</span></td>";
        table += "<td style='padding:6px 12px; width:100%;'>";
        table += "<div style='background:#eee; width:100%; height:28px; border-radius:4px; position:relative;'>";
        table += "<div style='background:" + swapColor + "; width:" + std::to_string(swapPercent) 
             + "%; height:28px; border-radius:4px; position:absolute; top:0; left:0; transition: width 0.4s;'></div>";
        table += "<span style='position:absolute; top:0; right:10px; height:28px; line-height:28px; "
              + std::string("color:black")
              + "font-weight:bold; font-size:13px; z-index:1;'>" 
              + std::to_string(toMB(ram.swapTotal - ram.swapFree)) + " / " 
              + std::to_string(toMB(ram.swapTotal)) + " MB (" 
              + std::to_string(swapPercent) + "%)</span>";
        table += "</div></td>";
        table += "</tr>";
    }
    
    // Буферы + Кэш
    table += "<tr>";
    table += "<td style='padding:6px 12px; white-space:nowrap; font-size:14px;'><b>Буферы + Кэш</b></td>";
    table += "<td style='padding:6px 12px; font-size:13px; color:#555;'>" 
          + std::to_string(toMB(buffCache)) + " MB</td>";
    table += "</tr>";

    
    table += "</table>";

    
    // === ГРАФИК ===
    double total_load = usagePercent;
    
    static std::deque<double> history_total;
    history_total.push_back(total_load);

    static std::deque<double> history_swap;
    history_swap.push_back(swapPercent);

    if (history_total.size() > MAX_HISTORY) history_total.pop_front();
    if (history_swap.size() > MAX_HISTORY) history_swap.pop_front();
    
    std::string svg;
    
    if (history_total.size() < 2) {
        svg = "<svg viewBox='0 0 600 300' preserveAspectRatio='none' "
            "style='width:100%; height:300px;' xmlns='http://www.w3.org/2000/svg'>"
            "<rect width='600' height='300' fill='#fafafa' rx='4'/>"
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
        
        
        // Сетка
        for (int pct = 0; pct <= 100; pct += 20) {
            int y = padT + gH - (pct * gH / 100);
            svg += "<line x1='" + std::to_string(padL) + "' y1='" + std::to_string(y)
                + "' x2='" + std::to_string(W - padR) + "' y2='" + std::to_string(y)
                + "' stroke='#989898' stroke-width='0.5'/>";
        }

        // Заливка и линия
        std::string path, areaPath;
        for (size_t i = 0; i < history_total.size(); i++) {
            double x = padL + (i * gW) / MAX_HISTORY - 1.0;
            double y = padT + gH - (history_total[i] * gH / 100.0);
            
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
        
        path = "";
        areaPath = "";
        for (size_t i = 0; i < history_swap.size(); i++) {
            double x = padL + (i * gW) / MAX_HISTORY - 1.0;
            double y = padT + gH - (history_swap[i] * gH / 100.0);
            
            if (i == 0) {
                path += "M" + std::to_string(x) + "," + std::to_string(y);
                areaPath += "M" + std::to_string(x) + "," + std::to_string(padT + gH)
                         + " L" + std::to_string(x) + "," + std::to_string(y);
            } else {
                path += " L" + std::to_string(x) + "," + std::to_string(y);
                areaPath += " L" + std::to_string(x) + "," + std::to_string(y);
            }
        }
        svg += "<path d='" + path + "' fill='none' stroke='#e9c46a' stroke-width='2' stroke-linejoin='round' vector-effect='non-scaling-stroke'/>";
      
        
        svg += "</svg>";
    }
    std::string html = "<div style='display:flex; flex-direction:column; gap:20px;'>";

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