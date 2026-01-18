import matplotlib
# Устанавливаем бэкенд для работы без GUI
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
import os
import sys
import glob

def plot_graph(data_file, x_col, y_col, x_label, y_label, title, output_file):
    """Построение одного графика из файла данных"""
    try:
        print(f"Обработка файла: {data_file}")
        
        # Проверяем существование файла
        if not os.path.exists(data_file):
            print(f"  Файл не найден: {data_file}")
            return False
            
        data = np.loadtxt(data_file, skiprows=1)
        
        # Если данные одномерные, преобразуем в двумерные
        if data.ndim == 1:
            data = data.reshape(-1, 2)
        
        # Проверяем, что данные не пустые
        if len(data) == 0:
            print(f"  Файл пуст: {data_file}")
            return False
            
        plt.figure(figsize=(10, 6))
        plt.plot(data[:, x_col], data[:, y_col], 'b-', linewidth=2)
        plt.xlabel(x_label, fontsize=12)
        plt.ylabel(y_label, fontsize=12)
        plt.title(title, fontsize=14)
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        
        # Создаем директорию для выходных файлов, если её нет
        output_dir = os.path.dirname(output_file)
        if output_dir and not os.path.exists(output_dir):
            os.makedirs(output_dir)
            
        plt.savefig(output_file, dpi=300)
        plt.close()
        print(f"  ✓ График сохранен: {output_file}")
        return True
    except Exception as e:
        print(f"  ✗ Ошибка при построении графика {data_file}: {e}")
        import traceback
        traceback.print_exc()
        return False

def plot_all_graphs(base_filename):
    """Построение всех графиков для одного расчета"""
    print(f"\nПостроение графиков для: {base_filename}")
    
    # Получаем базовое имя без расширения
    if '_graph_' in base_filename:
        base_name = os.path.basename(base_filename).replace('_graph', '')
    else:
        base_name = os.path.basename(base_filename)
    
    # Определяем директории
    data_dir = os.path.dirname(base_filename)
    if not data_dir:
        data_dir = "."
    
    # Создаем директорию для результатов, если её нет
    results_dir = "results"
    if not os.path.exists(results_dir):
        os.makedirs(results_dir)
    
    # Список графиков для построения
    graphs = [
        ("_Vt.txt", 0, 1, "Время, с", "Скорость, м/с", f"Зависимость скорости от времени\n{base_name}", "_Vt.png"),
        ("_thetact.txt", 0, 1, "Время, с", "Угол наклона траектории, град", f"Зависимость угла наклона траектории от времени\n{base_name}", "_thetact.png"),
        ("_yt.txt", 0, 1, "Время, с", "Высота, м", f"Зависимость высоты от времени\n{base_name}", "_yt.png"),
        ("_xt.txt", 0, 1, "Время, с", "Дальность, м", f"Зависимость дальности от времени\n{base_name}", "_xt.png"),
        ("_thetat.txt", 0, 1, "Время, с", "Угол тангажа, град", f"Зависимость угла тангажа от времени\n{base_name}", "_thetat.png"),
        ("_alphat.txt", 0, 1, "Время, с", "Угол атаки, град", f"Зависимость угла атаки от времени\n{base_name}", "_alphat.png"),
        ("_omegazt.txt", 0, 1, "Время, с", "Угловая скорость, 1/с", f"Зависимость угловой скорости от времени\n{base_name}", "_omegazt.png"),
        ("_Vx.txt", 0, 1, "Дальность, м", "Скорость, м/с", f"Зависимость скорости от дальности\n{base_name}", "_Vx.png"),
        ("_thetacx.txt", 0, 1, "Дальность, м", "Угол наклона, град", f"Зависимость угла наклона траектории от дальности\n{base_name}", "_thetacx.png"),
        ("_yx.txt", 0, 1, "Дальность, м", "Высота, м", f"Траектория полета\n{base_name}", "_yx.png")
    ]
    
    created_graphs = []
    for suffix, x_col, y_col, x_label, y_label, title, output_suffix in graphs:
        # Проверяем разные возможные пути к файлам
        possible_paths = [
            os.path.join(data_dir, base_filename + suffix),
            os.path.join(data_dir, base_name + suffix),
            os.path.join("build/results", base_name + suffix),
            os.path.join("results", base_name + suffix),
            base_filename + suffix,
            base_name + suffix
        ]
        
        data_file = None
        for path in possible_paths:
            if os.path.exists(path):
                data_file = path
                break
        
        if data_file:
            output_file = os.path.join(results_dir, base_name + output_suffix)
            if plot_graph(data_file, x_col, y_col, x_label, y_label, title, output_file):
                created_graphs.append(output_file)
        else:
            print(f"  Файл данных не найден для суффикса {suffix}")
            print(f"  Искал по путям: {possible_paths[:2]}...")
    
    return created_graphs, base_name

