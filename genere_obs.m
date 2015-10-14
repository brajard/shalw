%%Exp 1
%xobs = [60:4:80];
%yobs = [30:4:70];
%tobs = 300 ;
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

fid=fopen('obs.dat','w');
fprintf(fid,'#%d\n',length(X));
for j=1:length(X)
fprintf(fid,'%d %d %d\n',X(j),Y(j),T(j));
end
fclose(fid);
