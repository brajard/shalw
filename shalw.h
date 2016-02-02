//#define GEOSTROPHY

#define RENORM
#define USE_NETCDF
// Neglect dissipation in init term
//#define DISSIP0 


#ifdef USE_NETCDF
#include <netcdf.h>

/* Dim max des strings */
#define STRLEN 100

/* Observation structure */
struct obs {
  int X ;
  int Y ;
  int T ;
  YREAL val;
};

/* list of obs */
struct obs ** lobs = NULL ;
int nobs ;
/* Dimensions parameters */
#define NDIMS 3
#define Y_NAME "yaxis"
#define X_NAME "xaxis"
#define REC_NAME "time"
#define FILENAME "gradients.nc"

/* Name of variables */
#define NVARS 6
char SNames[NVARS][100]={"Hfil","Ufil","Vfil","Hphy","Uphy","Vphy"};
char Names[NVARS][100]={"dHfil","dUfil","dVfil","dHphy","dUphy","dVphy"};

/* Units attributes */
char Long_Names[NVARS][STRLEN]={"Gradient J on Filtered_Height",
				"Gradient J on Filtered x velocity",
				"Gradient J on Filtered y velocity",
				"Gradient J on Unfiltered Height",
				"Gradient J on Unfiltered x velocity",
				"Gradient J on Unfiltered y velocity"};
char SLong_Names[NVARS][STRLEN]={"Filtered_Height",
				 "Filtered x velocity",
				 "Filtered y velocity",
				 "Unfiltered Height",
				 "Unfiltered x velocity",
				 "Unfiltered y velocity"};
char Units[NVARS][STRLEN]={"unitless","seconds","seconds","unitless","seconds","seconds"};
char SUnits[NVARS][STRLEN]={"meters","m/s","m/s","meters","m/s","m/s"};

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRNC(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}


#endif //ifdef USE_NETCDF

#ifdef RENORM
#define SELDON_WITH_BLAS
#define SELDON_DEBUG_LEVEL_4
#define SELDON_WITH_LAPACK

//#include "/usr/home/jbrlod/usr/seldon-5.2/Seldon.hxx"
#include "/home/ROCQ/clime/jbrajard/project/verdandi/verdandi-1.6.1/include/seldon/Seldon.hxx"

using namespace Seldon;
typedef double Real_wp;

void test_seldon();
void get_adjoint(int i);
//Adjoint matrix
Matrix<YREAL> Radj ;

//Observation vector
Vector<YREAL> Yobs ;
Vector<YREAL> Xc ;
Vector<YREAL> Xa ; //Solution (in reduced space)
Vector<int> indok ; //Valid index in control vector
int maxiter = 0;
//Size of the control space :
const int TotS = SZY *SZX ;
#endif //ifdef RENORM


//Declaration
double randn (double mu, double sigma);
extern int Yobs_insert_data (char *nmmod, int sortie, int iaxe, int jaxe, int kaxe,
			     int pdt, YREAL val);
extern int Youtoobs(YioKind yiokind, int cdesc, char *cdes[]);
extern void       Yrazgrad_all();

double dx,dy,dedt,svdedt,pcor,grav,dissip,dissip0,hmoy,alpha,gb,gmx,gsx,gmy,gsy,rho0, nu,beta,sigper=0;
YREAL fcor[SZY];//fcor = pcor(y0)+beta*(y-y0)
int flag_cor=0; //+1 si init dy, +2 si init pcor, +4 si init beta
int cor_computed=0; //if cor is not computed yet.
int balanced = 0 ;
void savestate();
void erase_lobs();
void clear_Yst_nodo(struct Yst_nodo *n_obs, int lev, int max);

#ifdef USE_NETCDF
int ncinit();
int savegradnc();
int readnc (char filename[],char Name[STRLEN],YREAL data[SZY][SZX], int it0);
void xsavenc(int argc, char *argv[]);
int savenc(char filename[],char Names[NVARS][STRLEN],
	   char Units[NVARS][STRLEN], char Long_Names[NVARS][STRLEN],
	   YREAL data[NVARS][YNBALLTIME_Toce][SZY][SZX], int it);

#endif

YREAL grads[6][YNBALLTIME_Toce][SZY][SZX];
YREAL state[6][YNBALLTIME_Toce][SZY][SZX];

void savegrad(int it);

void xdisplay();
void appli_start(int argc, char *argv[]){
#ifdef USE_NETCDF
  // ncinit();
#endif
  test_seldon();

}

void init_coriolis () {
  printf("init coriolis\n");
  int y0=SZY/2;
  //Init de coriolis
  for (int iy=0;iy<SZY;iy++)
    fcor[iy]=pcor + beta * dy * (iy - y0);

}
void before_it(int nit){}
void cost_function(int pdt){}
void adjust_target(){}
void after_it(int nit){
  xdisplay();
  savegrad(Yt-1);
}

