clear all;
clc

addpath('~/Documents/Bombasa/ProyectoBombasa')
addpath('~/Documents/Bombasa/ProyectoBombasa/Matlab')
load Workspace.mat;


%% Calibración de la bomba
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
ylabel('Caudal (mL/seg)');
grid on
grid minor
hold on;

curva = fit(V,Q, 'poly2');
plot(curva)

%% Representación del ensayo general
figure
subplot(2,1,1)
plot(timeSecs, F20)
title('Ensayo con tren de escalones')
xlabel('Tiempo (seg)')
ylabel('Caudal [mL/seg]')
grid on
grid minor

subplot(2,1,2)
plot(timeSecs, entrada)
xlabel('Tiempo (seg)')
ylabel('Voltaje de control [V]')
grid on
grid minor



%% Cálculo del modelo actualizado del sistema en función del punto de operación

k    =  [primoModelo.Kp, secondoModelo.Kp, terzoModelo.Kp, quartoModelo.Kp];
tau =  [primoModelo.Tp1, secondoModelo.Tp1, terzoModelo.Tp1, quartoModelo.Tp1];
punto_op = [31, 39, 44, 48];

ganancia = fit(punto_op', k','poly3');
cteTiempo = fit(punto_op', tau', 'smoothingspline');

figure
subplot(2,1,1)
plot(punto_op, k)
hold on;
plot(ganancia)
subplot(2,1,2)
plot(punto_op, tau)
hold on;
plot(cteTiempo)


%% Obtenci�n de las funci�n de transferencia, en funci�n de la consigna.

Q =[31.5;39;44.5;48;50]  % Consigna

% Estimaci�n de la variable K dependiendo de la consigna

K=[14.99202552;12;6.710447761;2.330282531;0.469609856];             % Ganacia de los modelos obtenida, a trav�s, de ident.
interpK=fit(Q,K,'poly4')                                            % Funci�n que interpola.


% Estimaci�n de la variable tau dependiendo de la consigna

tau=[7.974481659;6.640106242;5.970149254;3.9793076;2.05338809];     % Cte de tiempo de los modelos obtenida, a trav�s, de ident.
interptau=fit(Q,tau,'poly4')


% Gr�fica de la interpolaci�n.

figure
subplot(2,1,1)
hold on
title('K')
plot(Q,K)
plot(interpK)
xlabel('Caudal')
ylabel('Ganancia')

subplot(2,1,2)
hold on
title('Tau')
plot(Q,tau)
hold on
plot(interptau)
xlabel('Caudal')
ylabel('Constante de tiempo')


