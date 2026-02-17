% Calculo de datos estadisticos de la altura
alt_media = mean(alt);
alt_max = max(alt);
alt_min = min(alt);
variacion_total = alt_max - alt_min;

% Creación de la grafica
figure('Name', 'Variación de Altitud', 'NumberTitle', 'off');
plot(timestamp, alt, 'LineWidth', 1.5, 'Color', [0.7000 0.3700 0.0280]); % Color naranja super cool
grid on;
hold on;

% Promedio de altitud registrada
yline(alt_media, '--r', 'Promedio', 'LineWidth', 2);

% Configuración de labels
xlabel('Tiempo');
ylabel('Altitud (metros sobre el nivel del mar)');
title(['Variación de Altura (Rango: ', num2str(variacion_total, '%.2f'), ' m)']);

% Mostrar métricas en la consola
fprintf('--- Análisis de Estabilidad Vertical ---\n');
fprintf('Altitud promedio: %.2f m\n', alt_media);
fprintf('Variación máxima (Salto): %.2f metros\n', variacion_total);
fprintf('Desviación estándar vertical: %.2f m\n', std(alt));