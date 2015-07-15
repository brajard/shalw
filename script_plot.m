%% init
clear all
close all
load grad.dat
NY=length(unique(grad(:,2)));
NX=length(unique(grad(:,3)));
NT=length(unique(grad(:,1)));
tobs=301;
xobs=51;
yobs=51;
dx=5;
dy=5;
dt=25;

y=dy*((1:NY)-yobs);
x=dx*((1:NX)-xobs);
t=dt*((1:NT)-tobs);

%% plot (2D)
figure(1)
%for j=300:-5:0
j=100
it=find(grad(:,1)==j);

dH=reshape(grad(it,4),NX,NY);
dH=dH';
dU=reshape(grad(it,5),NX,NY);
dU=dU';
dV=reshape(grad(it,6),NX,NY);
dV=dV';

if(0)
figure(1)
clf
imagesc(x,y,dH);
xlabel('km')
ylabel('km');
%caxis([-1 1]);
title(['H adj. ' int2str(dt*(tobs-j)/60) ' hrs before obs']);
colorbar
end

if(1)
figure(2)
clf
imagesc(x,y,dU);
xlabel('km')
ylabel('km');
%caxis([-1 1]);
title(['U adj. ' int2str(dt*(tobs-j)/60) ' hrs before obs']);
colorbar
end
pause
%end


%% plot x
iy=find(grad(:,2)==50);
dH=reshape(grad(iy,4),NX,NT);
dU=reshape(grad(iy,5),NX,NT);
figure(3)
subplot(2,1,1);
plot(t/60,dH(51,:));
xlabel('hrs');
ylabel('H adj.');
title('obs point');

subplot(2,1,2);
plot(t/60,dH(53,:));
xlabel('hrs');
ylabel('H adj.');
title([int2str(2*dy) ' km from obs point'])

figure(4)
subplot(3,1,1);
plot(t/60,dU(51,:));
xlabel('hrs');
ylabel('U adj.');
title('obs point');

subplot(3,1,3);
plot(t/60,dU(53,:));
xlabel('hrs');
ylabel('U adj.');
axis([-140 0 -1 1])
title([int2str(2*dy) ' km East from obs point'])

subplot(3,1,2);
plot(t/60,dU(49,:));
xlabel('hrs');
ylabel('U adj.');
axis([-140 0 -1 1])
title([int2str(2*dy) ' km West from obs point'])

%% sauvegarde
addpath('~/stage/programmes2/programmes/matlab');
save_fig(1:4,'adjoint_shallow.ps');

