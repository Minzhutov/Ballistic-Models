import numpy as np
import matplotlib.pyplot as plt
import matplotlib
from matplotlib import rc
from matplotlib.patches import FancyArrowPatch
import io

# Настройка шрифтов для поддержки русского языка
matplotlib.rcParams['font.sans-serif'] = ['DejaVu Sans']
matplotlib.rcParams['font.family'] = 'sans-serif'
matplotlib.rcParams['axes.unicode_minus'] = False

# Табличные данные из ГОСТ для Cxa(M)
M_table = [0.01, 0.55, 0.8, 0.9, 1.0, 1.06, 1.1, 1.2, 1.3, 1.4, 2.0, 2.6, 3.4, 6.0, 10.2]
Cxa_table = [0.30, 0.30, 0.55, 0.70, 0.84, 0.86, 0.87, 0.83, 0.80, 0.79, 0.65, 0.55, 0.50, 0.45, 0.41]

# Табличные данные из ГОСТ для Cya^alpha(M)
Cya_alpha_table = [0.25, 0.25, 0.25, 0.20, 0.30, 0.31, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25]

# Функция линейной интерполяции (общая)
def interpolate_linear(x, x_vals, y_vals):
    """Линейная интерполяция между точками"""
    if x <= x_vals[0]:
        return y_vals[0]
    if x >= x_vals[-1]:
        return y_vals[-1]
    
    for i in range(len(x_vals) - 1):
        if x_vals[i] <= x <= x_vals[i+1]:
            t = (x - x_vals[i]) / (x_vals[i+1] - x_vals[i])
            return y_vals[i] + t * (y_vals[i+1] - y_vals[i])
    return y_vals[-1]

def interpolate_Cxa(M):
    """Интерполяция Cxa(M)"""
    return interpolate_linear(M, M_table, Cxa_table)

def interpolate_Cya_alpha(M):
    """Интерполяция Cya^alpha(M)"""
    return interpolate_linear(M, M_table, Cya_alpha_table)

# ============================================================================
# 1. ГРАФИК ДЛЯ Cxa(M) - как в предыдущей программе
# ============================================================================
print("="*70)
print("АНАЛИЗ ЗАВИСИМОСТИ Cₓₐ(M) - Коэффициент лобового сопротивления")
print("="*70)

# Создаем гладкую кривую для интерполяции
M_smooth = np.linspace(0.01, 10.2, 500)
Cxa_smooth = [interpolate_Cxa(m) for m in M_smooth]

# Создаем график
fig1, ax1 = plt.subplots(figsize=(14, 9))

# Основной график интерполированной функции
ax1.plot(M_smooth, Cxa_smooth, 'b-', linewidth=3, label='Интерполирующая функция $C_{xa}(M)$')

# Точки табличных данных
ax1.plot(M_table, Cxa_table, 'ro', markersize=10, markerfacecolor='red', 
        markeredgecolor='black', markeredgewidth=2, label='Табличные данные ГОСТ')

# Вертикальные линии для выделения интервалов
ax1.axvline(x=0.8, color='gray', linestyle='--', alpha=0.5, linewidth=1)
ax1.axvline(x=1.2, color='gray', linestyle='--', alpha=0.5, linewidth=1)
ax1.axvline(x=1.0, color='green', linestyle=':', alpha=0.7, linewidth=1)

# Подписи областей
ax1.text(0.4, 0.85, 'Дозвуковая область\nM < 0.8', fontsize=11, 
        ha='center', va='center', bbox=dict(boxstyle="round,pad=0.3", facecolor="lightblue", alpha=0.7))
ax1.text(1.0, 0.9, 'Трансзвуковая область\n0.8 < M < 1.2', fontsize=11, 
        ha='center', va='center', bbox=dict(boxstyle="round,pad=0.3", facecolor="lightyellow", alpha=0.7))
ax1.text(5.0, 0.6, 'Сверхзвуковая область\nM > 1.2', fontsize=11, 
        ha='center', va='center', bbox=dict(boxstyle="round,pad=0.3", facecolor="lightgreen", alpha=0.7))

