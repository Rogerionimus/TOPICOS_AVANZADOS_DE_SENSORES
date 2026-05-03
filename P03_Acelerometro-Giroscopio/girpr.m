
data = load('gir_data.mat');


disp(fieldnames(data))

% Extraer datos 
fn = fieldnames(data);
gyro_raw = data.(fn{1});

if istimetable(gyro_raw) || istable(gyro_raw)
    gyro = gyro_raw.Variables;
else
    gyro = gyro_raw;
end

% Crear tiempo
fs = 100; % Hz 
t = (0:size(gyro,1)-1)/fs;

% Separar ejes 
gx = gyro(:,1);
gy = gyro(:,2);
gz = gyro(:,3);

% Integrar para obtener ángulos
roll = cumtrapz(t, gx);
pitch = cumtrapz(t, gy);
yaw = cumtrapz(t, gz); 

% Convertir a grados 
roll_deg = rad2deg(roll);
pitch_deg = rad2deg(pitch);
yaw_deg = rad2deg(yaw);

% Suavizar 
roll_deg = smoothdata(roll_deg, 'movmean', 10);
pitch_deg = smoothdata(pitch_deg, 'movmean', 10);

% Graficar
figure;

subplot(2,1,1)
plot(t, roll_deg, 'b')
grid on
title('Roll ')
xlabel('Tiempo [s]')
ylabel('Ángulo [°]')

subplot(2,1,2)
plot(t, pitch_deg, 'r')
grid on
title('Pitch ')
xlabel('Tiempo [s]')
ylabel('Ángulo [°]')