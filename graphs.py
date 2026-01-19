#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
graphs.py

Скрипт собирает *.txt‑файлы, полученные из расчётов траекторий,
и выводит **один** график для каждой величины (V(t), θc(t), …),
на котором одновременно отображаются данные всех расчётов.
"""

import matplotlib
# Бэкенд без окна (под CI / сервер)
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
import os
import sys
import glob
from collections import defaultdict

# ----------------------------------------------------------------------
# 1.  Поиск файлов данных
# ----------------------------------------------------------------------
def find_data_files(base_name: str) -> dict:
    """
    Ищет все *.txt‑файлы, относящиеся к расчету с base_name.
    Возвращает словарь:
        {
            "_Vt.txt"      : [(full_path, "calc1"), (full_path2, "calc2"), ...],
            "_thetact.txt" : [(...), ...],
            ...
        }
    где calcX – метка (обычно тот же base_name без суффикса),
    которую будем показывать в легенде.
    """
    suffixes = [
        "_Vt.txt", "_thetact.txt", "_yt.txt", "_xt.txt",
        "_thetat.txt", "_alphat.txt", "_omegazt.txt",
        "_Vx.txt", "_thetacx.txt", "_yx.txt"
    ]

    # Возможные каталоги, где могут лежать файлы
    possible_dirs = [
        os.path.dirname(base_name) or ".",
        "build/results",
        "results",
        "data"
    ]

    # Словарь:  suffix → list[(path, label)]
    files_by_suffix = defaultdict(list)

    # Ищем везде, пока не найдём хотя‑бы один файл
    for suffix in suffixes:
        for d in possible_dirs:
            # Полный путь: <dir>/<base_name_without_path><suffix>
            candidate = os.path.join(d, os.path.basename(base_name) + suffix)
            if os.path.exists(candidate):
                label = os.path.basename(base_name)   # используем имя расчёта как метку
                files_by_suffix[suffix].append((candidate, label))
                break   # нашли нужный файл → переходим к следующему суффиксу

    return files_by_suffix


# ----------------------------------------------------------------------
# 2.  Построение одного объединённого графика
# ----------------------------------------------------------------------
def plot_combined(
    data_series: list,
    x_col: int,
    y_col: int,
    x_label: str,
    y_label: str,
    title: str,
    output_file: str,
    legend_loc: str = "best",
) -> bool:
    """
    data_series – список кортежей (path_to_txt, label).
    Для каждого файла читаются данные, после чего кривая
    добавляется в один общий рисунок.
    """
    if not data_series:
        print(f"  ✗ Нет данных для построения: {title}")
        return False

    plt.figure(figsize=(12, 7))

    for data_path, label in data_series:
        try:
            arr = np.loadtxt(data_path, skiprows=1)
            if arr.ndim == 1:          # иногда файл содержит лишь одну строку
                arr = arr.reshape(1, -1)
            plt.plot(
                arr[:, x_col],
                arr[:, y_col],
                linewidth=2,
                label=label,
            )
        except Exception as exc:
            print(f"  ✗ Не удалось прочитать {data_path}: {exc}")

    plt.xlabel(x_label, fontsize=12)
    plt.ylabel(y_label, fontsize=12)
    plt.title(title, fontsize=14)
    plt.grid(True, alpha=0.3)
    plt.legend(loc=legend_loc, fontsize=10)
    plt.tight_layout()

    # создаём каталог, если его нет
    out_dir = os.path.dirname(output_file)
    if out_dir and not os.path.isdir(out_dir):
        os.makedirs(out_dir)

    plt.savefig(output_file, dpi=300)
    plt.close()
    print(f"  ✓ Объединённый график сохранён: {output_file}")
    return True


# ----------------------------------------------------------------------
# 3.  Генерация HTML‑отчёта (не меняется, только чуть подправлен путь к png)
# ----------------------------------------------------------------------
def create_html_report():
    """Создаёт html‑страницу со всеми графиками, находящимися в results/*.png."""
    html_template_head = """<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <title>Отчёт по расчёту траекторий</title>
    <style>
        body{font-family:Arial,sans-serif;background:#f5f7fa;margin:0;padding:20px}
        .container{max-width:1400px;margin:auto;background:#fff;padding:30px;border-radius:10px;
                  box-shadow:0 0 20px rgba(0,0,0,0.1);}
        h1{color:#2c3e50;font-size:2.5em;margin-bottom:10px}
        .graph-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(500px,1fr));
                    gap:25px;margin-bottom:30px}
        .graph-item{background:#fff;border:1px solid #e0e0e0;border-radius:8px;
                    overflow:hidden;box-shadow:0 4px 6px rgba(0,0,0,0.1);
                    transition:transform .3s,box-shadow .3s}
        .graph-item:hover{transform:translateY(-5px);box-shadow:0 8px 15px rgba(0,0,0,0.15)}
        .graph-img{width:100%;height:300px;object-fit:contain;background:#f8f9fa;padding:15px}
        .graph-caption{text-align:center;padding:15px;background:#f8f9fa;
                       border-top:1px solid #e0e0e0;font-weight:600}
        footer{text-align:center;margin-top:50px;padding-top:20px;border-top:1px solid #eee;
               color:#95a5a6;font-size:0.9em}
    </style>
</head>
<body>
<div class="container">
    <header>
        <h1>Отчёт по расчёту траекторий летательного аппарата</h1>
        <p>Анализ результатов моделирования баллистического полёта</p>
    </header>
"""

    html_template_tail = """
    <footer>
        <p>Отчёт сгенерирован автоматически</p>
        <p>Дата создания: {date}</p>
    </footer>
</div>
</body>
</html>"""

    png_files = sorted(glob.glob("results/*.png"))
    if not png_files:
        body = "<p>Графики не найдены – запустите расчёт и заново выполните скрипт.</p>"
    else:
        # Группируем по названию расчёта (часть до последнего подчёркивания)
        calc_groups = defaultdict(list)
        for fp in png_files:
            name = os.path.basename(fp)
            parts = name.split('_')
            if len(parts) > 2:
                calc_name = "_".join(parts[:-1])        # всё, кроме окончательного суффикса
                calc_groups[calc_name].append(fp)

        body = ""
        for calc_name, files in sorted(calc_groups.items()):
            body += f'<div class="calculation-section">\n<h2>Расчёт: {calc_name}</h2>\n<div class="graph-grid">\n'
            for f in sorted(files):
                caption = os.path.basename(f).replace('.png', '').replace(calc_name + '_', '')
                body += f'''
                <div class="graph-item">
                    <img src="{os.path.basename(f)}" alt="{caption}" class="graph-img">
                    <div class="graph-caption">{caption}</div>
                </div>
                '''
            body += '</div>\n</div>\n'

    # собираем окончательный html‑файл
    report_path = os.path.join("results", "report.html")
    with open(report_path, "w", encoding="utf-8") as f:
        f.write(html_template_head + body +
                html_template_tail.format(date=str(np.datetime64('now')).split('T')[0]))
    print(f"\nHTML‑отчёт создан: {report_path}")
    return report_path


# ----------------------------------------------------------------------
# 4.  Основная логика сборки и построения
# ----------------------------------------------------------------------
def main():
    print("=" * 60)
    print("Построение объединённых графиков для расчётов траекторий")
    print("=" * 60)

    # 1) Находим все *.txt‑файлы, относящиеся к расчётам
    data_patterns = ["*_Vt.txt", "*_summary.txt"]
    data_files = []
    for d in ["build/results", "results", ".", "data"]:
        if not os.path.isdir(d):
            continue
        for pat in data_patterns:
            data_files.extend(glob.glob(os.path.join(d, pat)))

    if not data_files:
        print("Файлы данных не найдены! Проверьте, что расчёт уже выполнен.")
        return

    # 2) Выделяем уникальные «базовые имена» (имя расчёта без суффикса)
    base_names = set()
    for f in data_files:
        # убираем любые известные суффиксы, оставляем только основу
        for suf in ["_Vt.txt", "_thetact.txt", "_yt.txt", "_xt.txt",
                    "_thetat.txt", "_alphat.txt", "_omegazt.txt",
                    "_Vx.txt", "_thetacx.txt", "_yx.txt",
                    "_summary.txt"]:
            if f.endswith(suf):
                base_names.add(f[:-len(suf)])
                break
        else:
            base_names.add(os.path.splitext(f)[0])

    print(f"Найдено расчётов: {len(base_names)}")

    # 3) Сохраняем в словарь: suffix → [(path, label), …] для **всех** расчётов
    all_series = defaultdict(list)   # suffix → list[(path, label)]

    for base in sorted(base_names):
        # получаем все файлы данного расчёта
        files_by_suffix = find_data_files(base)
        for suffix, lst in files_by_suffix.items():
            all_series[suffix].extend(lst)

    # 4) Описание графиков (как в оригинальном скрипте)
    graphs_descr = [
        ("_Vt.txt",      0, 1, "Время, с",      "Скорость, м/с",      "Зависимость скорости от времени"),
        ("_thetact.txt", 0, 1, "Время, с",      "Угол наклона траект., град", "Зависимость угла наклона от времени"),
        ("_yt.txt",      0, 1, "Время, с",      "Высота, м",         "Зависимость высоты от времени"),
        ("_xt.txt",      0, 1, "Время, с",      "Дальность, м",      "Зависимость дальности от времени"),
        ("_thetat.txt",  0, 1, "Время, с",      "Угол тангажа, град", "Зависимость угла тангажа от времени"),
        ("_alphat.txt",  0, 1, "Время, с",      "Угол атаки, град",   "Зависимость угла атаки от времени"),
        ("_omegazt.txt", 0, 1, "Время, с",      "Угловая скорость, 1/с", "Зависимость угловой скорости от времени"),
        ("_Vx.txt",      0, 1, "Дальность, м",  "Скорость, м/с",      "Зависимость скорости от дальности"),
        ("_thetacx.txt", 0, 1, "Дальность, м",  "Угол наклона, град", "Зависимость угла наклона от дальности"),
        ("_yx.txt",      0, 1, "Дальность, м",  "Высота, м",         "Траектория полёта")
    ]

# 5) Строим один общий рисунок для каждой величины
    results_dir = "results"
    if not os.path.isdir(results_dir):
        os.makedirs(results_dir)

    for suffix, xcol, ycol, xlabel, ylabel, base_title in graphs_descr:
        series = all_series.get(suffix, [])
        if not series:
            print(f"  ⚠  Данных для {suffix} не найдено – график будет пропущен.")
            continue

        title = f"{base_title}"
        out_png = os.path.join(results_dir, f"combined{suffix.replace('.txt', '.png')}")
        plot_combined(
            data_series=series,
            x_col=xcol,
            y_col=ycol,
            x_label=xlabel,
            y_label=ylabel,
            title=title,
            output_file=out_png,
            legend_loc="upper right",
        )

    # 6) Генерируем html‑отчёт (он покажет те объединённые png, которые созданы)
    create_html_report()
    print("\nГотово! Откройте файл results/report.html в браузере.\n")
    print("=" * 60)


if __name__ == "__main__":
    # Добавляем текущий каталог в PYTHONPATH (на случай, если скрипт вызывается из build‑директории)
    sys.path.insert(0, os.path.abspath(os.path.dirname(__file__)))
    main()