
data = load('acc_data.mat');

acc = data.Acceleration.Variables;   

fs = 100; 
t = (0:size(acc,1)-1)/fs;

% Separar ejes
ax = acc(:,1);
ay = acc(:,2);
az = acc(:,3);

% Calcular roll y pitch
roll = atan2(ay, az);
pitch = atan2(-ax, sqrt(ay.^2 + az.^2));

% Convertir a grados
roll = rad2deg(roll);
pitch = rad2deg(pitch);

% Suavizar
roll = smoothdata(roll, 'movmean', 10);
pitch = smoothdata(pitch, 'movmean', 10);

% Graficar
figure;

subplot(2,1,1)
plot(t, roll, 'b')
grid on
title('Roll')
xlabel('Tiempo [s]')
ylabel('Ángulo [°]')

subplot(2,1,2)
plot(t, pitch, 'r')
grid on
title('Pitch')
xlabel('Tiempo [s]')
ylabel('Ángulo [°]')