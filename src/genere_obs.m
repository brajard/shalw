%%Exp 1
%xobs = [60:4:80];
%yobs = [30:4:70];
%tobs = 300 ;

if(false)
xobs=46;
yobs=50;
tobs=[50:50:300];

[X,Y,T]=meshgrid(xobs,yobs,tobs);
X=X(:);
Y=Y(:);
T=T(:);
I=find(X==46 & Y==50 & T==300);
X=[X(I);X];
Y=[Y(I);Y];
T=[T(I);T];
X(I+1)=[];
Y(I+1)=[];
T(I+1)=[];
end

%%Exp 2
if (true)
Tmax=1461;
xmax=79;
ymax=79;

nobs=100000;

%X=nan*ones(nobs,1);

[X,Y,T] = meshgrid(0:xmax,0:ymax,1:Tmax);

Ind = randperm(numel(T));

X=X(Ind(1:nobs));
Y=Y(Ind(1:nobs));
T=T(Ind(1:nobs));



fid=fopen('obs.dat','w');
fprintf(fid,'#%d\n',length(X));
for j=1:length(X)
fprintf(fid,'%d %d %d\n',X(j),Y(j),T(j));
end
fclose(fid);
end

%% Plot
load obs_val.dat
t0=1460;
if ~exist('Hfil')
Hfil=ncread('state_true.nc','Hfil');
end

I=find(obs_val(:,3)==t0);
scatter(obs_val(I,1),obs_val(I,2),40,obs_val(I,4),'filled');