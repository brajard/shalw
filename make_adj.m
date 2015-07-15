%%%%%% Paramètres de l'experience
name='9obs_unif';
xobs=[49 50 51];
yobs=[49 50 51];
[Xobs,Yobs]=meshgrid(xobs,yobs);


%Temps du contrôle
t0=100;
%Temps et position de l'obs de ref

xo=50;
yo=50;

%Temps et position des obs (id. que dans le .i)
tobs=300;



[Xobs,Yobs]=meshgrid(xobs,yobs);

if ~exist('state')
load state.dat;
load grad.dat
end

%gradient de ref
I=find(grad(:,1)==t0);
NX=length(unique(grad(:,2)));
NY=length(unique(grad(:,3)));
R=reshape([grad(I,4:9)],NX,NY,6);
X=reshape([state(I,4:9)],[],1);

Y=zeros(length(Xobs(:)),1);

RR=zeros(length(Xobs(:)),length(R(:)));

for k=1:length(Xobs(:))
dx=Xobs(k)-xo;
dy=Yobs(k)-yo;

G=zeros(size(R));
G(max(1,1+dy):min(end,end+dy),max(1,1+dx):min(end,end+dx),:)=R(max(1,1-dy):min(end,end-dy),max(1,1-dx):min(end,end-dx),:);
%clf
%imagesc(G(:,:,1));
%hold on
%title(['X=' int2str(Xobs(k)) ', Y=' int2str(Yobs(k))]);
%plot(Xobs(k)+1,Yobs(k)+1,'*k');

I=find(state(:,1)==tobs & state(:,2)==Xobs(k) & state(:,3)==Yobs(k));
Y(k)=state(I,4);
%G(:)'*X
%Y(k)
RR(k,:)=G(:)';
%pause
end
I=find(state(:,1)==tobs);

figure(1)
imagesc(reshape(state(I,4),NY,NX));
hold on
plot(Xobs(:)+1,Yobs(:)+1,'+m');
title('Repartition des obs (h)');
colorbar

figure(2)
imagesc(reshape(X(1:10000),NY,NX));
title('True Solution (h)');
cax=caxis;
colorbar

figure(3)
imagesc(reshape(RR(1,1:10000),NY,NX));
hold on
plot(Xobs(1)+1,Yobs(1)+1,'+m')
title('Adjoint function (for h) associated with first obs');
colorbar

XX=RR'*inv(RR*RR')*Y;
figure(4)
imagesc(reshape(XX(1:10000),NY,NX));
title('Min norm. solution (h)')
colorbar
if ~exist(name,'dir')
mkdir(name)
end
fname={'obs','truesol','exadj','pinvsol'};
for j=1:length(fname)
  
print(['-f' int2str(j)],'-dpng',[name filesep fname{j}])

end