# Настройка осей
ax1.set_xlabel('Число Маха, M', fontsize=14)
ax1.set_ylabel('Коэффициент лобового сопротивления, $C_{xa}$', fontsize=14)
ax1.set_title('Зависимость коэффициента лобового сопротивления от числа Маха $C_{xa}(M)$', 
             fontsize=16, fontweight='bold', pad=20)

# Сетка
ax1.grid(True, which='both', linestyle='--', alpha=0.3)
ax1.set_axisbelow(True)

# Легенда
ax1.legend(loc='upper right', fontsize=12, framealpha=0.9)

# Установка пределов осей
ax1.set_xlim(0, 11)
ax1.set_ylim(0.25, 0.95)

# Добавление вторичной оси с характерными скоростями
def M_to_V(M):
    a = 340.29  # скорость звука на уровне моря, м/с
    return M * a

secax = ax1.secondary_xaxis('top', functions=(M_to_V, lambda V: V/340.29))
secax.set_xlabel('Скорость, м/с', fontsize=12)
secax.set_xticks([0, 340.29, 680.58, 1020.87, 1361.16])
secax.set_xticklabels(['0', '340\n(звук)', '680', '1020', '1360'])

# Добавление таблицы с данными в углу
table_data_cxa = []
for i, (M, Cxa) in enumerate(zip(M_table, Cxa_table)):
    if i % 4 == 0 or i == len(M_table)-1:  # Каждая 4-я точка и последняя
        table_data_cxa.append([f'{M:.2f}', f'{Cxa:.2f}'])

#table_text_cxa = 'Таблица 1 (выборка):\n' + '\n'.join([f'M={row[0]}, Cₓₐ={row[1]}' for row in table_data_cxa])
ax1.text(0.02, 0.02, table_text_cxa, transform=ax1.transAxes, fontsize=9,
        verticalalignment='bottom', bbox=dict(boxstyle="round,pad=0.5", facecolor="white", alpha=0.8))

plt.tight_layout()

# Сохранение графика Cxa(M)
plt.savefig('Cxa_M_dependency.png', dpi=300, bbox_inches='tight')
plt.savefig('Cxa_M_dependency.pdf', bbox_inches='tight')

# Отображение первого графика
plt.show()

# ============================================================================
# 2. ГРАФИК ДЛЯ Cya^alpha(M) - НОВЫЙ ГРАФИК
# ============================================================================
print("\n" + "="*70)
print("АНАЛИЗ ЗАВИСИМОСТИ $C_{ya}^{\\alpha}(M)$ - Производная коэффициента подъемной силы")
print("="*70)

# Создаем гладкую кривую для Cya^alpha
Cya_smooth = [interpolate_Cya_alpha(m) for m in M_smooth]

# Создаем график для Cya^alpha
fig2, ax2 = plt.subplots(figsize=(14, 9))

# Основной график интерполированной функции
ax2.plot(M_smooth, Cya_smooth, 'g-', linewidth=3, label='Интерполирующая функция $C_{ya}^{\\alpha}(M)$')

# Точки табличных данных
ax2.plot(M_table, Cya_alpha_table, 'bs', markersize=10, markerfacecolor='blue', 
        markeredgecolor='black', markeredgewidth=2, label='Табличные данные ГОСТ')

# Вертикальные линии для выделения особых интервалов
ax2.axvline(x=0.9, color='orange', linestyle='--', alpha=0.7, linewidth=1.5)
ax2.axvline(x=1.0, color='orange', linestyle='--', alpha=0.7, linewidth=1.5)
ax2.axvline(x=1.06, color='orange', linestyle='--', alpha=0.7, linewidth=1.5)

# Горизонтальная линия для среднего значения
mean_Cya = np.mean(Cya_alpha_table)
ax2.axhline(y=mean_Cya, color='red', linestyle=':', alpha=0.5, linewidth=1, 
           label=f'Среднее значение = {mean_Cya:.3f}')