void forward_before(int ctrp){
	if(Yt==1) {svdedt=dedt;dedt=0.0;}
	if(Yt==2) dedt /= 2;
	
}
void forward_after(int ctrp){
	if(Yt==1) dedt=svdedt;
	if(Yt==2) dedt *= 2;
}

void backward_before(int ctrp){
  if(Yt==2) dedt /=2;
  if(Yt==1) {svdedt = dedt;dedt = 0.0;}
  savegrad(Yt);
}

void backward_after(int ctrp){
	if(Yt==2) dedt *=2;
	if(Yt==1) dedt = svdedt;
}

short select_io(int indic, char *nmod, int sortie, int iaxe,
		int jaxe, int kaxe, int pdt, YREAL *val){
	if(indic==YIO_SAVESTATE){
	  if(iaxe == 0 && jaxe == 0) fprintf(Yiofp, "\n\n");
	    else if (jaxe == 0) fprintf(Yiofp, "\n");
	}
	return(1);
}

void load_allobs() {
  //Load the state of the model as observation 
  srand(300);
if (lobs == NULL || nobs == 0) {
    fprintf(stderr,"load_allobs : no obs loaded");
    return;
  }
    
 //First erase all obs
 erase_lobs();

 //Compute the model
 Yset_modeltime(0);
 before_it(1);
 //printf("---forward(i=%d)---\n",i);
 Yforward(-1, 0);
 fprintf(stdout,"Perturbation appliquée : %g\n",sigper);
 for (int i = 0 ; i < nobs ; i++) {
   lobs[i]->val = YS_Hfil(0,lobs[i]->X,lobs[i]->Y,lobs[i]->T);
   lobs[i]->val += randn(0,sigper);
   Yobs_insert_data("Hfil",0,lobs[i]->X,lobs[i]->Y,0,lobs[i]->T,lobs[i]->val);
 }
 after_it(1);
 Yset_modeltime(0);

}

void load_bck() {
  //Load the initial state of the model as background
  //perturbation by sigper
  srand(200);
  char *opt[]={"outoebx","Hfil","1","1"};
  //1 save state
  int ix,iy;
  int it=0;
    for (ix=0;ix<YA1_Soce;ix++)
      for (iy=0;iy<YA2_Soce;iy++)
	{
	  //	  state[0][it][iy][ix]=YS_Hfil(0,ix,iy,it);
	  YS_Hfil(0,ix,iy,it) += randn(0,sigper);
	  }
    Youtoobs(YIO_OUTOEBX,4,opt);
      
    
}

void convol_obs(int iobs, int sz) {
  /*Convolue la ieme observation avec
   un noyeau cosinus de taille 2*sz
   */
  int xx[2*sz+1];
  int yy[2*sz+1];
  YREAL dval[2*sz+1][2*sz+1];
  YREAL cnorm=0;
  int i,j;
  for (i=0;i<2*sz+1;i++)
    {
      yy[i]=(lobs[iobs]->Y)-sz+i;
      // printf("yy[%d]=%d\n",i,yy[i]);
    }
      for (j=0;j<2*sz+1;j++){
	xx[j]=(lobs[iobs]->X)-sz+j;
	//printf("xx[%d]=%d\n",j,xx[j]);
      }
      for (i=0;i<2*sz+1;i++){
	for (j=0;j<2*sz+1;j++)
	  {
	    dval[i][j]= 
	      (cos(M_PI*(xx[j]-lobs[iobs]->X)/sz)+1)*
	      (cos(M_PI*(yy[i]-lobs[iobs]->Y)/sz)+1);
	    cnorm+=dval[i][j];
	    //    printf("%3.3g ",dval[i][j]);
	  }
	//	printf("\n");
      }
 for (i=0;i<2*sz+1;i++){
	for (j=0;j<2*sz+1;j++)
	  {
	    dval[i][j]= dval[i][j]/cnorm;
	    //    printf("%3.3g ",dval[i][j]);
	  }
	//	printf("\n");
      }

 //Set observations
 YREAL mu=0;
 for (i=0;i<2*sz+1;i++)
   for (j=0;j<2*sz+1;j++)
     {
       mu += YS_Hfil(0,xx[i],yy[j],lobs[iobs]->T)*dval[i][j];
       Yobs_insert_data("Hfil",0,xx[i],yy[j],0,lobs[iobs]->T,YS_Hfil(0,xx[i],yy[j],lobs[iobs]->T));
       YS_Hfil(0,xx[i],yy[j],lobs[iobs]->T)+=dval[i][j];
     }
	lobs[iobs]->val = mu;                                        ;

}
      
