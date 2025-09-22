#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include "atmosphere.h"

// Функция для создания файла с данными
void create_data_file(const std::string& filename, const std::vector<double>& altitudes, 
                     const std::vector<double>& values, const std::string& title) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка создания файла: " << filename << std::endl;
        return;
    }
    
    file << "# " << title << "\n";
    file << "# Высота (м)\tЗначение\n";
    for (size_t i = 0; i < altitudes.size(); ++i) {
        file << altitudes[i] << "\t" << values[i] << "\n";
    }
    file.close();
}

// Функция для создания скрипта GNUplot
void create_gnuplot_script(const std::string& script_name, const std::string& data_file, 
                          const std::string& output_file, const std::string& title, 
                          const std::string& xlabel, const std::string& ylabel, 
                          bool logscale = false) {
    std::ofstream script(script_name);
    if (!script.is_open()) {
        std::cerr << "Ошибка создания скрипта: " << script_name << std::endl;
        return;
    }
    
    script << "set terminal pngcairo size 1200,800 enhanced font 'Arial,12'\n";
    script << "set output '" << output_file << "'\n";
    script << "set title '" << title << "'\n";
    script << "set xlabel '" << xlabel << "'\n";
    script << "set ylabel '" << ylabel << "'\n";
    script << "set grid\n";
    script << "set key top right\n";
    
    if (logscale) {
        script << "set logscale y\n";
    }
    
    script << "plot '" << data_file << "' using 1:2 with lines linewidth 2 linecolor rgb '#0066cc' title ''\n";
    script << "exit\n";
    
    script.close();
}

// Функция для запуска GNUplot
void run_gnuplot(const std::string& script_name) {
    std::string command = "gnuplot " + script_name;
    int result = system(command.c_str());
    if (result != 0) {
        std::cout << "GNUplot не установлен или произошла ошибка. Установите GNUplot для просмотра графиков.\n";
    }
}

void print_table_header() {
    std::cout << std::left;
    std::cout << std::setw(10) << "Высота, м" << " | ";
    std::cout << std::setw(10) << "H_гео, м" << " | ";
    std::cout << std::setw(8) << "T, К" << " | ";
    std::cout << std::setw(12) << "p, Па" << " | ";
    std::cout << std::setw(12) << "ρ, кг/м³" << " | ";
    std::cout << std::setw(8) << "a, м/с" << " | ";
    std::cout << std::setw(8) << "g, м/с²" << "\n";
    
    std::cout << std::string(95, '-') << "\n";
}

void print_results(double altitude, const AtmosphereParams& params) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::left;
    
    std::cout << std::setw(10) << altitude << " | ";
    std::cout << std::setw(10) << params.H << " | ";
    std::cout << std::setw(8) << params.T << " | ";
    
    if (params.p >= 1000) {
        std::cout << std::setw(12) << params.p << " | ";
    } else {
        std::cout << std::setw(12) << std::setprecision(6) << params.p << " | ";
        std::cout << std::fixed << std::setprecision(2);
    }
    
    if (params.ro >= 0.1) {
        std::cout << std::setw(12) << params.ro << " | ";
    } else {
        std::cout << std::setw(12) << std::setprecision(6) << params.ro << " | ";
        std::cout << std::fixed << std::setprecision(2);
    }
    
    std::cout << std::setw(8) << params.a << " | ";
    std::cout << std::setw(8) << params.g << "\n";
}