# Подписи областей
ax2.text(0.4, 0.26, 'Стабильная область\n$C_{ya}^{\\alpha} = 0.25$', fontsize=11, 
        ha='center', va='center', bbox=dict(boxstyle="round,pad=0.3", facecolor="lightblue", alpha=0.7))
ax2.text(0.95, 0.26, 'Трансзвуковая\nаномалия', fontsize=11, 
        ha='center', va='center', bbox=dict(boxstyle="round,pad=0.3", facecolor="lightyellow", alpha=0.7))
ax2.text(5.0, 0.26, 'Сверхзвуковая\nстабильность', fontsize=11, 
        ha='center', va='center', bbox=dict(boxstyle="round,pad=0.3", facecolor="lightgreen", alpha=0.7))

# Настройка осей
ax2.set_xlabel('Число Маха, M', fontsize=14)
ax2.set_ylabel('Производная коэффициента подъемной силы, $C_{ya}^{\\alpha}$', fontsize=14)
ax2.set_title('Зависимость производной коэффициента подъемной силы от числа Маха $C_{ya}^{\\alpha}(M)$', 
             fontsize=16, fontweight='bold', pad=20)

# Сетка
ax2.grid(True, which='both', linestyle='--', alpha=0.3)
ax2.set_axisbelow(True)

# Легенда
ax2.legend(loc='upper right', fontsize=12, framealpha=0.9)

# Установка пределов осей
ax2.set_xlim(0, 11)
ax2.set_ylim(0.18, 0.34)

# Добавление стрелок для объяснения изменений
arrow1 = FancyArrowPatch((0.9, 0.25), (0.9, 0.20), 
                        arrowstyle='->', mutation_scale=20, color='darkred', linewidth=2)
ax2.add_patch(arrow1)
ax2.text(0.85, 0.225, 'Снижение\nна 20%', fontsize=10, ha='right', 
        bbox=dict(boxstyle="round,pad=0.2", facecolor="white", alpha=0.8))

arrow2 = FancyArrowPatch((1.0, 0.25), (1.0, 0.30), 
                        arrowstyle='->', mutation_scale=20, color='darkgreen', linewidth=2)
ax2.add_patch(arrow2)
ax2.text(1.05, 0.275, 'Рост\nна 20%', fontsize=10, ha='left',
        bbox=dict(boxstyle="round,pad=0.2", facecolor="white", alpha=0.8))

arrow3 = FancyArrowPatch((1.06, 0.31), (1.1, 0.25), 
                        arrowstyle='->', mutation_scale=20, color='darkred', linewidth=2)
ax2.add_patch(arrow3)
ax2.text(1.15, 0.28, 'Возврат\nк норме', fontsize=10, ha='left',
        bbox=dict(boxstyle="round,pad=0.2", facecolor="white", alpha=0.8))

# Добавление таблицы с данными
table_data_cya = []
for i, (M, Cya) in enumerate(zip(M_table, Cya_alpha_table)):
    if i % 4 == 0 or i in [3, 4, 5, 6]:  # Каждая 4-я точка + точки аномалии
        table_data_cya.append([f'{M:.2f}', f'{Cya:.2f}'])

table_text_cya = 'Ключевые точки:\n' + '\n'.join([f'M={row[0]}, $C_{{ya}}^{{\\alpha}}$={row[1]}' for row in table_data_cya])
ax2.text(0.12, 0.02, table_text_cya, transform=ax2.transAxes, fontsize=9,
        verticalalignment='bottom', bbox=dict(boxstyle="round,pad=0.5", facecolor="white", alpha=0.8))

plt.tight_layout()

# Сохранение графика Cya^alpha(M)
plt.savefig('Cya_alpha_M_dependency.png', dpi=300, bbox_inches='tight')
plt.savefig('Cya_alpha_M_dependency.pdf', bbox_inches='tight')

# Отображение второго графика
plt.show()

