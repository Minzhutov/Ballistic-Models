#include "atmosphere.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <fstream>
#include <cstdlib>


std::string format_with_6_significant(double value) {
    if (value == 0.0) return "0.00000";
    

    int order = (value == 0) ? 0 : floor(log10(fabs(value)));
    

    int precision = 5 - order;
    
    std::stringstream ss;
    
    if (precision < 0) {
        ss << std::fixed << std::setprecision(0) << value;
    } else if (precision > 10) {
        ss << std::scientific << std::setprecision(5) << value;
    } else {
        ss << std::fixed << std::setprecision(precision) << value;
    }
    
    std::string result = ss.str();
    
    size_t dot_pos = result.find('.');
    if (dot_pos != std::string::npos) {
        result = result.erase(result.find_last_not_of('0') + 1);
        if (result.back() == '.') {
            result.pop_back();
        }
    }
    
    return result;
}


void create_gnuplot_graph_vertical(const std::vector<double>& altitudes, 
                                  const std::vector<double>& values,
                                  const std::string& output_filename,
                                  const std::string& title,
                                  const std::string& xlabel,
                                  const std::string& ylabel,
                                  bool logarithmic = false) {
    

    std::string data_filename = "temp_data.dat";
    std::ofstream data_file(data_filename);
    
    for (size_t i = 0; i < altitudes.size(); i++) {
        data_file << values[i] << " " << altitudes[i] << std::endl;
    }
    data_file.close();
    

    std::string script_filename = "temp_script.plt";
    std::ofstream script_file(script_filename);
    
    script_file << "set terminal pngcairo size 1000,1200 enhanced font 'Arial,12'" << std::endl;
    script_file << "set output '" << output_filename << "'" << std::endl;
    script_file << "set title '" << title << "'" << std::endl;
    script_file << "set xlabel '" << xlabel << "'" << std::endl;
    script_file << "set ylabel '" << ylabel << "'" << std::endl;
    script_file << "set grid" << std::endl;
    script_file << "set key top right" << std::endl;
    
    if (logarithmic) {
        script_file << "set logscale x" << std::endl;
    }
    

    script_file << "set yrange [*:*] reverse" << std::endl;
    
    script_file << "plot '" << data_filename << "' with lines linewidth 2 linecolor rgb 'blue' title ''" << std::endl;
    script_file << "quit" << std::endl;
    
    script_file.close();
    

    std::string command = "gnuplot " + script_filename;
    system(command.c_str());
    
    remove(data_filename.c_str());
    remove(script_filename.c_str());
    
    std::cout << "Создан график: " << output_filename << std::endl;
}

void create_all_graphs_vertical() {
    std::cout << std::endl;
    std::cout << "СОЗДАНИЕ ГРАФИКОВ (ВЫСОТА НА ВЕРТИКАЛЬНОЙ ОСИ)" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    std::vector<double> altitudes;
    for (double h = 0; h <= 94000; h += 100) {
        altitudes.push_back(h);
    }

    std::vector<double> temperatures, pressures, densities, sound_speeds, gravities;
    
    for (double h : altitudes) {
        try {
            AtmosphereParams params = calculate_atmosphere(h);
            temperatures.push_back(params.T);
            pressures.push_back(params.p);
            densities.push_back(params.ro);
            sound_speeds.push_back(params.a);
            gravities.push_back(params.g);
        } catch (const std::exception& e) {
        }
    }
    
    // Создаем графики с высотой на вертикальной оси
    create_gnuplot_graph_vertical(altitudes, temperatures, "height_vs_temperature.png",
                                 "Зависимость высоты от температуры (ГОСТ 4401-81)",
                                 "Температура, K", "Высота, м");
    
    create_gnuplot_graph_vertical(altitudes, pressures, "height_vs_pressure.png",
                                 "Зависимость высоты от давления (ГОСТ 4401-81)",
                                 "Давление, Па", "Высота, м", true);
    
    create_gnuplot_graph_vertical(altitudes, densities, "height_vs_density.png",
                                 "Зависимость высоты от плотности (ГОСТ 4401-81)",
                                 "Плотность, кг/м³", "Высота, м", true);
    
    create_gnuplot_graph_vertical(altitudes, sound_speeds, "height_vs_sound_speed.png",
                                 "Зависимость высоты от скорости звука (ГОСТ 4401-81)",
                                 "Скорость звука, м/с", "Высота, м");
    
    create_gnuplot_graph_vertical(altitudes, gravities, "height_vs_gravity.png",
                                 "Зависимость высоты от ускорения свободного падения (ГОСТ 4401-81)",
                                 "Ускорение свободного падения, м/с²", "Высота, м");
}