def create_html_report():
    """Создание HTML-отчета с графиками"""
    # Создаем директорию results, если её нет
    if not os.path.exists("results"):
        os.makedirs("results")
    
    html_content = """<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Отчет по расчету траекторий ЛА</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            line-height: 1.6;
            color: #333;
            background-color: #f5f7fa;
            margin: 0;
            padding: 20px;
        }
        
        .container {
            max-width: 1400px;
            margin: 0 auto;
            background-color: white;
            border-radius: 10px;
            box-shadow: 0 0 20px rgba(0,0,0,0.1);
            padding: 30px;
        }
        
        header {
            text-align: center;
            margin-bottom: 40px;
            padding-bottom: 20px;
            border-bottom: 2px solid #eaeaea;
        }
        
        h1 {
            color: #2c3e50;
            font-size: 2.5em;
            margin-bottom: 10px;
        }
        
        .subtitle {
            color: #7f8c8d;
            font-size: 1.2em;
        }
        
        .calculation-section {
            margin-bottom: 50px;
        }
        
        h2 {
            color: #3498db;
            font-size: 1.8em;
            margin-bottom: 25px;
            padding-bottom: 10px;
            border-bottom: 1px solid #eee;
        }
        
        .graph-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(500px, 1fr));
            gap: 25px;
            margin-bottom: 30px;
        }
        
        .graph-item {
            background: #fff;
            border-radius: 8px;
            overflow: hidden;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
            transition: transform 0.3s ease, box-shadow 0.3s ease;
            border: 1px solid #e0e0e0;
        }
        
        .graph-item:hover {
            transform: translateY(-5px);
            box-shadow: 0 8px 15px rgba(0,0,0,0.15);
        }
        
        .graph-img {
            width: 100%;
            height: 300px;
            object-fit: contain;
            background-color: #f8f9fa;
            padding: 15px;
        }
        
        .graph-caption {
            padding: 15px;
            text-align: center;
            background-color: #f8f9fa;
            border-top: 1px solid #e0e0e0;
            font-weight: 600;
            color: #2c3e50;
        }
        
        .no-data {
            text-align: center;
            padding: 40px;
            color: #95a5a6;
            font-style: italic;
        }
        
        footer {
            text-align: center;
            margin-top: 50px;
            padding-top: 20px;
            border-top: 1px solid #eee;
            color: #95a5a6;
            font-size: 0.9em;
        }
        
        @media (max-width: 768px) {
            .container {
                padding: 15px;
            }
            
            .graph-grid {
                grid-template-columns: 1fr;
            }
            
            h1 {
                font-size: 2em;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>Отчет по расчету траекторий летательного аппарата</h1>
            <div class="subtitle">Анализ результатов моделирования баллистического полета</div>
        </header>
"""
    
    # Находим все PNG файлы
    png_files = glob.glob("results/*.png")
    png_files.sort()
    
    if not png_files:
        html_content += """
        <div class="no-data">
            <h2>Нет данных для отображения</h2>
            <p>Графики еще не были построены или сохранены в папку 'results'.</p>
            <p>Запустите программу расчета траектории и убедитесь, что файлы данных созданы.</p>
        </div>
"""
    else:
        # Группируем по имени расчета
        calculations = {}
        for file in png_files:
            basename = os.path.basename(file)
            # Извлекаем имя расчета (до последнего подчеркивания)
            parts = basename.split('_')
            if len(parts) > 2:
                calc_name = '_'.join(parts[:-1])
                if calc_name not in calculations:
                    calculations[calc_name] = []
                calculations[calc_name].append(file)
        
        # Добавляем графики в HTML
        for calc_name, files in sorted(calculations.items()):
            html_content += f'<div class="calculation-section">\n'
            html_content += f'<h2>Расчет: {calc_name}</h2>\n<div class="graph-grid">\n'
            for file in sorted(files):
                graph_name = os.path.basename(file).replace('.png', '').replace(calc_name + '_', '')
                html_content += f'''
                <div class="graph-item">
                    <img src="{os.path.basename(file)}" alt="{graph_name}" class="graph-img">
                    <div class="graph-caption">{graph_name}</div>
                </div>
                '''
            html_content += '</div>\n</div>\n'
    
    html_content += """
        <footer>
            <p>Отчет сгенерирован автоматически</p>
            <p>Дата создания: """ + str(np.datetime64('now')) + """</p>
        </footer>
    </div>
</body>
</html>
"""
    
    report_file = os.path.join("results", "report.html")
    with open(report_file, "w", encoding="utf-8") as f:
        f.write(html_content)
    
    print(f"\nHTML-отчет создан: {report_file}")
    return report_file