# ============================================================================
# 3. ОБЪЕДИНЕННЫЙ ГРАФИК ДЛЯ СРАВНЕНИЯ
# ============================================================================
print("\n" + "="*70)
print("СРАВНИТЕЛЬНЫЙ АНАЛИЗ Cₓₐ(M) и $C_{ya}^{\\alpha}(M)$")
print("="*70)

fig3, (ax3_1, ax3_2) = plt.subplots(2, 1, figsize=(14, 12))

# Верхний график - Cxa(M)
ax3_1.plot(M_smooth, Cxa_smooth, 'b-', linewidth=2.5, label='$C_{xa}(M)$')
ax3_1.plot(M_table, Cxa_table, 'ro', markersize=8, markerfacecolor='red', 
          markeredgecolor='black', markeredgewidth=1.5)
ax3_1.set_ylabel('$C_{xa}$', fontsize=14)
ax3_1.set_title('Сравнение аэродинамических коэффициентов', fontsize=16, fontweight='bold')
ax3_1.grid(True, alpha=0.3)
ax3_1.legend(loc='upper right', fontsize=12)
ax3_1.set_xlim(0, 11)
ax3_1.set_ylim(0.25, 0.95)

# Нижний график - Cya^alpha(M)
ax3_2.plot(M_smooth, Cya_smooth, 'g-', linewidth=2.5, label='$C_{ya}^{\\alpha}(M)$')
ax3_2.plot(M_table, Cya_alpha_table, 'bs', markersize=8, markerfacecolor='blue', 
          markeredgecolor='black', markeredgewidth=1.5)
ax3_2.set_xlabel('Число Маха, M', fontsize=14)
ax3_2.set_ylabel('$C_{ya}^{\\alpha}$', fontsize=14)
ax3_2.grid(True, alpha=0.3)
ax3_2.legend(loc='upper right', fontsize=12)
ax3_2.set_xlim(0, 11)
ax3_2.set_ylim(0.18, 0.34)

# Вертикальные линии для синхронизации областей
for ax in [ax3_1, ax3_2]:
    ax.axvline(x=0.8, color='gray', linestyle='--', alpha=0.3, linewidth=0.5)
    ax.axvline(x=1.2, color='gray', linestyle='--', alpha=0.3, linewidth=0.5)
    ax.axvline(x=0.9, color='orange', linestyle='--', alpha=0.3, linewidth=0.5)
    ax.axvline(x=1.06, color='orange', linestyle='--', alpha=0.3, linewidth=0.5)

plt.tight_layout()
plt.savefig('Cxa_Cya_comparison.png', dpi=300, bbox_inches='tight')
plt.show()

# ============================================================================
# 4. ВИЗУАЛИЗАЦИЯ АЛГОРИТМА ЛИНЕЙНОЙ ИНТЕРПОЛЯЦИИ
# ============================================================================
print("\n" + "="*70)
print("ДЕМОНСТРАЦИЯ АЛГОРИТМА ЛИНЕЙНОЙ ИНТЕРПОЛЯЦИИ")
print("="*70)

# Выберем конкретный интервал для демонстрации
demo_M = 0.95  # Точка для интерполяции (между 0.9 и 1.0)
demo_idx = 3   # Индекс начала интервала [0.9, 1.0]

# Получаем границы интервала
M1, M2 = M_table[demo_idx], M_table[demo_idx+1]
Cya1, Cya2 = Cya_alpha_table[demo_idx], Cya_alpha_table[demo_idx+1]

# Выполняем интерполяцию
t = (demo_M - M1) / (M2 - M1)
Cya_interp = Cya1 + t * (Cya2 - Cya1)

# Создаем демонстрационный график
fig4, ax4 = plt.subplots(figsize=(12, 8))

# Рисуем точки интервала
ax4.plot([M1, M2], [Cya1, Cya2], 'bo-', linewidth=3, markersize=12, 
        label=f'Интервал интерполяции: M=[{M1}, {M2}]')

# Рисуем точку интерполяции
ax4.plot(demo_M, Cya_interp, 'rs', markersize=15, markerfacecolor='red',
        markeredgecolor='black', markeredgewidth=2, 
        label=f'Интерполированная точка: M={demo_M:.2f}, $C_{{ya}}^{{\\alpha}}$={Cya_interp:.3f}')

