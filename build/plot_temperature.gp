set terminal pngcairo size 1200,800 enhanced font 'Arial,12'
set output 'temperature_plot.png'
set title 'Зависимость температуры от высоты'
set xlabel 'Высота, м'
set ylabel 'Температура, К'
set grid
set key top right
plot 'temperature_data.txt' using 1:2 with lines linewidth 2 linecolor rgb '#0066cc' title ''
exit
