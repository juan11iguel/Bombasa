instrfind
delete(instrfind);
clear all;
clc;

run CalibracionYModelos.m
Vanterior = 0;
%% Acquire and analyze data
A = 3;
%% Connect to Arduino
% Use the arduino command to connect to an Arduino device.

a = arduino('/dev/ttyUSB1','Uno','Trace',true);

    writeDigitalPin(a,'D7',1);
    writeDigitalPin(a,'D4',0);




%% Acquire and display live data
ii = 0;
V = zeros(1e4,1);
t = zeros(1e4,1);

tic

figure
h = animatedline;
ax = gca;
ax.YGrid = 'on';
ax.YLim = [-0.1 60];

stop = false;
startTime = datetime('now');


sc=0;
X=0;
tic;
T_INICIAL=tic;
i=1;
while ~stop

TIEMPO = toc(T_INICIAL)
if (TIEMPO < 10)
   sc=20
   X = 5/100 * sc;
end
if (TIEMPO < 20 && TIEMPO> 10)
   sc = 30
   X = 5/100 * sc;
end
 if (TIEMPO < 30 && TIEMPO> 20)
    sc = 40
    X = 5/100 * sc;
 end
 if (TIEMPO < 40 && TIEMPO> 30)
    sc = 50
    X = 5/100 * sc;
 end
  if (TIEMPO < 50 && TIEMPO> 40)
    sc = 60
    X = 5/100 * sc;
  end
  if (TIEMPO < 60 && TIEMPO> 50)
    sc = 80
    X = 5/100 * sc;
  end
  if (TIEMPO < 70 && TIEMPO> 60)
    sc = 100
    X = 5/100 * sc;
  end



     
     
 

    
    writePWMVoltage(a,'D3',X);
    % Read current voltage value
    v = readVoltage(a,'A5');
    V = curva(v);
    
    % Get current time
    t =  datetime('now') - startTime;
    % Add points to animation
    addpoints(h,datenum(t),V)
    % Update axes
    ax.XLim = datenum([t-seconds(15) t]);
    datetick('x','keeplimits')
    drawnow
    
    %Generar señal de entrada
    entrada(i) = X;
    i = i+1;

    % Check stop condition
    stop = readDigitalPin(a,'D8');
end
writePWMVoltage(a,'D3',0);

%% Plot the recorded data

[timeLogs,tempLogs] = getpoints(h);
timeSecs = (timeLogs-timeLogs(1))*24*3600;
figure
plot(timeSecs,tempLogs)
xlabel('Elapsed time (sec)')
ylabel('Temperature (\circF)')

%% Smooth out readings with moving average filter

F15 = smooth(tempLogs,15);
F20 = smooth(tempLogs,20);
F30 = smooth(tempLogs,30);


figure
subplot(2,1,1)
plot(timeSecs,tempLogs)
xlabel('Elapsed time (sec)')
ylabel('Caudal (\circF)')
grid on
grid minor
hold on 

%% Plot the original and the smoothed temperature signal, and illustrate the uncertainty.

plot(timeSecs,F15,'r', timeSecs, F20, 'b')
subplot(2,1,2)
plot(timeSecs, entrada)




%%