# Вспомогательные линии для наглядности
ax4.plot([demo_M, demo_M], [0, Cya_interp], 'r--', alpha=0.5, linewidth=1)
ax4.plot([0, demo_M], [Cya_interp, Cya_interp], 'r--', alpha=0.5, linewidth=1)
ax4.plot([M1, demo_M], [Cya1, Cya_interp], 'g--', alpha=0.7, linewidth=1.5)
ax4.plot([demo_M, M2], [Cya_interp, Cya2], 'g--', alpha=0.7, linewidth=1.5)

# Подписи
ax4.text(M1, Cya1-0.01, f'({M1}, {Cya1})', fontsize=12, ha='center', 
        bbox=dict(boxstyle="round,pad=0.3", facecolor="white", alpha=0.8))
ax4.text(M2, Cya2+0.01, f'({M2}, {Cya2})', fontsize=12, ha='center',
        bbox=dict(boxstyle="round,pad=0.3", facecolor="white", alpha=0.8))
ax4.text(demo_M, Cya_interp+0.01, f'({demo_M:.2f}, {Cya_interp:.3f})', fontsize=12, ha='center',
        bbox=dict(boxstyle="round,pad=0.3", facecolor="yellow", alpha=0.8))

# Формула интерполяции
formula_text = f'Формула линейной интерполяции:\n'
formula_text += f'$C_{{ya}}^{{\\alpha}}(M) = C_1 + \\frac{{C_2 - C_1}}{{M_2 - M_1}} \\cdot (M - M_1)$\n'
formula_text += f'где $M_1={M1}$, $M_2={M2}$, $C_1={Cya1}$, $C_2={Cya2}$, $M={demo_M:.2f}$\n'
formula_text += f'$C_{{ya}}^{{\\alpha}}({demo_M:.2f}) = {Cya1} + \\frac{{{Cya2}-{Cya1}}}{{{M2}-{M1}}} \\cdot ({demo_M:.2f}-{M1})$'
formula_text += f'$ = {Cya_interp:.3f}$'

ax4.text(0.02, 0.98, formula_text, transform=ax4.transAxes, fontsize=11,
        verticalalignment='top', bbox=dict(boxstyle="round,pad=0.5", facecolor="lightyellow", alpha=0.9))

# Настройка графика
ax4.set_xlabel('Число Маха, M', fontsize=14)
ax4.set_ylabel('$C_{ya}^{\\alpha}$', fontsize=14)
ax4.set_title('Демонстрация алгоритма линейной интерполяции $C_{ya}^{\\alpha}(M)$', 
             fontsize=16, fontweight='bold', pad=20)
ax4.grid(True, alpha=0.3)
ax4.legend(loc='lower right', fontsize=12)
ax4.set_xlim(0.85, 1.05)
ax4.set_ylim(0.18, 0.32)

plt.tight_layout()
plt.savefig('linear_interpolation_demo.png', dpi=300, bbox_inches='tight')
plt.show()

# ============================================================================
# 5. ВЫВОД АЛГОРИТМА В ПСЕВДОКОДЕ И РЕАЛЬНОМ КОДЕ
# ============================================================================
print("\n" + "="*70)
print("АЛГОРИТМ ЛИНЕЙНОЙ ИНТЕРПОЛЯЦИИ $C_{ya}^{\\alpha}(M)$")
print("="*70)

