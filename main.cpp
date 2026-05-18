#ifndef CROW_USE_BOOST
#define CROW_USE_BOOST
#endif
#include "crow.h"
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <unordered_map>
#include "cpu.h"
#include "ram.h"
#include "proc.h"

std::string get_dynamic_content(int i) {
    return std::to_string(cpu_usage()[i]);
}

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){ 
        std::ifstream file("templates/index.html");
        std::stringstream buffer;
        
        if (file.is_open()) {
            buffer << file.rdbuf();
            std::string content = buffer.str();
            
            size_t pos;
            if ((pos = content.find("{{title}}")) != std::string::npos)
                content.replace(pos, 9, "Мониторинг CPU");
            if ((pos = content.find("{{heading}}")) != std::string::npos)
                content.replace(pos, 11, "Загрузка процессора");
            if ((pos = content.find("{{initial_CPU}}")) != std::string::npos)
                content.replace(pos, 15, get_all_cpu_usage());
            if ((pos = content.find("{{initial_RAM}}")) != std::string::npos)
                content.replace(pos, 15, get_ram_usage_html());
            if ((pos = content.find("{{initial_PROC}}")) != std::string::npos)
                content.replace(pos, 16, get_proc_usage_pid_html());
            return content;
        }
        return std::string("<h1>Ошибка загрузки шаблона</h1>");
    });

    // API для динамического обновления
    CROW_ROUTE(app, "/api/cpu")([](){
        return get_all_cpu_usage();
    });

    CROW_ROUTE(app, "/api/ram")([](){
        return get_ram_usage_html();
    });

    CROW_ROUTE(app, "/api/proc_pid")([](){
        return get_proc_usage_pid_html();
    });

    CROW_ROUTE(app, "/api/proc_cpu")([](){
        return get_proc_usage_cpu_html();
    });

    CROW_ROUTE(app, "/api/proc_mem")([](){
        return get_proc_usage_mem_html();
    });

    CROW_ROUTE(app, "/api/proc_user")([](){
        return get_proc_usage_cpu_html();
    });

    app.port(18080).run();
}