void create_combined_graph_vertical() {
    std::cout << std::endl;
    
    
    std::vector<double> altitudes;
    for (double h = 0; h <= 94000; h += 100) {
        altitudes.push_back(h);
    }
    
    std::vector<double> temperatures, pressures, densities;
    
    for (double h : altitudes) {
        try {
            AtmosphereParams params = calculate_atmosphere(h);
            temperatures.push_back(params.T);
            pressures.push_back(params.p);
            densities.push_back(params.ro);
        } catch (const std::exception& e) {
            // Пропускаем ошибки
        }
    }
    
    // Создаем временный файл с данными
    std::string data_filename = "temp_combined.dat";
    std::ofstream data_file(data_filename);
    
    for (size_t i = 0; i < altitudes.size(); i++) {
        data_file << temperatures[i] << " " << pressures[i] << " " << densities[i] << " " << altitudes[i] << std::endl;
    }
    data_file.close();
    
    // Создаем скрипт для GNUplot
    std::string script_filename = "temp_combined.plt";
    std::ofstream script_file(script_filename);
    
    script_file << "set terminal pngcairo size 1400,1200 enhanced font 'Arial,12'" << std::endl;
    script_file << "set output 'combined_graph_vertical.png'" << std::endl;
    script_file << "set title 'Параметры стандартной атмосферы ГОСТ 4401-81 (высота на вертикальной оси)'" << std::endl;
    script_file << "set xlabel 'Температура, K / Давление, Па / Плотность, кг/м³'" << std::endl;
    script_file << "set ylabel 'Высота, м'" << std::endl;
    script_file << "set grid" << std::endl;
    script_file << "set key top right" << std::endl;
    script_file << "set logscale x2" << std::endl;
    script_file << "set x2tics" << std::endl;
    

    script_file << "set yrange [*:*] reverse" << std::endl;
    
    script_file << "plot '" << data_filename << "' using 1:4 with lines linewidth 2 linecolor rgb 'red' title 'Температура', \\" << std::endl;
    script_file << "     '" << data_filename << "' using 2:4 with lines linewidth 2 linecolor rgb 'blue' axes x2y1 title 'Давление', \\" << std::endl;
    script_file << "     '" << data_filename << "' using 3:4 with lines linewidth 2 linecolor rgb 'green' axes x2y1 title 'Плотность'" << std::endl;
    script_file << "quit" << std::endl;
    
    script_file.close();
    
    system("gnuplot temp_combined.plt");
    
    remove(data_filename.c_str());
    remove(script_filename.c_str());
    
    std::cout << "Создан совмещенный график: combined_graph_vertical.png" << std::endl;
}

void create_zoomed_graphs_vertical() {
    
    std::vector<double> altitudes_troposphere;
    for (double h = 0; h <= 15000; h += 50) {
        altitudes_troposphere.push_back(h);
    }
    
    std::vector<double> temp_trop, press_trop, dens_trop;
    
    for (double h : altitudes_troposphere) {
        try {
            AtmosphereParams params = calculate_atmosphere(h);
            temp_trop.push_back(params.T);
            press_trop.push_back(params.p);
            dens_trop.push_back(params.ro);
        } catch (const std::exception& e) {
        }
    }
    
    std::string data_filename = "temp_troposphere.dat";
    std::ofstream data_file(data_filename);
    
    for (size_t i = 0; i < altitudes_troposphere.size(); i++) {
        data_file << temp_trop[i] << " " << press_trop[i] << " " << dens_trop[i] << " " << altitudes_troposphere[i] << std::endl;
    }
    data_file.close();
    
    std::string script_filename = "temp_troposphere.plt";
    std::ofstream script_file(script_filename);
    
    script_file << "set terminal pngcairo size 1200,1000 enhanced font 'Arial,12'" << std::endl;
    script_file << "set output 'troposphere_graph_vertical.png'" << std::endl;
    script_file << "set title 'Параметры атмосферы в тропосфере (0-15 км) - высота на вертикальной оси'" << std::endl;
    script_file << "set xlabel 'Температура, K'" << std::endl;
    script_file << "set ylabel 'Высота, м'" << std::endl;
    script_file << "set x2label 'Давление, Па / Плотность, кг/м³'" << std::endl;
    script_file << "set grid" << std::endl;
    script_file << "set key top right" << std::endl;
    script_file << "set logscale x2" << std::endl;
    script_file << "set x2tics" << std::endl;
    
    script_file << "set yrange [*:*] reverse" << std::endl;
    
    script_file << "plot '" << data_filename << "' using 1:4 with lines linewidth 2 linecolor rgb 'red' title 'Температура', \\" << std::endl;
    script_file << "     '" << data_filename << "' using 2:4 with lines linewidth 2 linecolor rgb 'blue' axes x2y1 title 'Давление', \\" << std::endl;
    script_file << "     '" << data_filename << "' using 3:4 with lines linewidth 2 linecolor rgb 'green' axes x2y1 title 'Плотность'" << std::endl;
    script_file << "quit" << std::endl;
    
    script_file.close();
    
    system("gnuplot temp_troposphere.plt");
    remove(data_filename.c_str());
    remove(script_filename.c_str());
    std::cout << "Создан график тропосферы: troposphere_graph_vertical.png" << std::endl;
}