print("\n1. ПСЕВДОКОД АЛГОРИТМА:")
print("-"*40)
pseudocode = """
ФУНКЦИЯ interpolate_Cya_alpha(M):
    ВХОД: M - число Маха
    ВЫХОД: Cya - значение коэффициента
    
    ЕСЛИ M <= M_min ТО:
        ВЕРНУТЬ Cya_min
    КОНЕЦ ЕСЛИ
    
    ЕСЛИ M >= M_max ТО:
        ВЕРНУТЬ Cya_max
    КОНЕЦ ЕСЛИ
    
    ДЛЯ i ОТ 0 ДО (n-2):
        ЕСЛИ M_table[i] <= M <= M_table[i+1] ТО:
            t = (M - M_table[i]) / (M_table[i+1] - M_table[i])
            Cya = Cya_table[i] + t * (Cya_table[i+1] - Cya_table[i])
            ВЕРНУТЬ Cya
        КОНЕЦ ЕСЛИ
    КОНЕЦ ДЛЯ
    
    ВЕРНУТЬ Cya_table[последний]
КОНЕЦ ФУНКЦИИ
"""
print(pseudocode)

print("\n2. РЕАЛИЗАЦИЯ НА PYTHON:")
print("-"*40)
python_code = """
def interpolate_Cya_alpha(M):
    # Табличные данные
    M_table = [0.01, 0.55, 0.8, 0.9, 1.0, 1.06, 1.1, 1.2, 
               1.3, 1.4, 2.0, 2.6, 3.4, 6.0, 10.2]
    Cya_table = [0.25, 0.25, 0.25, 0.20, 0.30, 0.31, 0.25,
                 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25]
    
    # Проверка граничных условий
    if M <= M_table[0]:
        return Cya_table[0]
    if M >= M_table[-1]:
        return Cya_table[-1]
    
    # Поиск интервала интерполяции
    for i in range(len(M_table) - 1):
        if M_table[i] <= M <= M_table[i+1]:
            # Линейная интерполяция
            t = (M - M_table[i]) / (M_table[i+1] - M_table[i])
            return Cya_table[i] + t * (Cya_table[i+1] - Cya_table[i])
    
    return Cya_table[-1]  # Запасной вариант
"""
print(python_code)

print("\n3. РЕАЛИЗАЦИЯ НА C++:")
print("-"*40)
cpp_code = """
double interpolate_Cya_alpha(double M) {
    // Табличные данные
    std::vector<double> M_table = {0.01, 0.55, 0.8, 0.9, 1.0, 1.06, 1.1, 1.2,
                                   1.3, 1.4, 2.0, 2.6, 3.4, 6.0, 10.2};
    std::vector<double> Cya_table = {0.25, 0.25, 0.25, 0.20, 0.30, 0.31, 0.25,
                                     0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25};
    
    // Проверка границ
    if (M <= M_table.front()) return Cya_table.front();
    if (M >= M_table.back()) return Cya_table.back();
    
    // Поиск интервала
    for (size_t i = 0; i < M_table.size() - 1; ++i) {
        if (M >= M_table[i] && M <= M_table[i+1]) {
            // Линейная интерполяция
            double t = (M - M_table[i]) / (M_table[i+1] - M_table[i]);
            return Cya_table[i] + t * (Cya_table[i+1] - Cya_table[i]);
        }
    }
    
    return Cya_table.back();  // Запасной вариант
}
"""
print(cpp_code)

# ============================================================================
# 6. АНАЛИЗ И ВЫВОД СТАТИСТИКИ
# ============================================================================
print("\n" + "="*70)
print("СТАТИСТИЧЕСКИЙ АНАЛИЗ $C_{ya}^{\\alpha}(M)$")
print("="*70)

print("\n1. Основные статистические характеристики:")
print("-"*40)
print(f"Минимальное значение: {min(Cya_alpha_table):.3f} (при M={M_table[Cya_alpha_table.index(min(Cya_alpha_table))]})")
print(f"Максимальное значение: {max(Cya_alpha_table):.3f} (при M={M_table[Cya_alpha_table.index(max(Cya_alpha_table))]})")
print(f"Среднее значение: {np.mean(Cya_alpha_table):.3f}")
print(f"Стандартное отклонение: {np.std(Cya_alpha_table):.3f}")
print(f"Медиана: {np.median(Cya_alpha_table):.3f}")

