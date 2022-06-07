% Exposure Logger Data Processing

% - This code imports, and graphs the data collected by the PLANTOWER and
% the SGP30

clear, clc

%- Import wizard: Make sure data file is in same directory
dataimport = uiimport('EEM6042022.TXT'); %replace file name
data = dataimport.EEM6042022; %- make sure to change to your file name after dot

%- Creating a time vector that is the same length of the data to plot with
time = 5*(1:length(data(:,1)));
    % Data was sampled every five minutes, hence the 5

%- Creating vectors to plot exposure limits on
shortTermPM2p5 = 35*ones(length(data(:,1)),1);
longTermPM2p5 = 12*ones(length(data(:,1)),1);

easLow = 300*ones(length(data(:,1)),1);
easAccpt = 500*ones(length(data(:,1)),1);
easMarg = 1000*ones(length(data(:,1)),1);
easHigh = 3000*ones(length(data(:,1)),1);

%- Creating vectors for the bar graph
totalparticles = sum(data);
bary = totalparticles(:,7:10);
barx = {'Particles > 0.3 \mu m/ 0.1L of Air', 'Particles > 0.5 \mu m/ 0.1L of Air', 'Particles > 1.0 \mu m/ 0.1L of Air', 'Particles > 2.5 \mu m/ 0.1L of Air'}


% Plot 1 PM2.5 Standard Concentration Units
figure(1)
plot(time,data(:,2),time,shortTermPM2p5,time,longTermPM2p5)
legend('Collected Data', 'EPA Short Term Exposure Limit (35 \mu g/m^3)', 'EPA Long Term Exposure Limit (12 \mu g/m^3)')
xlabel('Time (min)')
ylabel('Concentration (\mu g/m^3)')
title('PM2.5 Concentration', '(Standard)')
grid off



% Plot 2 PM2.5 Enviromental Concentration under atmospheric conditions
figure(2)
plot(time,data(:,5))
hold on
plot(time, shortTermPM2p5)
plot(time, longTermPM2p5)
legend('Collected Data', 'EPA Short Term Exposure Limit (35 \mu g/m^3)', 'EPA Long Term Exposure Limit (12 \mu g/m^3)')
xlabel('Time (min)')
ylabel('Concentration (\mu g/m^3)')
title('PM2.5 Concentration','(Enviromental)')
grid off
hold off

% Plot 3 eCO2 Readings from SGP30
figure(3)
plot(time,data(:,14))
hold on
xlabel('Time (min)')
ylabel('Concentration (ppm)')
title('CO2 Concentration')
grid off
hold off

% Plot 4 TVOC Readings from SGP30
figure(4)
hold on
plot(time,4.5*data(:,13)) %4.5 converts from ppb to ug/m^3
plot(time,easLow,'g')
plot(time,easAccpt,'y')
plot(time,easMarg,'r')
plot(time,easHigh,'k')
legend('Collected Data', 'EAS Acceptable Concern', 'EAS Marginal Concern', 'EAS High Concern', 'EAS Very High Concern')
xlabel('Time (min)')
ylabel('Concentration (\mu g/m^3)')
title('TVOC')
grid off
hold off

% Plot Particulate Distribution Histogram
figure(5)
bar(bary)
set(gca,'xticklabel',barx)
xlabel('Particle Diameter')
ylabel('Number of Particles')
title('Particle Distribution')

% That is all the data to pull and graph from the sensor! %