void create_atmospheric_layers_graphs() {
    
    std::vector<std::pair<double, double>> layers = {
        {0, 11000},      // Тропосфера
        {11000, 20000},  // Нижняя стратосфера
        {20000, 32000},  // Средняя стратосфера
        {32000, 47000},  // Верхняя стратосфера
        {47000, 51000},  // Нижняя мезосфера
        {51000, 71000},  // Средняя мезосфера
        {71000, 85000},  // Верхняя мезосфера
        {85000, 94000}   // Термосфера
    };
    
    std::vector<std::string> layer_names = {
        "Тропосфера", "Нижняя стратосфера", "Средняя стратосфера", "Верхняя стратосфера",
        "Нижняя мезосфера", "Средняя мезосфера", "Верхняя мезосфера", "Термосфера"
    };
    
    for (size_t i = 0; i < layers.size(); i++) {
        double h_start = layers[i].first;
        double h_end = layers[i].second;
        
        std::vector<double> altitudes;
        std::vector<double> temperatures;

        double step = (h_end - h_start) / 100.0;
        if (step < 10) step = 10;
        
        for (double h = h_start; h <= h_end; h += step) {
            try {
                AtmosphereParams params = calculate_atmosphere(h);
                altitudes.push_back(h);
                temperatures.push_back(params.T);
            } catch (const std::exception& e) {
            }
        }
        
        if (altitudes.empty()) continue;
        

        std::string data_filename = "temp_layer_" + std::to_string(i) + ".dat";
        std::ofstream data_file(data_filename);
        
        for (size_t j = 0; j < altitudes.size(); j++) {
            data_file << temperatures[j] << " " << altitudes[j] << std::endl;
        }
        data_file.close();
        

        std::string script_filename = "temp_layer_" + std::to_string(i) + ".plt";
        std::ofstream script_file(script_filename);
        
        script_file << "set terminal pngcairo size 800,600 enhanced font 'Arial,10'" << std::endl;
        script_file << "set output 'layer_" << i << "_graph.png'" << std::endl;
        script_file << "set title '" << layer_names[i] << " (" << h_start << "-" << h_end << " м)'" << std::endl;
        script_file << "set xlabel 'Температура, K'" << std::endl;
        script_file << "set ylabel 'Высота, м'" << std::endl;
        script_file << "set grid" << std::endl;
        script_file << "set key top right" << std::endl;
        
        // Инвертируем ось Y чтобы высота увеличивалась снизу вверх
        script_file << "set yrange [" << h_end << ":" << h_start << "]" << std::endl;
        
        script_file << "plot '" << data_filename << "' with lines linewidth 2 linecolor rgb 'red' title 'Температура'" << std::endl;
        script_file << "quit" << std::endl;
        
        script_file.close();
        
        std::string command = "gnuplot " + script_filename;
        system(command.c_str());
        

        remove(data_filename.c_str());
        remove(script_filename.c_str());
        
        std::cout << "Создан график слоя: layer_" << i << "_graph.png (" << layer_names[i] << ")" << std::endl;
    }
}

