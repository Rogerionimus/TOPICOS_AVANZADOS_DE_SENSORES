%% 1. IMPORTACIÓN DE DATOS
archivo = 'CAMINATA_SENSOR.txt'; %Colocar aqui el nombre del archivo.txt

%% 2. Graficar
figure('Name', 'Visualización de Caminata a piramide', 'NumberTitle', 'off');
gx = geoaxes;

% Graficamos la caminata como una linea continua
geoplot(gx, lat, lon, 'b', 'LineWidth', 2); 
hold on;

%% 3. Estética del Mapa
geobasemap satellite;
title('Recorrido del Sensor NEO-6M');

% Botón de cambio de mapa
btn = uicontrol('Style', 'togglebutton',...
                'String', 'Cambiar a satelite', ...
                'Position', [20 20 150 30], ...
                'Callback', @(src, event) togglemap(gx, src));

% Funcionamiento del botón
function togglemap(ax, button)
    if strcmp(ax.Basemap, 'streets')
        geobasemap(ax, 'satellite');
        button.String = 'Cambiar a calles';
    else 
        geobasemap(ax, 'streets');
        button.String = 'Cambiar a satelite';
    end
end