void compute_adjoint() {

  if (lobs == NULL || nobs == 0) {
    fprintf(stderr,"compute_adjoint : no obs loaded");
    return;
  }
  
  
  for (int i = 0 ; i < nobs ; i++) {
    //Forward to initialise states
    //Needs to be done at each time step to have the good time
    // Needs to be changes to optimize...
    Yset_modeltime(0);
    before_it(1);
    //printf("---forward(i=%d)---\n",i);
    Yforward(-1, 0);
    
    erase_lobs();

    //Trick to compute adjoint (should also work for non-linear models)
    convol_obs(i,4);

    //lobs[i]->val = YS_Hfil(0,lobs[i]->Y,lobs[i]->X,lobs[i]->T);
    //YS_Hfil(0,lobs[i]->Y,lobs[i]->X,lobs[i]->T)++;
    //Yobs_insert_data("Hfil",0,lobs[i]->Y,lobs[i]->X,0,lobs[i]->T,lobs[i]->val);



    Yrazgrad_all();  /* avant fct de cout et backprop : sur tous les pas de temps, raz de tous les gradients de tous les modules */
    
    YTotalCost = 0.0;	/* Raz aussi du Cout avant les calculs de cout */
    
    Ybackward (-1, 0); // Ybackward (YNBSTEPTIME);/* AD (adjoint):-> d*x =M*(X).dX : Yjac * YG -> Ytbeta */
    after_it(1);
    get_adjoint(i);
  } //for i
  savestate();
}

void xdisplay(){
	int i,j;
	i=(int)(SZX/2);
	j=(int)(SZY/2);
	printf("point:%i,%i, Hfil=% -23.15e @J/@Hfil=% -23.15e\n", i,j,
			YS_Hfil(0,i,j,300), YG_Hfil(0,i,j,0));
}


void xcos(int argc, char *argv[]) {
  gb = atof(argv[1]);
  if (argc==2) {
    gmx = SZX*dx / 2 ;
    gmy = SZY*dy / 2 ;
    gsx = SZX*dx / 20 ;
    gsy = SZY*dy / 20 ;
  }
  else
    if (argc==4) {
      gmx = SZX*dx / 2 ;
      gmy = SZY*dy / 2 ;
      gsx = atof(argv[2]);
      gsy = atof(argv[3]);
    }
    else
      {
	
	gmx = atof(argv[2]); gsx = atof(argv[3]);
	gmy = atof(argv[4]); gsy = atof(argv[5]);
      }

  int i0,j0,di,dj;
  i0 = gmx/dx;
  j0 = gmy/dy;
  di = gsx/dx;
  dj = gsy/dy;
  for (int j = j0-dj ; j<= j0+dj ; j++)
    for (int i = i0-di ; i<=i0+di ; i++)
	    YS_Hfil(0,i,j,0) = 0.25*gb*
	      (cos(M_PI*(i*dx-gmx)/gsx)+1)*
	      (cos(M_PI*(j*dx-gmy)/gsy)+1);
	
}

void xporte(int argc, char *argv[]) {
  gb = atof(argv[1]);
  if (argc==2) {
    gmx = SZX*dx / 2 ;
    gmy = SZY*dy / 2 ;
    gsx = SZX*dx / 20 ;
    gsy = SZY*dy / 20 ;
  }
  else
    if (argc==4) {
      gmx = SZX*dx / 2 ;
      gmy = SZY*dy / 2 ;
      gsx = atof(argv[2]);
      gsy = atof(argv[3]);
    }
    else
      {
	
	gmx = atof(argv[2]); gsx = atof(argv[3]);
	gmy = atof(argv[4]); gsy = atof(argv[5]);
      }

  int i0,j0,di,dj;
  i0 = gmx/dx;
  j0 = gmy/dy;
  di = gsx/dx;
  dj = gsy/dy;
  for (int j = j0-dj ; j<= j0+dj ; j++)
    for (int i = i0-di ; i<=i0+di ; i++)
      YS_Hfil(0,i,j,0)=gb;
	
}

void xwind (int argc, char *argv[]){
  YREAL tau0 =  atof(argv[1]);
  	if (argc==2) {
	  gmx = SZX*dx / 2 ;
	  gmy = SZY*dy / 2 ;
	  gsx = SZX*dx / 20 ;
	  gsy = SZY*dy / 20 ;
	}
	//	YREAL Lx = SZX*dx;
	YREAL Ly = SZY*dy;
	
	for (int i = 0; i<SZX; i++)
	  for (int j = 0; j<SZY; j++) {
	    YS1_Taux(i,j) = tau0 * 
	      //exp( -pow((i*dx-gmx)/gsx,2) /2.) *
	      //exp(-pow((j*dy-gmy)/gsy,2) /2.) *
	       cos (2*M_PI*((j*dy)-gmy)/Ly) ;
	    YS1_Tauy(i,j) = 0 ;
	  }
	
}

