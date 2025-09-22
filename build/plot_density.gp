set terminal pngcairo size 1200,800 enhanced font 'Arial,12'
set output 'density_plot.png'
set title 'Зависимость плотности от высоты'
set xlabel 'Высота, м'
set ylabel 'Плотность, кг/м³'
set grid
set key top right
set logscale y
plot 'density_data.txt' using 1:2 with lines linewidth 2 linecolor rgb '#0066cc' title ''
exit
