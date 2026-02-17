%RMD
% Función para el algoritmo de Haversine
function [d] = haversine(lat1, lon1, lat2, lon2)
    R = 6371; %Radio de la tierra en kilometros

    %Convertir grados a radianes
    Phi1=deg2rad(lat1);
    Phi2=deg2rad(lat2);
    DeltaPhi=deg2rad(lat2-lat1);
    DeltaLambda=deg2rad(lon2-lon1);

    %Formula de haversine
    a = sin(DeltaPhi/2)^2 + cos(Phi1) * cos(Phi2) * sin(DeltaLambda/2)^2;
    d = 2 * R * atan2(sqrt(a),sqrt(1-a)); %Distancia en km
end

% 1. Calculo del punto promedio (ubicación "real" estimada)
lat_ref = mean(lat);
lon_ref = mean(lon);

% 2. Calcular el error (distancia) de cada punto respecto al promedio
num_puntos = length(lat);
errores_metros = zeros(num_puntos, 1);

for i = 1:num_puntos
    % Usamos la función de Haversine (multiplicamos por 1000 para tener metros, pude haber declarado antes todo en metros)
    errores_metros(i) = haversine(lat_ref, lon_ref, lat(i), lon(i)) * 1000;
end

% 3. Estadísticas de error
error_promedio = mean(errores_metros);
error_maximo = max(errores_metros);
desviacion_std = std(errores_metros);

fprintf('--- Análisis de Error del Sensor ---\n');
fprintf('Error promedio (dispersión): %.2f metros\n', error_promedio);
fprintf('Error máximo detectado: %.2f metros\n', error_maximo);
fprintf('Desviación estándar: %.2f metros\n', desviacion_std);

% Esto se agrega a la gráfica de GPS_Phone
hold(gx, 'on'); % Para poder encimar nuevos elementos en el mapa

% Graficar todos los puntos capturados
geoscatter(gx, lat, lon, 15, 'red', 'filled', 'MarkerFaceAlpha', 0.3);

% Graficar el punto promedio (Target)
geoscatter(gx, lat_ref, lon_ref, 100, 'blue', 'x', 'LineWidth', 2);

legend(gx, 'Puntos con ruido', 'Centroide (Promedio)');