void xgauss(int argc, char *argv[]){

	gb = atof(argv[1]);
	if (argc==2) {
	  gmx = SZX*dx / 2 ;
	  gmy = SZY*dy / 2 ;
	  gsx = SZX*dx / 20 ;
	  gsy = SZY*dy / 20 ;
	}
	else
	  if (argc==4) {
	    gmx = SZX*dx / 2 ;
	    gmy = SZY*dy / 2 ;
	    gsx = atof(argv[2]);
	    gsy = atof(argv[3]);
	  }
	  else
	    {
	      
	      gmx = atof(argv[2]); gsx = atof(argv[3]);
	      gmy = atof(argv[4]); gsy = atof(argv[5]);
	    }
        for (int i = 0; i<SZX; i++)
	  for (int j = 0; j<SZY; j++)
	    YS_Hfil(0,i,j,0) = gb*(exp( -pow((i*dx-gmx)/gsx,2) /2.))
	      *(exp(-pow((j*dy-gmy)/gsy,2) /2.));
}

void xset_maxiter(int argc, char *argv[]) {
    maxiter = atoi(argv[1]);
  }
void xvitgeo(){
	double gf, gfh;
#ifdef GEOSTROPHY
	fprintf(stderr,"xvitgeo : Warning, geostrophy computation is included in the model\n");
#endif
	gf = (grav / pcor);
	for(int i=0; i<SZX; i++)
	  for(int j=0; j<SZY; j++){ 
	    gfh=gf*YS_Hfil(0,i,j,0);
	    YS_Ufil(0,i,j,0)=gfh*(j*dy-gmy)/(gsy*gsy);
            YS_Vfil(0,i,j,0)=gfh*-(i*dx-gmx)/(gsx*gsx);
	  }
}

void read_lobs(int argc, char *argv[]){
  //Read list of obseravtion in a file
  //Format of the file :
  // Header begining with #Nombre d'obs
  // liste of couples (x,y,t) (begining with 0);

  FILE * fid;
  int status, x,y,t;
  fid = fopen(argv[1],"r");
  if (fid == NULL) {
    fprintf(stderr,"read_lobs: file %s not found\n",argv[1]);
    return;
  }
  char line[128];
  fgets(line,sizeof(line),fid);
  sscanf(line,"#%d",&nobs);
  fprintf(stdout,"%d obs read in %s\n",nobs,argv[1]);
  
  lobs = (struct obs**)malloc(nobs * sizeof(struct obs*));
  for (int i=0;i<nobs;i++) {
    status = fscanf(fid,"%d %d %d",&x,&y,&t);
    if (status != 3) {
      fprintf(stderr,"Unable to read obs #%d\n",i);
      break;
    }
    lobs[i] = (struct obs*)malloc(sizeof(struct obs));
    lobs[i] -> X = x;
    lobs[i] -> Y = y;
    lobs[i] -> T = t;
    lobs[i] -> val = NAN;

  }
  if (nobs<10)
    for (int i=0;i<nobs;i++) 
      printf("%d %d %d\n",lobs[i]->X,lobs[i]->Y,lobs[i]->T);
  else {
    fprintf(stdout,"%d obs loaded\n",nobs);
    for (int i=0;i<5;i++)
      printf("%d %d %d\n",lobs[i]->X,lobs[i]->Y,lobs[i]->T);
    printf("...\n");
    printf("%d %d %d\n",lobs[nobs-1]->X,lobs[nobs-1]->Y,lobs[nobs-1]->T);
  }
  fclose(fid);

}

void xsave_obs(int argc, char *argv[]) {
  FILE *fid;
  fid=fopen(argv[1],"w");
  for (int i=0;i<nobs;i++) {
    fprintf(fid,"%d %d %d %g\n",lobs[i]->X,lobs[i]->Y,lobs[i]->T,lobs[i]->val);
  }
  fclose(fid);

}

void xivg(int argc,char *argv[]){
	double val;
	val = atof(argv[2]);
			
	if       (strcmp(argv[1], "dt") == 0) dedt=val;
	else if  (strcmp(argv[1], "dx") == 0) dx=val;
	else if  (strcmp(argv[1], "dy") == 0) { 
	  dy=val;
	  cor_computed=0;
	  flag_cor=flag_cor-flag_cor%2 + 1 ;
	}

	else if  (strcmp(argv[1], "pcor") == 0) {
	  pcor=val;
	  cor_computed=0;

	  flag_cor=4*(flag_cor/4)+(flag_cor%2) + 2;
	}
	else if  (strcmp(argv[1], "grav") == 0) grav=val;
	else if  (strcmp(argv[1], "dissip") == 0) 
	  {
	    dissip=val;
#ifdef DISSIP0
	    dissip0 = 0 ;
	    fprintf(stdout," dissipiation neglected in balance term\n");
#else
	    dissip0 = dissip ;
#endif
	  }
	    
	else if  (strcmp(argv[1], "hmoy") == 0) hmoy=val;
	else if  (strcmp(argv[1], "alpha") == 0) alpha=val;
	else if  (strcmp(argv[1], "rho0") == 0) rho0=val;
	else if  (strcmp(argv[1], "nu") == 0) nu=val;
	else if  (strcmp(argv[1], "sigper") == 0) sigper = val;
	else if  (strcmp(argv[1], "beta")== 0) {
	  beta=val;
	  cor_computed=0;
	  
	  flag_cor=(flag_cor%4)+4;
	}
	if (flag_cor==7 && cor_computed==0) {
	  init_coriolis() ;
	  cor_computed=1;
	}
}

