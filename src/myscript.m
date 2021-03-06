close all;
clear all;

osave=false;

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
if osave
print -dpng 'eclairement.png';
end

figure(1)
plot(squeeze(Htrue(Obs(1,1)+1,Obs(1,2)+1,:)),'m');
hold on
plot(squeeze(H0(Obs(1,1)+1,Obs(1,2)+1,:)),'b');
plot(squeeze(Ha(Obs(1,1)+1,Obs(1,2)+1,:)),'r');
plot(Obs(1,3)+1,Y(1),'*r');
xlabel('Time (in step time)')
ylabel('Height (m)')
legend('Reference','pseudo-inv','renormalization','observation');
if osave
print -dpng 'compare_sol.png'
end


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

if osave
print -dpng 'renorm-sol.png'
end

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
if osave
print -dpng 'pseudoinv-sol.png'
end

figure(5);
cax= [-1 1];
cmap=scol_dif(50);
subplot(2,2,1)
imagesc(H0(:,:,1)-Htrue(:,:,1),cax);
title('error pseudo-inv (t=0)');
colormap(cmap)
colorbar

subplot(2,2,2)
imagesc(H0(:,:,end)-Htrue(:,:,end),cax);
title('error pseudo-inv (t=300)');

colormap(cmap)
colorbar

subplot(2,2,3)
imagesc(Ha(:,:,1)-Htrue(:,:,1),cax);
title('error renorm (t=0)');

colormap(cmap)
colorbar

subplot(2,2,4)
imagesc(Ha(:,:,end)-Htrue(:,:,end),cax);
title('error renorm (t=300)');

colormap(cmap)
colorbar
if osave
print -dpng 'error_H.png'
end

Utrue = ncread('state_true.nc','Ufil');
U0    = ncread('state_0.nc','Ufil');
Ua    = ncread('state_a.nc','Ufil');
figure(6)
cax=[-0.03 0.03];

subplot(2,2,1)
H2 = U0(:,:,2);
imagesc(H2',cax);
title('Initial U (psuedo-inv)')

subplot(2,2,2);
H2 = U0(:,:,end);
imagesc(H2',cax);
title('U at t=300 (pseudo-inv)');

subplot(2,2,3)
H2 = Utrue(:,:,2);
imagesc(H2',cax);
title('Initial U (reference)')
colorbar('horiz')
subplot(2,2,4);
H2 = Utrue(:,:,end);
imagesc(H2',cax);
title('U at t=300 (reference)');
if osave
print -dpng 'pseudoinv-sol-U.png'
end


figure(7)
cax=[-0.03 0.03];

subplot(2,2,1)
H2 = Ua(:,:,2);
imagesc(H2',cax);
title('Initial U (renorm)')

subplot(2,2,2);
H2 = Ua(:,:,end);
imagesc(H2',cax);
title('U at t=300 (renorm)');

subplot(2,2,3)
H2 = Utrue(:,:,2);
imagesc(H2',cax);
title('Initial U (reference)')
colorbar('horiz')
subplot(2,2,4);
H2 = Utrue(:,:,end);
imagesc(H2',cax);
title('U at t=300 (reference)');
if osave
print -dpng 'renorm-sol-U.png'
end

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