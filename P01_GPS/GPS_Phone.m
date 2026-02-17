%RMD
% crear conexión con el dispositivo al crear el objeto "m"
m = mobiledev;

% Habilitar el sensor de posición (se puede hacer con el telefono pero esto es más cool)
m.PositionSensorEnabled = 1;

% Iniciar el registro de datos
m.logging = 1;

% Paussar (Tomar) datos durante 15 minutos
pause(15 * 60);

% Detener el registro de datos
m.logging = 0;

%Deshabilitar el sensor de posición
m.PositionSensorEnabled = 0;

% Comando poslog que devuelve la información del sensor a la PC
[lat, lon, timestamp, speed, course, alt, horizacc] = poslog(m);



% Crear figura
fig = figure(name="Mapa de puntos",NumberTitle="off");
% Creamos el eje geográfico explicitamente, así podemos controlarlo
gx = geoaxes;
% Mapa de puntos en un mapa
geoscatter(gx, lat, lon);
% Fondo de calles por defecto
geobasemap (gx, "streets");

% Creamos el boton
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