void savestate() {
 int it,ix,iy;
  for (it=0;it<YNBALLTIME_Toce;it++)
    for (ix=0;ix<YA1_Soce;ix++)
      for (iy=0;iy<YA2_Soce;iy++)
	{
	  state[0][it][iy][ix]=YS_Hfil(0,ix,iy,it);
	  state[1][it][iy][ix]=YS_Ufil(0,ix,iy,it);
	  state[2][it][iy][ix]=YS_Vfil(0,ix,iy,it);
	  state[3][it][iy][ix]=YS_Hphy(0,ix,iy,it);
	  state[4][it][iy][ix]=YS_Uphy(0,ix,iy,it);
	  state[5][it][iy][ix]=YS_Vphy(0,ix,iy,it);	 
	}

}

void saveinit() {
  //Save state at the end of the trajectory in the init
  int ix,iy;
    for (ix=0;ix<YA1_Soce;ix++)
      for (iy=0;iy<YA2_Soce;iy++) {
	YS_Hfil(0,ix,iy,0) = YS_Hfil(0,ix,iy,YNBALLTIME_Toce-1);
	YS_Ufil(0,ix,iy,0) = YS_Ufil(0,ix,iy,YNBALLTIME_Toce-1);
	YS_Vfil(0,ix,iy,0) = YS_Vfil(0,ix,iy,YNBALLTIME_Toce-1);

      }
}

void xsavestate(int argc,char *argv[]){
  if (argc!=2) {
    printf("xsavestate: incorrect number of args\n");
    exit(2);
  }
 
  char  *filename=argv[1];
  FILE *fid;
  fid=fopen(filename,"w");
  if (fid==NULL) {
    printf("xsavestate: unable to open file %s\n",filename);
    exit(2);
  }
  int it,ix,iy;
  for (it=0;it<YNBALLTIME_Toce;it++)
    for (ix=0;ix<YA1_Soce;ix++)
      for (iy=0;iy<YA2_Soce;iy++)
	{
	  fprintf(fid,"%d %d %d ",it,ix,iy);
	  fprintf(fid,"%g %g %g %g %g %g\n",YS_Hfil(0,ix,iy,it),YS_Ufil(0,ix,iy,it),YS_Vfil(0,ix,iy,it),YS_Hphy(0,ix,iy,it),YS_Uphy(0,ix,iy,it),YS_Vphy(0,ix,iy,it));
	}
  fclose(fid);
}



int savegradnc() {
  int ncid,retval,it,iy,ix,ik,nvar;
 /* The start and count arrays will tell the netCDF library where to
      write our data. */
   size_t start[NDIMS], count[NDIMS];
   int varids[NVARS];
  /* These settings tell netcdf to write one timestep of data. (The
     setting of start[0] inside the loop below tells netCDF which
     timestep to write.) */
   count[0]=1;
   count[1]=SZY;
   count[2]=SZX;
   start[0]=0;
   start[1]=0;
   start[2]=0;

  /* Open nc file */
   if ((retval = nc_open(FILENAME, NC_WRITE, &ncid)))
     ERRNC(retval);
   if ((retval=nc_inq_varids(ncid,&nvar,varids)))
     ERRNC(retval);
   for (it=0;it<YNBALLTIME_Toce;it++)
     for (iy=0;iy<YA1_Soce;iy++)
       for (ix=0;ix<YA2_Soce;ix++)
	 {
	   start [0]=it;
	   for (ik=0;ik<6;ik++) 
	     if ((retval = nc_put_vara_double(ncid, varids[ik], start, count, 
					     &grads[ik][it][0][0])))
	       ERRNC(retval);
	       }
   
   /* Close the file. */
   if ((retval = nc_close(ncid)))
     ERRNC(retval);
   return(0);
}


