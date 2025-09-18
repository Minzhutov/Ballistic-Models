#include <iostream>
#include <iomanip>
#include "atmosphere.h"

void print_results(double altitude, const AtmosphereParams& params) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Высота: " << std::setw(8) << altitude << " м | ";
    std::cout << "T: " << std::setw(6) << params.T << " К | ";
    std::cout << "p: " << std::setw(9) << params.p << " Па | ";
    std::cout << "ro: " << std::setw(8) << params.ro << " кг/м³ | ";
    std::cout << "a: " << std::setw(5) << params.a << " м/с | ";
    std::cout << "g: " << std::setw(5) << params.g << " м/с²\n";
}

int main() {
    try {
        std::cout << "Расчет параметров атмосферы по ГОСТ 4401-81\n";
        std::cout << "Диапазон высот: от -2000 м до 94000 м\n\n";
        
        double test_altitudes[] = {
            -1000.0, 0.0, 5000.0, 11000.0, 15000.0, 
            25000.0, 35000.0, 45000.0, 50000.0, 
            60000.0, 75000.0, 85000.0, 90000.0
        };
        
        std::cout << "Результаты расчета для различных высот:\n";
        std::cout << "============================================================================================================\n";
        
        for (double alt : test_altitudes) {
            AtmosphereParams params = calculate_atmosphere(alt);
            print_results(alt, params);
        }
        
        std::cout << "\nВысота: ";
        double custom_altitude;
        std::cin >> custom_altitude;
        
        AtmosphereParams custom_params = calculate_atmosphere(custom_altitude);
        std::cout << "\nРезультаты для высоты " << custom_altitude << " м:\n";
        std::cout << "============================================================================================================\n";
        print_results(custom_altitude, custom_params);
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}