set terminal pngcairo size 1200,800 enhanced font 'Arial,12'
set output 'sound_speed_plot.png'
set title 'Зависимость скорости звука от высоты'
set xlabel 'Высота, м'
set ylabel 'Скорость звука, м/с'
set grid
set key top right
plot 'sound_speed_data.txt' using 1:2 with lines linewidth 2 linecolor rgb '#0066cc' title ''
exit