void savegrad(int argc,char *argv[]){
  if (argc==1) {
#ifdef USE_NETCDF
    printf("savegrad : save netcdf file %s\n",FILENAME);
    savegradnc();
    return;
#else
    fprintf(stderr,"savegrad : no filename specified or netcdf inactive\n");
    exit(2);
#endif
  }

  if (argc!=2) {
    printf("savegrad: incorrect number of args\n");
    exit(2);
  }
 
  char  *filename=argv[1];
  FILE *fid;
  fid=fopen(filename,"w");
  if (fid==NULL) {
    printf("savegrad: unable to open file %s\n",filename);
    exit(2);
  }
  int it,ix,iy,ik;
  for (it=0;it<YNBALLTIME_Toce;it++)
    for (iy=0;iy<YA1_Soce;iy++)
      for (ix=0;ix<YA2_Soce;ix++)
	{
	  fprintf(fid,"%d %d %d ",it,iy,ix);
	  for (ik=0;ik<6;ik++) 
	    fprintf(fid,"%g ",grads[ik][it][iy][ix]);
	  fprintf(fid,"\n");
	}
  fclose(fid);
}

void xobs (int argc, char *argv[]) {
  char *nmod=argv[1];
  int i,j,t,val;
  i=atoi(argv[2]);
  j=atoi(argv[3]);
  t=atoi(argv[4]);
  val=(YREAL)atof(argv[5]);
  printf("YS_Hfil(0,%d,%d,%d)=%g\n",t,i,j,YS_Hfil(0,i,j,t));
  Yobs_insert_data(nmod,0,i,j,0,t,YS_Hfil(0,i,j,t)+val);
}

void savegrad(int it){
  int iy,ix;	
 for (iy=0;iy<YA1_Soce;iy++)
    for (ix=0;ix<YA2_Soce;ix++)
      {
	grads[0][it][iy][ix]=YG_Hfil(0,iy,ix,it);
	grads[1][it][iy][ix]=YG_Ufil(0,iy,ix,it);
	grads[2][it][iy][ix]=YG_Vfil(0,iy,ix,it);
	grads[3][it][iy][ix]=YG_Hphy(0,iy,ix,it);
	grads[4][it][iy][ix]=YG_Uphy(0,iy,ix,it);
	grads[5][it][iy][ix]=YG_Vphy(0,iy,ix,it);	      
      }
}



void erase_lobs() {
  //Efface toute l'arboresence des obs (y compris les ébauches)
  
  int itraj;
  struct Yst_nodo *YRobs = NULL;
  YioInsertObsCtr = 0 ;
  for (itraj=0;itraj<YNBTRAJ;itraj++) {
    //Effacement des obs liées au modul imod

    //  if (YTabMod[itraj].is_target || !YTabMod[itraj].is_cout) {
      YRobs=YTabTraj[itraj].YRobs;
      clear_Yst_nodo(YRobs,0,10);

      YTabTraj[itraj].YRobs=NULL;
      //}

  } //for itraj
  for (int wi = 0; wi < YNBTRAJ; ++wi)
    {
      YTabTraj[wi].YRobs = (struct Yst_nodo*) malloc (sizeof (struct Yst_nodo)); /* init du Root des arborescences */
      YTabTraj[wi].YRobs->frere = YTabTraj[wi].YRobs->fils = NULL;	/* d'observations pour chaque trajectoire */
    }

}


void clear_Yst_nodo(struct Yst_nodo *n_obs, int lev, int max) {
  /* lev est le niveau courant (racine=0)
max est le niveau des feuilles soit
4 si dimod=1
5 si dimod=2
6 si dimod=3
dimod est défini au niveau 2 avec dimod=YTabMod[imod].dim


  */
  
  if (lev==max) {
    //on nettoie une feuille (ni frere ni fils)
    free(n_obs);
    n_obs = NULL ;
  }
  
  else {

    if (lev==2) {
      //on peut déterminer le niveau max
      int dimod=YTabMod[n_obs->iind].dim;
      max=dimod+3;
    }

    if (n_obs->fils!=NULL && n_obs->frere!=NULL) {
      clear_Yst_nodo(n_obs->frere,lev,max);
      n_obs->frere=NULL;
    }    
    if (n_obs->fils!=NULL) {
      clear_Yst_nodo(n_obs->fils,lev+1,max);
      n_obs->fils=NULL;
    }
    free(n_obs);
    n_obs=NULL;
  } // else (pas feuille)
  

}

/************************************
 *    NETCDF FUNCTIONS              *
 ************************************/

