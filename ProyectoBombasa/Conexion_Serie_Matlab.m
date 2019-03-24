a = arduino('/dev/ttyUSB0','Uno','Trace',true);

while 1:
     V=readVoltage(a, 'A5');
end
passo = 1;
t=1;
x=0;
x=[x,V];
plot(x);
grid
t=t+passo;
drawnow;
end
