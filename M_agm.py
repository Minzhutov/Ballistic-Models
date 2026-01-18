import math

def equation(M, di, k=1.25, d_star=0.148):
    # Левая часть уравнения
    term1 = ((k + 1) / 2) ** ((k + 1) / (2 * (k - 1)))
    term2 = (1 + (k - 1) / 2 * M**2) ** (-(k + 1) / (2 * (k - 1)))
    left_side = M * term1 * term2
    
    # Правая часть уравнения
    right_side = (d_star / di) ** 2
    
    return left_side - right_side

def find_root_bisection(di, a=0.01, b=5.0, tol=1e-10, max_iter=1000):
    """
    Находит корень уравнения методом бисекции на интервале [a, b]
    """
    f_a = equation(a, di)
    f_b = equation(b, di)
    
    # Проверяем, что на концах интервала функция имеет разные знаки
    if f_a * f_b > 0:
        # Пробуем найти другой интервал
        for test_b in [0.1, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 10.0]:
            for test_a in [0.01, 0.05, 0.1]:
                if test_a < test_b:
                    f_a_test = equation(test_a, di)
                    f_b_test = equation(test_b, di)
                    if f_a_test * f_b_test <= 0:
                        a, b = test_a, test_b
                        f_a, f_b = f_a_test, f_b_test
                        break
            if f_a * f_b <= 0:
                break
    
    if f_a * f_b > 0:
        return None
    
    # Метод бисекции
    for i in range(max_iter):
        c = (a + b) / 2
        f_c = equation(c, di)
        
        if abs(f_c) < tol or (b - a) / 2 < tol:
            return c
        
        if f_a * f_c <= 0:
            b = c
            f_b = f_c
        else:
            a = c
            f_a = f_c
    
    return (a + b) / 2

def find_M_roots(di):
    """
    Ищет возможные корни M в различных интервалах
    """
    roots = []
    
    # Проверяем различные интервалы для поиска корней
    intervals = [
        (0.01, 0.99),   # Дозвуковой режим (M < 1)
        (1.01, 10.0)    # Сверхзвуковой режим (M > 1)
    ]
    
    for a, b in intervals:
        root = find_root_bisection(di, a, b)
        if root is not None:
            # Проверяем, не нашли ли мы уже этот корень (с небольшой погрешностью)
            is_duplicate = False
            for existing_root in roots:
                if abs(existing_root - root) < 0.001:
                    is_duplicate = True
                    break
            
            if not is_duplicate:
                # Проверяем точность решения
                error = abs(equation(root, di))
                if error < 1e-6:
                    roots.append(root)
    
    return sorted(roots)

def main():
    print("Решение уравнения для параметров газовой динамики")
    print("M * ((k+1)/2)^((k+1)/(2*(k-1))) * (1 + (k-1)/2 * M^2)^(-(k+1)/(2*(k-1))) = (d*/di)^2")
    print("k = 1.25, d* = 0.1517")
    print("-" * 70)
    
    while True:
        try:
            user_input = input("Введите значение di (или 'q' для выхода): ").strip()
            
            if user_input.lower() == 'q':
                print("Выход из программы.")
                break
            
            di = float(user_input)
            
            if di <= 0:
                print("Ошибка: di должен быть положительным числом!")
                continue
            
            # Находим корни
            roots = find_M_roots(di)
            
            print(f"\nРезультаты для di = {di}:")
            print(f"Правая часть уравнения: {(0.1517 / di) ** 2:.10f}")
            
            if roots:
                for i, M in enumerate(roots, 1):
                    # Вычисляем левую часть для проверки
                    k = 1.25
                    d_star = 0.1517
                    term1 = ((k + 1) / 2) ** ((k + 1) / (2 * (k - 1)))
                    term2 = (1 + (k - 1) / 2 * M**2) ** (-(k + 1) / (2 * (k - 1)))
                    left_value = M * term1 * term2
                    right_value = (d_star / di) ** 2
                    error = abs(left_value - right_value)
                    
                    regime = "дозвуковой" if M < 1 else "сверхзвуковой"
                    print(f"  Решение {i}:")
                    print(f"    M = {M:.8f} ({regime} режим)")
                    print(f"    Левая часть: {left_value:.10f}")
                    print(f"    Погрешность: {error:.2e}")
            else:
                print("  Корни не найдены. Попробуйте другое значение di.")
            
            print("-" * 70)
            
        except ValueError:
            print("Ошибка: введите корректное число!")
        except KeyboardInterrupt:
            print("\n\nПрограмма прервана пользователем.")
            break

if __name__ == "__main__":
    main()