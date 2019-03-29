clear all;
clc

V=[
0
0.6
0.9
1.0
1.1
1.2
1.2

];

Q=[
0
30.2
38.7
43.7
48.6
50.7
52.9
];

figure
plot(V,Q)
title('Relación voltaje del sensor con el caudal medido');
xlabel('Voltios');
ylabel('Caudal');
grid on
grid minor



curva = fit(V,Q, 'poly4')

