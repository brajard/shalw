%% Script qui vérifie RX=Y

%Temps du contrôle
t0=100;

%Temps et position de l'obs (id. que dans le .i)
tobs=300;
xobs=50;
yobs=50;



if ~exist('state')
load state.dat;
load grad.dat
end

NX=length(unique(grad(:,3)));
NT=length(unique(grad(:,1)));
I=find(state(:,1)==tobs & state(:,2)==xobs & state(:,3)==yobs);
Y=state(I,4);

I=find(grad(:,1)==t0);
%Ip1=find(grad(:,1)==t0+1);

%R matrix (ligne)
R=reshape([grad(I,4:9)],1,[]);

%X matrix (colonne)
X=reshape([state(I,4:9)],[],1);