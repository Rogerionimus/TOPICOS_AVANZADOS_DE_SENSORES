%% 1. IMPORTACIÓN DE DATOS
archivo = 'DATOS_ENTRE_EDIFICIOS_SENSOR.txt'; %Colocar aqui el nombre del archivo.txt

% Usamos readtable para detectar automáticamente el formato CSV
%Forzamos la lectura de columnas por nombre, así evitamos la basura que
%aparece antes de que comience a recabar datos reales.
opts = detectImportOptions(archivo);
data = readtable(archivo, opts);

lat = data.latitud;
lon = data.longitud;

%% 2. CÁLCULO DE REFERENCIA Y ERROR
% Al igual que con el telefono, consideramos el promedio de los datos como
% la posición real, para tener un punto de referencia base para el análisis
lat_ref = mean(lat);
lon_ref = mean(lon);

num_puntos = length(lat);
errores_metros = zeros(num_puntos, 1);

for i = 1:num_puntos
    %Llammamos a la función de haversine que estamos declarando al final
    errores_metros(i) = haversine(lat_ref, lon_ref, lat(i), lon(i)) * 1000;
end

%% 3. ESTADÍSTICAS EN CONSOLA
error_promedio = mean(errores_metros); %Esto es igual al otro archivo
error_maximo = max(errores_metros);
desviacion_std = std(errores_metros);

fprintf('--- RESULTADOS MÓDULO NEO-6M ---\n');
fprintf('Puntos analizados: %d\n', num_puntos);
fprintf('Error promedio (dispersión): %.2f metros\n', error_promedio);
fprintf('Error máximo detectado: %.2f metros\n', error_maximo);
fprintf('Desviación estándar (Sigma): %.2f metros\n', desviacion_std);
%esto tambien es mas o menos lo mismo, pero colocamos la cantidad de puntos
%que recabo el modulo

%% 4. VISUALIZACIÓN EN MAPA
fig = figure('Name', 'Análisis de Dispersión Arduino', 'NumberTitle', 'off');
gx = geoaxes;

%Nube de puntos
geoscatter(gx, lat, lon, 25, errores_metros, 'filled', 'MarkerFaceAlpha', 0.5);
hold on;

% Graficar el Centroide (esta es la referencia para el error)
geoscatter(gx, lat_ref, lon_ref, 120, 'blue', 'x', 'LineWidth', 2);

% Configuración estética
geobasemap(gx, 'streets');
colormap(gx, 'jet'); % Colores cálidos para mayor error, porque es visualmente cool
colorbar(gx);
title(gx, 'Dispersión de posición: Sensor Quieto');
legend(gx, 'Lecturas NEO-6M', 'Centroide Promedio');

% 5. Botón cool
btn = uicontrol('Style', 'togglebutton',...
                'String', 'Cambiar a Satélite', ...
                'Position', [20 20 150 30], ...
                'Callback', @(src, event) togglemap(gx, src));

%% Funciones importantes

function togglemap(ax, button)%El boton para cambiar entre calles y satelite
    if strcmp(ax.Basemap, 'streets')
        geobasemap(ax, 'satellite');
        button.String = 'Cambiar a Calles';
    else 
        geobasemap(ax, 'streets');
        button.String = 'Cambiar a Satélite';
    end
end

%función haversine que se uso en el archivo del telefono
function [d] = haversine(lat1, lon1, lat2, lon2)
    R = 6371; % Radio de la tierra en km
    Phi1 = deg2rad(lat1);
    Phi2 = deg2rad(lat2);
    DeltaPhi = deg2rad(lat2-lat1);
    DeltaLambda = deg2rad(lon2-lon1);

    a = sin(DeltaPhi/2)^2 + cos(Phi1) * cos(Phi2) * sin(DeltaLambda/2)^2;
    d = 2 * R * atan2(sqrt(a), sqrt(1-a)); 
end