def main():
    print("=" * 60)
    print("Построение графиков для расчетов траектории")
    print("=" * 60)
    
    # Проверяем возможные директории с данными
    possible_data_dirs = [
        "build/results",
        "results",
        ".",
        "data"
    ]
    
    # Ищем файлы с данными
    data_patterns = [
        "*_Vt.txt",
        "*_graph_*.txt",
        "*_summary.txt"
    ]
    
    data_files = []
    for data_dir in possible_data_dirs:
        if os.path.exists(data_dir):
            for pattern in data_patterns:
                files = glob.glob(os.path.join(data_dir, pattern))
                data_files.extend(files)
    
    if not data_files:
        print("Файлы данных не найдены!")
        print("Искал в директориях:", possible_data_dirs)
        print("С шаблонами:", data_patterns)
        return
    
    print(f"Найдено файлов данных: {len(data_files)}")
    
    # Извлекаем уникальные базовые имена
    base_files = set()
    for file in data_files:
        # Извлекаем базовое имя разными способами
        basename = os.path.basename(file)
        
        # Пытаемся определить базовое имя
        if '_Vt.txt' in file:
            base_name = file.replace('_Vt.txt', '')
        elif '_graph_' in file:
            base_name = file[:file.rfind('_graph_')]
        else:
            # Убираем суффиксы для других типов файлов
            for suffix in ['_Vt.txt', '_thetact.txt', '_yt.txt', '_xt.txt', 
                          '_thetat.txt', '_alphat.txt', '_omegazt.txt',
                          '_Vx.txt', '_thetacx.txt', '_yx.txt', '_summary.txt']:
                if file.endswith(suffix):
                    base_name = file.replace(suffix, '')
                    break
            else:
                base_name = os.path.splitext(file)[0]
        
        base_files.add(base_name)
    
    print(f"\nНайдено расчетов: {len(base_files)}")
    
    # Строим графики для каждого расчета
    all_graphs = []
    for base_file in sorted(base_files):
        print(f"\n{'='*40}")
        graphs, calc_name = plot_all_graphs(base_file)
        all_graphs.extend(graphs)
    
    print(f"\n{'='*60}")
    print(f"Всего построено графиков: {len(all_graphs)}")
    
    # Создаем HTML-отчет
    if all_graphs:
        report_file = create_html_report()
        print(f"\nОтчет доступен по адресу: file://{os.path.abspath(report_file)}")
    else:
        print("\nГрафики не были построены. Проверьте наличие файлов данных.")
    
    print("\n" + "="*60)

if __name__ == "__main__":
    # Добавляем текущую директорию в путь импорта
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    main()