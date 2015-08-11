close all;
clear all;

Htrue = ncread('state_true.nc','Hfil');
H0    = ncread('state_0.nc','Hfil');
Ha    = ncread('state_a.nc','Hfil');
load Y.dat

%Eclairement
load Phi.dat
load indok.dat
phi2 = nan*ones(100,100);
phi2(indok == 1) = Phi ;
figure(2)
imagesc(phi2'.^0.5);
title('Square root of Eclairement');
colorbar
colormap('gray')

%Obs
fid = fopen('obs.dat','r');

nobs = fscanf(fid,'#%d\n');
Obs = nan*ones(3,nobs);

  Obs=fscanf(fid,'%d',[3 nobs]);

fclose(fid);
Obs=Obs';
hold on
plot(Obs(:,1)+1,Obs(:,2)+1,'+r');
print -dpng 'eclairement.png';

figure(1)
plot(squeeze(Htrue(Obs(1,2)+1,Obs(1,1)+1,:)),'m');
hold on
plot(squeeze(H0(Obs(1,2)+1,Obs(1,1)+1,:)),'b');
plot(squeeze(Ha(Obs(1,2)+1,Obs(1,1)+1,:)),'r');
plot(Obs(1,3)+1,Y(1),'*r');
xlabel('Time (in step time)')
ylabel('Height (m)')
legend('Reference','pseudo-inv','renormalization','observation');
print -dpng 'compare_sol.png'


cax = [-10 10]
figure(3)
subplot(2,2,1)
H2 = Ha(:,:,1);
imagesc(H2',cax);
title('Initial Height (renormalization)')



subplot(2,2,2);
H2 = Ha(:,:,end);
imagesc(H2',cax);
title('Height at t=300 (renormalization)');

subplot(2,2,3)
H2 = Htrue(:,:,1);
imagesc(H2',cax);
title('Initial Height (reference)')
colorbar('horiz')
subplot(2,2,4);
H2 = Htrue(:,:,end);
imagesc(H2',cax);
title('Height at t=300 (reference)');

print -dpng 'renorm-sol.png'

figure(4)
subplot(2,2,1)
H2 = H0(:,:,1);
imagesc(H2',cax);
title('Initial Height (psuedo-inv)')



subplot(2,2,2);
H2 = H0(:,:,end);
imagesc(H2',cax);
title('Height at t=300 (pseudo-inv)');

subplot(2,2,3)
H2 = Htrue(:,:,1);
imagesc(H2',cax);
title('Initial Height (reference)')
colorbar('horiz')
subplot(2,2,4);
H2 = Htrue(:,:,end);
imagesc(H2',cax);
title('Height at t=300 (reference)');
print -dpng 'pseudoinv-sol.png'



if (0)
load Y.dat
Y=Y';
load Xt.dat
Xt = Xt';
load Radj.dat;
load Rext.dat;
load indok.dat;
load Xa_fin.dat;
Xa_fin=Xa_fin';
load Xa_init.dat;
Xa_init=Xa_init';
Xext = Xt(indok==1);

X2 = nan*ones(100,100);
X2(indok==1) = Xext;
mok2 = reshape(indok,100,100);

figure(1)
him = imagesc(X2);
%set(him,'AlphaData',mok2)

figure(2)
Xa2=nan*ones(100,100);
Xa2(indok==1) = Xa_init;
imagesc(Xa2);

figure(3)
Xa2(indok==1) = Xa_fin;
imagesc(Xa2);
end;