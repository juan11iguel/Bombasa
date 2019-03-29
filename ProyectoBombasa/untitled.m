instrfind
delete(instrfind);
clear all;
clc;

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
ax.YLim = [-0.1 2];

stop = false;
startTime = datetime('now');



X=0;
TIEMPO=0;
while ~stop

if (TIEMPO < 10)
   X=1
end
if (TIEMPO < 20 && TIEMPO> 10)
   X = 1.3
end
 if (TIEMPO < 30 && TIEMPO> 20)
    X = 1.6
 end
 if (TIEMPO < 40 && TIEMPO> 35)
    X = 2
 end
  if (TIEMPO < 45 && TIEMPO> 40)
    X = 3
  end
  if (TIEMPO < 50 && TIEMPO> 45)
    X = 4
  end
  if (TIEMPO < 55 && TIEMPO> 50)
    X = 5
  end
  if ( TIEMPO> 55)
    stop = 1;
 end
     
     
 TIEMPO = TIEMPO + 0.1

    
    writePWMVoltage(a,'D3',X);
    % Read current voltage value
    v = readVoltage(a,'A2');
    V = curva(v);
    
    % Get current time
    t =  datetime('now') - startTime;
    % Add points to animation
    addpoints(h,datenum(t),V)
    % Update axes
    ax.XLim = datenum([t-seconds(15) t]);
    datetick('x','keeplimits')
    drawnow
    
    Vanterior=V;

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

smoothedTemp = smooth(tempLogs,25);
tempMax = smoothedTemp + 2*9/5;
tempMin = smoothedTemp - 2*9/5;

figure
plot(timeSecs,tempLogs, timeSecs,tempMax,'r--',timeSecs,tempMin,'r--')
xlabel('Elapsed time (sec)')
ylabel('Temperature (\circF)')
hold on 

%%
% Plot the original and the smoothed temperature signal, and illustrate the
% uncertainty.

plot(timeSecs,smoothedTemp,'r')





%%

