% baseFileName shall indicate the folder and the query Id

function display_percentiles(baseFileName)

popFig = figure();
clf;
hold on;

val = load(strcat(baseFileName , '_population_1.dat'));
plot(val(:,1), val(:,2), 'r');

perc = load(strcat(baseFileName , '_population_1_p.dat'));
for pIndex = 2: size(perc,2)
    plot(perc(:,1), perc(:,pIndex), 'b');
endfor
title('Population prediction and percentiles');

hold off;

print(popFig, strcat(baseFileName , '_population.png'));


aprioriFig = figure();
clf;
hold on;

val = load(strcat(baseFileName , '_apriori_1.dat'));
plot(val(:,1), val(:,2), 'r');

perc = load(strcat(baseFileName , '_apriori_1_p.dat'));
for pIndex = 2: size(perc,2)
    plot(perc(:,1), perc(:,pIndex), 'b');
endfor
title('A priori prediction and percentiles');

hold off;
print(aprioriFig, strcat(baseFileName , '_apriori.png'));

apostFig = figure();
clf;
hold on;

val = load(strcat(baseFileName , '_aposteriori_1.dat'));
plot(val(:,1), val(:,2), 'r');

perc = load(strcat(baseFileName , '_aposteriori_1_p.dat'));
for pIndex = 2: size(perc,2)
    plot(perc(:,1), perc(:,pIndex), 'b');
endfor
title('A posteriori prediction and percentiles');

hold off;
print(apostFig, strcat(baseFileName , '_aposteriori.png'));

endfunction
