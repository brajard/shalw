if ~exist('state')
  load state.dat
end

T=unique(state(:,1));
NX=length(unique(state(:,2)));
NY=length(unique(state(:,3)));

for j=T'
it=find(state(:,1)==j);
H=reshape(state(it,4),NX,NY);

imagesc(H,[0 0.6]);
colorbar
pause

  
end