void generate_plots() {
    std::cout << "\nГенерация графиков...\n";
    
    // Создаем данные для графиков
    std::vector<double> altitudes;
    std::vector<double> temperatures;
    std::vector<double> pressures;
    std::vector<double> densities;
    std::vector<double> sound_speeds;
    std::vector<double> gravities;
    
    // Генерируем данные с шагом 1000 м
    for (double h = -2000; h <= 94000; h += 1000) {
        try {
            AtmosphereParams params = calculate_atmosphere(h);
            altitudes.push_back(h);
            temperatures.push_back(params.T);
            pressures.push_back(params.p);
            densities.push_back(params.ro);
            sound_speeds.push_back(params.a);
            gravities.push_back(params.g);
        } catch (...) {
            // Пропускаем невалидные высоты
        }
    }
    
    // Создаем файлы с данными
    create_data_file("temperature_data.txt", altitudes, temperatures, "Зависимость температуры от высоты");
    create_data_file("pressure_data.txt", altitudes, pressures, "Зависимость давления от высоты");
    create_data_file("density_data.txt", altitudes, densities, "Зависимость плотности от высоты");
    create_data_file("sound_speed_data.txt", altitudes, sound_speeds, "Зависимость скорости звука от высоты");
    create_data_file("gravity_data.txt", altitudes, gravities, "Зависимость ускорения свободного падения от высоты");
    
    // Создаем скрипты GNUplot
    create_gnuplot_script("plot_temperature.gp", "temperature_data.txt", "temperature_plot.png", 
                         "Зависимость температуры от высоты", "Высота, м", "Температура, К");
    
    create_gnuplot_script("plot_pressure.gp", "pressure_data.txt", "pressure_plot.png", 
                         "Зависимость давления от высоты", "Высота, м", "Давление, Па", true);
    
    create_gnuplot_script("plot_density.gp", "density_data.txt", "density_plot.png", 
                         "Зависимость плотности от высоты", "Высота, м", "Плотность, кг/м³", true);
    
    create_gnuplot_script("plot_sound_speed.gp", "sound_speed_data.txt", "sound_speed_plot.png", 
                         "Зависимость скорости звука от высоты", "Высота, м", "Скорость звука, м/с");
    
    create_gnuplot_script("plot_gravity.gp", "gravity_data.txt", "gravity_plot.png", 
                         "Зависимость ускорения свободного падения от высоты", "Высота, м", "Ускорение, м/с²");
    
    // Запускаем GNUplot
    std::cout << "Создание графиков...\n";
    run_gnuplot("plot_temperature.gp");
    run_gnuplot("plot_pressure.gp");
    run_gnuplot("plot_density.gp");
    run_gnuplot("plot_sound_speed.gp");
    run_gnuplot("plot_gravity.gp");
    
    std::cout << "Графики сохранены в файлы:\n";
    std::cout << " - temperature_plot.png\n";
    std::cout << " - pressure_plot.png\n";
    std::cout << " - density_plot.png\n";
    std::cout << " - sound_speed_plot.png\n";
    std::cout << " - gravity_plot.png\n";
}

int main() {
    try {
        std::cout << "РАСЧЕТ ПАРАМЕТРОВ АТМОСФЕРЫ ПО ГОСТ 4401-81\n";
        std::cout << "Диапазон высот: от -2000 м до 94000 м\n\n";
        
        // Тестовые высоты для демонстрации
        std::vector<double> test_altitudes = {
            -1000.0, 0.0, 5000.0, 11000.0, 15000.0, 
            25000.0, 35000.0, 45000.0, 50000.0, 
            60000.0, 75000.0, 85000.0, 90000.0
        };
        
        std::cout << "ТАБЛИЦА ПАРАМЕТРОВ АТМОСФЕРЫ:\n";
        print_table_header();
        
        for (double alt : test_altitudes) {
            AtmosphereParams params = calculate_atmosphere(alt);
            print_results(alt, params);
        }
        
        // Запрос пользовательской высоты
        std::cout << "\n\n" << std::string(50, '=') << "\n";
        std::cout << "Введите произвольную высоту от -2000 м до 94000 м: ";
        double custom_altitude;
        std::cin >> custom_altitude;
        
        if (custom_altitude < -2000.0 || custom_altitude > 94000.0) {
            std::cout << "Ошибка: высота должна быть в диапазоне от -2000 м до 94000 м!\n";
            return 1;
        }
        
        AtmosphereParams custom_params = calculate_atmosphere(custom_altitude);
        
        std::cout << "\nРезультаты для высоты " << custom_altitude << " м:\n";
        std::cout << "Геопотенциальная высота: " << custom_params.H << " м\n";
        std::cout << "Температура: " << custom_params.T << " К\n";
        std::cout << "Давление: " << custom_params.p << " Па\n";
        std::cout << "Плотность: " << custom_params.ro << " кг/м³\n";
        std::cout << "Скорость звука: " << custom_params.a << " м/с\n";
        std::cout << "Ускорение свободного падения: " << custom_params.g << " м/с²\n";
        
        // Генерация графиков
        std::cout << "\nХотите сгенерировать графики? (y/n): ";
        char choice;
        std::cin >> choice;
        
        if (choice == 'y' || choice == 'Y') {
            generate_plots();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}