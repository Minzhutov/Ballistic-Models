set terminal pngcairo size 1200,800 enhanced font 'Arial,12'
set output 'gravity_plot.png'
set title 'Зависимость ускорения свободного падения от высоты'
set xlabel 'Высота, м'
set ylabel 'Ускорение, м/с²'
set grid
set key top right
plot 'gravity_data.txt' using 1:2 with lines linewidth 2 linecolor rgb '#0066cc' title ''
exit