#ifdef USE_NETCDF
int ncinit() {
  int ivar,retval;
  int ncid;
  int dimids[NDIMS];
  int varids[NVARS];
  /* Create the file */
  if ((retval = nc_create(FILENAME, NC_CLOBBER, &ncid)))
      ERRNC(retval);

  /* Define the dimensions. The record dimension is defined to have
    * unlimited length - it can grow as needed. In this example it is
    * the time dimension.*/
 
   if ((retval = nc_def_dim(ncid, Y_NAME, SZY, &dimids[1])))
      ERRNC(retval);
   if ((retval = nc_def_dim(ncid, X_NAME, SZX, &dimids[2])))
      ERRNC(retval);
   if ((retval = nc_def_dim(ncid, REC_NAME, NC_UNLIMITED, &dimids[0])))
      ERRNC(retval);

   /*Define the netcdf variables */
   for (ivar=0;ivar<NVARS;ivar++) {
     if ((retval = nc_def_var(ncid, Names[ivar], NC_DOUBLE, NDIMS, 
			      dimids, &varids[ivar])))
       ERRNC(retval);
     if ((retval = nc_put_att_text(ncid, varids[ivar], "units" , 
				   strlen(Units[ivar]), Units[ivar])))
     ERRNC(retval);
     if ((retval = nc_put_att_text(ncid, varids[ivar], "long_name" , 
				   strlen(Long_Names[ivar]), Long_Names[ivar])))
     ERRNC(retval);
   }
   /* End define mode. */
   if ((retval = nc_enddef(ncid)))
     ERRNC(retval);
   
   
   
   /* Close the file. */
   if ((retval = nc_close(ncid)))
      ERRNC(retval);
   return(0);
}

void xsavenc(int argc, char *argv[]) {
  int it=-1;
  if (argc !=3 && argc!=4) {
    fprintf(stderr,"xsavegradnc : wrong number of arguments");
    return;
  }
  if (argc == 4)
    it = atoi(argv[3]);
  if (!strcmp(argv[2],"grad") )
    savenc(argv[1],Names,Units,Long_Names,grads,it);  
  else {
    savestate();
    savenc(argv[1],SNames,SUnits,SLong_Names,state,it);
  }
}
void xperturb(int argc, char *argv[]) {
  int it = atoi(argv[1]);
  int ix,iy;
  YREAL dx = atof(argv[2]);
  srand(100);
  for (ix=0;ix<YA1_Soce;ix++)
    for (iy=0;iy<YA2_Soce;iy++)
      YS_Hfil(0,ix,iy,it) += randn(0,dx);
  
}

void set_balanced (int argc, char *argv[]) {
  if (!strcmp(argv[1],"true") | !strcmp(argv[1],"TRUE")) {
    balanced = 1;
    fprintf(stdout,"Initial velocities are balanced\n");
  }
 if (!strcmp(argv[1],"FALSE") | !strcmp(argv[1],"FALSE")) {
    balanced = 0;
    fprintf(stdout,"Initial velocities are balanced\n");
  }
  
}
void xload_init(int argc, char *argv[]) {
  // Load initial state from nc file
  // argv[1] filename
  // argv[2] "std" for standard load "incr" to load the linward
  YREAL data[SZY][SZX];
  int std = 1;
  if (argc > 2) 
    std = strcmp(argv[2],"incr") ;
  
  int ix,iy;
  readnc(argv[1],"Hfil",data,0);
  for (ix=0;ix<YA1_Soce;ix++)
    for (iy=0;iy<YA2_Soce;iy++) 
      if (std)
	YS_Hfil(0,ix,iy,0) = data[iy][ix];
      else
	YG_Hfil(0,ix,iy,0) = data[iy][ix] - YS_Hfil(0,ix,iy,0);
  
  readnc(argv[1],"Ufil",data,0);
  for (ix=0;ix<YA1_Soce;ix++)
    for (iy=0;iy<YA2_Soce;iy++) 
      if (std)
	YS_Ufil(0,ix,iy,0) = data[iy][ix];
      else
	YG_Ufil(0,ix,iy,0) = data[iy][ix] - YS_Ufil(0,ix,iy,0);
  readnc(argv[1],"Vfil",data,0);
  for (ix=0;ix<YA1_Soce;ix++)
    for (iy=0;iy<YA2_Soce;iy++) 
      if (std)
	YS_Vfil(0,ix,iy,0) = data[iy][ix];
      else
	YG_Vfil(0,ix,iy,0) = data[iy][ix] - YS_Vfil(0,ix,iy,0);
  
}

void update_incr () {
  for (int it=0;it<YNBALLTIME_Toce;it++)
    for (int ix=0;ix<YA1_Soce;ix++)
      for (int iy = 0 ; iy < YA2_Soce ; iy++) {
	YS_Hfil(0,ix,iy,it)+=YG_Hfil(0,ix,iy,it);
	YS_Ufil(0,ix,iy,it)+=YG_Ufil(0,ix,iy,it);
	YS_Vfil(0,ix,iy,it)+=YG_Vfil(0,ix,iy,it);
	YS_Hphy(0,ix,iy,it)+=YG_Hphy(0,ix,iy,it);
	YS_Uphy(0,ix,iy,it)+=YG_Uphy(0,ix,iy,it);
	YS_Vphy(0,ix,iy,it)+=YG_Vphy(0,ix,iy,it);
	
	
	
      }
}