void print_atmosphere_params(double /*altitude*/, const AtmosphereParams& params) {
    std::cout << "┌────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Высота геометрическая: " << std::setw(14) << format_with_6_significant(params.H_geom) << " м    │" << std::endl;
    std::cout << "│ Высота геопотенциальная: " << std::setw(12) << format_with_6_significant(params.H_geo) << " м    │" << std::endl;
    std::cout << "├────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│ Температура: " << std::setw(20) << format_with_6_significant(params.T - 273.15) << " °C   │" << std::endl;
    std::cout << "│ Давление: " << std::setw(24) << format_with_6_significant(params.p) << " Па  │" << std::endl;
    std::cout << "│ Плотность: " << std::setw(23) << format_with_6_significant(params.ro) << " кг/м³ │" << std::endl;
    std::cout << "│ Скорость звука: " << std::setw(18) << format_with_6_significant(params.a) << " м/с   │" << std::endl;
    std::cout << "│ Ускорение св. падения: " << std::setw(14) << format_with_6_significant(params.g) << " м/с²  │" << std::endl;
    std::cout << "└────────────────────────────────────────────────┘" << std::endl;
    std::cout << std::endl;
}


void create_altitude_table() {
    std::vector<double> altitudes = {
        0, 1000, 2000, 5000, 10000, 11000, 15000, 20000, 
        25000, 32000, 40000, 47000, 51000, 60000, 71000, 
        80000, 85000, 90000, 94000
    };
    
    std::cout << "ТАБЛИЦА СТАНДАРТНОЙ АТМОСФЕРЫ ГОСТ 4401-81" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << std::setw(10) << "H_geom" 
              << std::setw(10) << "H_geo"
              << std::setw(12) << "Темп,°C" 
              << std::setw(15) << "Давл,Па" 
              << std::setw(15) << "Плотн,кг/м³" 
              << std::setw(12) << "Звук,м/с" 
              << std::setw(12) << "g,м/с²" << std::endl;
    std::cout << std::string(86, '-') << std::endl;
    
    for (double alt : altitudes) {
        try {
            AtmosphereParams params = calculate_atmosphere(alt);
            
            std::cout << std::setw(10) << format_with_6_significant(params.H_geom);
            std::cout << std::setw(10) << format_with_6_significant(params.H_geo);
            std::cout << std::setw(12) << format_with_6_significant(params.T - 273.15);
            std::cout << std::setw(15) << format_with_6_significant(params.p);
            std::cout << std::setw(15) << format_with_6_significant(params.ro);
            std::cout << std::setw(12) << format_with_6_significant(params.a);
            std::cout << std::setw(12) << format_with_6_significant(params.g) << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Ошибка для высоты " << alt << " м: " << e.what() << std::endl;
        }
    }
}

int main() {
    std::cout << "СТАНДАРТНАЯ АТМОСФЕРА ГОСТ 4401-81" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;
    
    create_altitude_table();
    

    create_all_graphs_vertical();
    create_combined_graph_vertical();
    create_zoomed_graphs_vertical();
    create_atmospheric_layers_graphs();
    
    std::cout << std::endl;
    std::cout << "ВСЕ ГРАФИКИ СОЗДАНЫ В ФАЙЛАХ PNG!" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "Созданные файлы (высота на вертикальной оси):" << std::endl;
    std::cout << "- height_vs_temperature.png" << std::endl;
    std::cout << "- height_vs_pressure.png" << std::endl;
    std::cout << "- height_vs_density.png" << std::endl;
    std::cout << "- height_vs_sound_speed.png" << std::endl;
    std::cout << "- height_vs_gravity.png" << std::endl;
    std::cout << "- combined_graph_vertical.png" << std::endl;
    std::cout << "- troposphere_graph_vertical.png" << std::endl;
    std::cout << "- layer_0_graph.png ... layer_7_graph.png (отдельные слои атмосферы)" << std::endl;
    
    std::cout << std::endl;
    std::cout << "ИНТЕРАКТИВНЫЙ РЕЖИМ" << std::endl;
    std::cout << "===================" << std::endl;
    std::cout << "Введите высоту в метрах (или 'q' для выхода): ";
    
    double altitude;
    while (std::cin >> altitude) {
        try {
            AtmosphereParams params = calculate_atmosphere(altitude);
            print_atmosphere_params(altitude, params);
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
        std::cout << "Введите следующую высоту (или 'q' для выхода): ";
    }
    
    std::cout << "Завершение работы программы." << std::endl;
    return 0;
}