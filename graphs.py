import matplotlib.pyplot as plt
import numpy as np
import os
import glob

def plot_graph(data_file, x_col, y_col, x_label, y_label, title, output_file):
    """Построение одного графика из файла данных"""
    try:
        data = np.loadtxt(data_file, skiprows=1)
        if data.ndim == 1:
            data = data.reshape(-1, 2)
        
        plt.figure(figsize=(10, 6))
        plt.plot(data[:, 0], data[:, 1], 'b-', linewidth=2)
        plt.xlabel(x_label)
        plt.ylabel(y_label)
        plt.title(title)
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        plt.savefig(output_file, dpi=300)
        plt.close()
        print(f"График сохранен: {output_file}")
    except Exception as e:
        print(f"Ошибка при построении графика {data_file}: {e}")

def plot_all_graphs(base_filename):
    """Построение всех графиков для одного расчета"""
    base_name = os.path.basename(base_filename).replace('_graph', '')
    output_dir = os.path.dirname(base_filename)
    
    # Список графиков для построения
    graphs = [
        ("_Vt.txt", 0, 1, "Время, с", "Скорость, м/с", f"Зависимость скорости от времени\n{base_name}", "_Vt.png"),
        ("_thetact.txt", 0, 1, "Время, с", "Угол наклона траектории, град", f"Зависимость угла наклона от времени\n{base_name}", "_thetact.png"),
        ("_yt.txt", 0, 1, "Время, с", "Высота, м", f"Зависимость высоты от времени\n{base_name}", "_yt.png"),
        ("_xt.txt", 0, 1, "Время, с", "Дальность, м", f"Зависимость дальности от времени\n{base_name}", "_xt.png"),
        ("_thetat.txt", 0, 1, "Время, с", "Угол тангажа, град", f"Зависимость угла тангажа от времени\n{base_name}", "_thetat.png"),
        ("_alphat.txt", 0, 1, "Время, с", "Угол атаки, град", f"Зависимость угла атаки от времени\n{base_name}", "_alphat.png"),
        ("_omegazt.txt", 0, 1, "Время, с", "Угловая скорость, 1/с", f"Зависимость угловой скорости от времени\n{base_name}", "_omegazt.png"),
        ("_Vx.txt", 0, 1, "Дальность, м", "Скорость, м/с", f"Зависимость скорости от дальности\n{base_name}", "_Vx.png"),
        ("_thetacx.txt", 0, 1, "Дальность, м", "Угол наклона, град", f"Зависимость угла наклона от дальности\n{base_name}", "_thetacx.png"),
        ("_yx.txt", 0, 1, "Дальность, м", "Высота, м", f"Траектория полета\n{base_name}", "_yx.png")
    ]
    
    for suffix, x_col, y_col, x_label, y_label, title, output_suffix in graphs:
        data_file = base_filename + suffix
        if os.path.exists(data_file):
            output_file = os.path.join(output_dir, base_name + output_suffix)
            plot_graph(data_file, x_col, y_col, x_label, y_label, title, output_file)

def main():
    # Ищем все файлы с данными для графиков
    data_files = glob.glob("build/results/*_graph_*.txt")
    
    # Уникальные базовые имена
    base_files = set()
    for file in data_files:
        if '_Vt.txt' in file:
            base_name = file.replace('_Vt.txt', '')
            base_files.add(base_name)
    
    # Строим графики для каждого расчета
    for base_file in sorted(base_files):
        print(f"\nПостроение графиков для: {os.path.basename(base_file)}")
        plot_all_graphs(base_file)
    
    # Создаем HTML-отчет
    create_html_report()

def create_html_report():
    """Создание HTML-отчета с графиками"""
    html_content = """
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="UTF-8">
        <title>Отчет по расчету траекторий ЛА</title>
        <style>
            body { font-family: Arial, sans-serif; margin: 40px; }
            h1 { color: #333; text-align: center; }
            h2 { color: #555; border-bottom: 2px solid #eee; padding-bottom: 10px; }
            .graph-grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 20px; margin: 20px 0; }
            .graph-item { border: 1px solid #ddd; padding: 10px; text-align: center; }
            .graph-item img { max-width: 100%; height: auto; }
            .caption { font-size: 14px; color: #666; margin-top: 10px; }
        </style>
    </head>
    <body>
        <h1>Отчет по расчету траекторий летательного аппарата</h1>
    """
    
    # Находим все PNG файлы
    png_files = glob.glob("results/*.png")
    png_files.sort()
    
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
    for calc_name, files in calculations.items():
        html_content += f'<h2>Расчет: {calc_name}</h2>\n<div class="graph-grid">\n'
        for file in sorted(files):
            graph_name = os.path.basename(file).replace('.png', '').replace(calc_name + '_', '')
            html_content += f'''
            <div class="graph-item">
                <img src="{os.path.basename(file)}" alt="{graph_name}">
                <div class="caption">{graph_name}</div>
            </div>
            '''
        html_content += '</div>\n'
    
    html_content += """
    </body>
    </html>
    """
    
    with open("results/report.html", "w", encoding="utf-8") as f:
        f.write(html_content)
    
    print(f"\nHTML-отчет создан: results/report.html")

if __name__ == "__main__":
    main()