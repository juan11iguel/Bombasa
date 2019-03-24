instrfind
delete(instrfind);

clear all;
clc;

%% Acquire and analyze data

%% Connect to Arduino
% Use the arduino command to connect to an Arduino device.

a = arduino('/dev/ttyUSB0','Uno','Trace',true);

%% Take a single measurement
v = readVoltage(a,'A5');

    writeDigitalPin(a,'D7',1);
    writeDigitalPin(a,'D4',0);


%% Record and plot 10 seconds of data

ii = 0;
V = zeros(1e4,1);
t = zeros(1e4,1);

tic
X=0;
while toc < 10
    ii = ii + 1;
    % Read current voltage value
    v = readVoltage(a,'A4');
    V(ii) = v;
    % Get time since starting
    t(ii) = toc;
    
    
    writePWMVoltage(a,'D3',X);
    
     if (X < 4.5)
        X=X+0.1;
    else
        X=0;
    end
end

% Post-process and plot the data. First remove any excess zeros on the
% logging variables.
V = V(1:ii);
t = t(1:ii);
% Plot data versus time
figure
plot(t,V,'-o')
xlabel('Elapsed time (sec)')
ylabel('Voltage')
title('Ten Seconds of Voltage Data')
set(gca,'xlim',[t(1) t(ii)])

%% Compute acquisition rate

timeBetweenDataPoints = diff(t);
averageTimePerDataPoint = mean(timeBetweenDataPoints);
dataRateHz = 1/averageTimePerDataPoint;
fprintf('Acquired one data point per %.3f seconds (%.f Hz)\n',...
    averageTimePerDataPoint,dataRateHz)

%% Acquire and display live data

figure
h = animatedline;
ax = gca;
ax.YGrid = 'on';
ax.YLim = [-1 6];

stop = false;
startTime = datetime('now');
X=0;
while ~stop
    

    writePWMVoltage(a,'D3',X);

    % Read current voltage value
    v = readVoltage(a,'A5');
    if(v < 4.7 && v ~= 0)
        V =v;    
    else
        V=Vanterior;
    end
    
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
    if (X < 4)
        X=X+0.05;
    else
        X=0;
    end
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