void linward () {
  Ylinward(0);
}

int savenc(char filename[],char Names[NVARS][STRLEN],
	   char Units[NVARS][STRLEN], char Long_Names[NVARS][STRLEN], 
	   YREAL data[NVARS][YNBALLTIME_Toce][SZY][SZX], int it0) {
  
  int ivar,retval;
  int it,ik;
  int itbeg,itn;
	   
  int ncid;
  int dimids[NDIMS];
  int varids[NVARS];
  size_t start[NDIMS], count[NDIMS];
  /* parametrize the time step */
  if (it0==-1) {//alltimestep
    itbeg = 0;
    itn = YNBALLTIME_Toce;
  }
  else {
    itbeg = it0;
    itn = 1;
  }
  /* Create the file */
  if ((retval = nc_create(filename, NC_CLOBBER, &ncid)))
    ERRNC(retval);
  
  /* Define the dimensions. The record dimension is defined to have
   * unlimited length - it can grow as needed. In this example it is
    * the time dimension.*/
  
  if ((retval = nc_def_dim(ncid, Y_NAME, SZY, &dimids[1])))
      ERRNC(retval);
   if ((retval = nc_def_dim(ncid, X_NAME, SZX, &dimids[2])))
     ERRNC(retval);
   if ((retval = nc_def_dim(ncid, REC_NAME, NC_UNLIMITED, &dimids[0])))
     ERRNC(retval);

   /*Define the netcdf variables */
   for (ivar=0;ivar<NVARS;ivar++) {
     if ((retval = nc_def_var(ncid, Names[ivar], NC_DOUBLE, NDIMS, 
			      dimids, &varids[ivar])))
       ERRNC(retval);
     if ((retval = nc_put_att_text(ncid, varids[ivar], "units" , 
				   strlen(Units[ivar]), Units[ivar])))
       ERRNC(retval);
     if ((retval = nc_put_att_text(ncid, varids[ivar], "long_name" , 
				   strlen(Long_Names[ivar]), Long_Names[ivar])))
       ERRNC(retval);
   }
   /* End define mode. */
   if ((retval = nc_enddef(ncid)))
     ERRNC(retval);
   
   /* These settings tell netcdf to write one timestep of data. (The
    setting of start[0] inside the loop below tells netCDF which
     timestep to write.) */
   count[0]=1;
   count[1]=SZY;
   count[2]=SZX;
   start[0]=0;
   start[1]=0;
   start[2]=0;
   
   //   for (it=0;it<YNBALLTIME_Toce;it++)
   for (it=itbeg ; it < itbeg+itn ; it++)
     //  for (iy=0;iy<YA1_Soce;iy++)
     // for (ix=0;ix<YA2_Soce;ix++)
	 {
	   start [0]=it-itbeg;
	   for (ik=0;ik<6;ik++) 
	     if ((retval = nc_put_vara_double(ncid, varids[ik], start, count, 
					      &data[ik][it][0][0])))
	       ERRNC(retval);
	 }
 
 /* Close the file. */
 if ((retval = nc_close(ncid)))
     ERRNC(retval);
   return(0);
   
}

int readnc (char filename[],char Name[STRLEN],YREAL data[SZY][SZX], int it0) {
  int retval,ncid, data_id;
  size_t start[NDIMS], count[NDIMS];
  int it;
  
  //Open file
   if ((retval = nc_open(filename, NC_NOWRITE, &ncid)))
      ERRNC(retval);

   //Get varids
    if ((retval = nc_inq_varid(ncid, Name, &data_id)))
      ERR(retval);
    count[0]=1;
   count[1]=SZY;
   count[2]=SZX;
   start[0]=0;
   start[1]=0;
   start[2]=0;
   //  for (it=0;it<=it0;it++) {
     start[0]=it0;
     if ((retval = nc_get_vara_double(ncid, data_id, start, 
				     count, &data[0][0])))
       	 ERR(retval);
     // }
   
   
   if ((retval = nc_close(ncid)))
     ERR(retval);
   return(0);
}
#endif


#include <math.h>
#include <stdlib.h>
 
double randn (double mu, double sigma)
{
  double U1, U2, W, mult;
  static double X1, X2;
  static int call = 0;
 
  if (call == 1)
    {
      call = !call;
      return (mu + sigma * (double) X2);
    }
 
  do
    {
      U1 = -1 + ((double) rand () / RAND_MAX) * 2;
      U2 = -1 + ((double) rand () / RAND_MAX) * 2;
      W = pow (U1, 2) + pow (U2, 2);
    }
  while (W >= 1 || W == 0);
 
  mult = sqrt ((-2 * log (W)) / W);
  X1 = U1 * mult;
  X2 = U2 * mult;
 
  call = !call;
 
  return (mu + sigma * (double) X1);
}


