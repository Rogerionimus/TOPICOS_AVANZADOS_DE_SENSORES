%% IDENTIFICACIÓN Y COMPARATIVA DE MODELOS TÉRMICOS (BMP180 vs LM35)
% Proyecto: Caracterización de Cámara Térmica con Foco Incandescente
% Materia: Control Clásico / Sensores

clear; clc; close all;

%% 1. CARGA DE DATOS EXPERIMENTALES
datos = readtable('Respuesta al escalón_Sistema térmico.txt');

tiempo = datos.Tiempo_s_;
T_BMP  = datos.T_BMP_C_;   % Sensor Digital (I2C)
T_LM35 = datos.T_LM35_C_;  % Sensor Analógico (ADC)

% Parámetros Experimentales del Escalón
U_escalon = 1.0; % Magnitud del escalón (100% de potencia del foco)
Ts = mean(diff(tiempo)); % Período de muestreo promedio (~2 segundos)

%% 2. MODELADO PARA SENSOR BMP180 (DIGITAL)
T0_BMP = T_BMP(1);
Tmax_BMP = max(T_BMP);
y_bmp_id = T_BMP - T0_BMP;
u_id = ones(size(tiempo)) * U_escalon;
data_bmp = iddata(y_bmp_id, u_id, Ts);

% Estimación de función de transferencia: 1 polo, con tiempo muerto (retraso)
G_BMP = tfest(data_bmp, 1, 0, NaN); 

%% 3. MODELADO PARA SENSOR LM35 (ANALÓGICO)
T0_LM35 = T_LM35(1);
Tmax_LM35 = max(T_LM35);
y_lm35_id = T_LM35 - T0_LM35;
data_lm35 = iddata(y_lm35_id, u_id, Ts);

% Estimación de la función de transferencia para el LM35
G_LM35 = tfest(data_lm35, 1, 0, NaN);

%% 4. DESPLIEGUE DE RESULTADOS EN CONSOLA
fprintf('======================================================\n');
fprintf('      COMPARATIVA DE MODELOS DE TRANSFERENCIA         \n');
fprintf('======================================================\n');
fprintf('--- SENSOR DIGITAL BMP180 ---\n');
fprintf('Temperatura Inicial: %.2f °C | Máxima: %.2f °C\n', T0_BMP, Tmax_BMP);
disp(G_BMP);

fprintf('--- SENSOR ANALÓGICO LM35 ---\n');
fprintf('Temperatura Inicial: %.2f °C | Máxima: %.2f °C\n', T0_LM35, Tmax_LM35);
disp(G_LM35);

%% 5. SIMULACIÓN Y GENERACIÓN DE GRÁFICA COMPARATIVA INTERFACES
% Extraemos las respuestas al escalón de los modelos matemáticos puros (Dinámica Pura)
[y_sim_bmp, t_sim_bmp] = step(G_BMP, max(tiempo));
[y_sim_lm, t_sim_lm] = step(G_LM35, max(tiempo));

% Generación de la ventana de gráficos formal
figure('Color', [1 1 1], 'Position', [150, 150, 1000, 480]);

% --- SUBGRÁFICA IZQUIERDA: DATOS OBTENIDOS POR CADA SENSOR ---
subplot(1,2,1);
plot(tiempo, T_BMP, 'r.', 'MarkerSize', 6); hold on;
plot(tiempo, T_LM35, 'b.', 'MarkerSize', 6); 

grid on;
title('Datos Experimentales Obtenidos', 'FontSize', 12, 'FontWeight', 'bold');
xlabel('Tiempo (segundos)', 'FontSize', 10); 
ylabel('Temperatura Absoluta (°C)', 'FontSize', 10);
legend('Curva Real BMP180 (Digital)', 'Curva Real LM35 (Analógico)', 'Location', 'southeast');

% --- SUBGRÁFICA DERECHA: FUNCIONES DE TRANSFERENCIA (DINÁMICA PURA) ---
subplot(1,2,2);
plot(t_sim_bmp, y_sim_bmp, 'r-', 'LineWidth', 2.5); hold on;
plot(t_sim_lm, y_sim_lm, 'b--', 'LineWidth', 2.5);

grid on;
title('Funciones de Transferencia Estimadas (\DeltaT)', 'FontSize', 12, 'FontWeight', 'bold');
xlabel('Tiempo (segundos)', 'FontSize', 10); 
ylabel('Incremento de Temperatura (\Delta °C)', 'FontSize', 10);
legend('Modelo G_{BMP}(s)', 'Modelo G_{LM35}(s)', 'Location', 'southeast');

% Extracción de parámetros para el cuadro de texto indexado
[K_b, tau_b] = tfdata(G_BMP, 'v'); td_b = G_BMP.IODelay;
[K_l, tau_l] = tfdata(G_LM35, 'v'); td_l = G_LM35.IODelay;

str_info = sprintf(['\\bfParámetros Identificados:\\rm\n\n' ...
                    '\\color{red}\\bfG_{BMP}(s):\\rm\\color{black}\n' ...
                    'K = %.2f °C/U\n\\tau = %.1f s\nt_d = %.1f s\n\n' ...
                    '\\color{blue}\\bfG_{LM35}(s):\\rm\\color{black}\n' ...
                    'K = %.2f °C/U\n\\tau = %.1f s\nt_d = %.1f s'], ...
                    K_b(2), 1/K_b(1), td_b, K_l(2), 1/K_l(1), td_l);

% Ubicación estratégica del cuadro de texto sobre la subgráfica derecha
annotation('textbox', [0.55 0.15 0.16 0.35], 'String', str_info, ...
           'FitBoxToText', 'on', 'BackgroundColor', [1 1 1 0.9], 'EdgeColor', [0.5 0.5 0.5]);