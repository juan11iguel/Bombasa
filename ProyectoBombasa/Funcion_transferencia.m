%% Obtención de las función de transferencia, en función de la consigna.

Q =[31.5;39;44.5;48;50]  % Consigna

% Estimación de la variable K dependiendo de la consigna

K=[14.99202552;12;6.710447761;2.330282531;0.469609856];             % Ganacia de los modelos obtenida, a través, de ident.
interpK=fit(Q,K,'poly4')                                            % Función que interpola.


% Estimación de la variable tau dependiendo de la consigna

tau=[7.974481659;6.640106242;5.970149254;3.9793076;2.05338809];     % Cte de tiempo de los modelos obtenida, a través, de ident.
interptau=fit(Q,tau,'poly4')


% Gráfica de la interpolación.


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