print("\n2. Анализ изменений по интервалам:")
print("-"*40)
for i in range(len(M_table)-1):
    M1, M2 = M_table[i], M_table[i+1]
    C1, C2 = Cya_alpha_table[i], Cya_alpha_table[i+1]
    if C1 != C2:
        change_percent = abs(C2 - C1) / C1 * 100
        direction = "увеличение" if C2 > C1 else "уменьшение"
        print(f"Интервал [{M1:.2f}, {M2:.2f}]: {direction} на {abs(C2-C1):.3f} ({change_percent:.1f}%)")

print("\n3. Анализ стабильности:")
print("-"*40)
stable_intervals = []
current_interval = [M_table[0], M_table[0]]
current_value = Cya_alpha_table[0]

for i in range(1, len(M_table)):
    if Cya_alpha_table[i] == current_value:
        current_interval[1] = M_table[i]
    else:
        if current_interval[1] > current_interval[0]:
            stable_intervals.append((current_interval.copy(), current_value))
        current_interval = [M_table[i], M_table[i]]
        current_value = Cya_alpha_table[i]

# Добавляем последний интервал
if current_interval[1] > current_interval[0]:
    stable_intervals.append((current_interval, current_value))

print("Интервалы постоянства $C_{ya}^{\\alpha}$:")
for interval, value in stable_intervals:
    if interval[1] - interval[0] > 0.1:  # Показываем только значимые интервалы
        print(f"  M ∈ [{interval[0]:.2f}, {interval[1]:.2f}]: $C_{{ya}}^{{\\alpha}}$ = {value:.2f}")

print("\n4. Особенности функции $C_{ya}^{\\alpha}(M)$:")
print("-"*40)
print("1. В основном постоянная величина: $C_{ya}^{\\alpha} = 0.25$")
print("2. Трансзвуковая аномалия при M ≈ 0.9-1.1:")
print("   - Снижение до 0.20 при M = 0.9")
print("   - Рост до 0.31 при M = 1.06 (максимум)")
print("   - Возврат к 0.25 при M > 1.1")
print("3. Стабильность в сверхзвуковой области (M > 1.2)")
print("4. Физическая интерпретация: изменения связаны с")
print("   - Изменением распределения давления")
print("   - Образованием скачков уплотнения")
print("   - Изменением центра давления")

# ============================================================================
# 7. ПРОВЕРКА ТОЧНОСТИ ИНТЕРПОЛЯЦИИ
# ============================================================================
print("\n" + "="*70)
print("ПРОВЕРКА ТОЧНОСТИ ЛИНЕЙНОЙ ИНТЕРПОЛЯЦИИ")
print("="*70)

# Тестовые точки для проверки
test_points = [0.5, 0.95, 1.03, 2.5, 5.0, 8.0]

print("\nТестовые расчеты:")
print("-"*40)
print(f"{'M':>8} | {'Cya табличное':>14} | {'Cya интерпол.':>14} | {'Погрешность':>12} | {'Отн. погр., %':>12}")
print("-"*70)

for M_test in test_points:
    # "Точное" значение (ближайшее табличное)
    exact_idx = min(range(len(M_table)), key=lambda i: abs(M_table[i] - M_test))
    exact_value = Cya_alpha_table[exact_idx]
    
    # Интерполированное значение
    interp_value = interpolate_Cya_alpha(M_test)
    
    # Погрешность
    error = abs(interp_value - exact_value)
    rel_error = error / exact_value * 100 if exact_value != 0 else 0
    
    print(f"{M_test:>8.2f} | {exact_value:>14.3f} | {interp_value:>14.3f} | {error:>12.3e} | {rel_error:>12.1f}")

print("\n" + "="*70)
print("ПРОГРАММА ЗАВЕРШЕНА")
print("="*70)
print("\nСозданные файлы:")
print("1. Cxa_M_dependency.png/pdf - график Cₓₐ(M)")
print("2. Cya_alpha_M_dependency.png/pdf - график $C_{ya}^{\\alpha}(M)$")
print("3. Cxa_Cya_comparison.png - сравнительный график")
print("4. linear_interpolation_demo.png - демонстрация алгоритма")
print("\nВсе графики готовы для включения в отчет.")