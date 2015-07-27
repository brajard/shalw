#define GEOSTROPHY
#define USE_NETCDF
#ifdef USE_NETCDF
#include <netcdf.h>

/* Dim max des strings */
#define STRLEN 100

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
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}


#endif



//Declaration 
extern int Yobs_insert_data (char *nmmod, int sortie, int iaxe, int jaxe, int kaxe,
			     int pdt, YREAL val);

double dx,dy,dedt,svdedt,pcor,grav,dissip,hmoy,alpha,gb,gmx,gsx,gmy,gsy;
void savestate();

#ifdef USE_NETCDF
int ncinit();
int savegradnc();
void xsavenc(int argc, char *argv[]);
int savenc(char filename[],char Names[NVARS][STRLEN],
	   char Units[NVARS][STRLEN], char Long_Names[NVARS][STRLEN],
	    YREAL data[NVARS][YNBALLTIME_Toce][SZY][SZX]);

#endif

YREAL grads[6][YNBALLTIME_Toce][SZY][SZX];
YREAL state[6][YNBALLTIME_Toce][SZY][SZX];

void savegrad(int it);

void xdisplay();
void appli_start(int argc, char *argv[]){
#ifdef USE_NETCDF
  // ncinit();
#endif

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

void xdisplay(){
	int i,j;
	i=(int)(SZX/2);
	j=(int)(SZY/2);
	printf("point:%i,%i, Hfil=% -23.15e @J/@Hfil=% -23.15e\n", i,j,
			YS_Hfil(0,i,j,100), YG_Hfil(0,i,j,100));
}

void xgauss(int argc, char *argv[]){
	gb = atof(argv[1]);
	gmx = atof(argv[2]); gsx = atof(argv[3]);
	gmy = atof(argv[4]); gsy = atof(argv[5]);
        for (int j = 0; j<SZY; j++)
	  for (int i = 0; i<SZX; i++)
	   YS_Hfil(0,i,j,0) = gb*(exp( -pow((i*dx-gmx)/gsx,2) /2.))
			    *(exp(-pow((j*dy-gmy)/gsy,2) /2.));
}

void xvitgeo(){
	double gf, gfh;
#ifdef GEOSTROPHY
	fprintf(stderr,"xvitgeo : Warning, geostrophy computation is included in the model\n");
#endif
	gf = (grav / pcor);
	for(int j=0; j<SZY; j++)
	  for(int i=0; i<SZX; i++){ 
	    gfh=gf*YS_Hfil(0,i,j,0);
	    YS_Ufil(0,i,j,0)=gfh*(j*dy-gmy)/(gsy*gsy);
            YS_Vfil(0,i,j,0)=gfh*-(i*dx-gmx)/(gsx*gsx);
	  }
}

void xivg(int argc,char *argv[]){
	double val;
	val = atof(argv[2]);
			
	if       (strcmp(argv[1], "dt") == 0) dedt=val;
	else if  (strcmp(argv[1], "dx") == 0) dx=val;
	else if  (strcmp(argv[1], "dy") == 0) dy=val;
	else if  (strcmp(argv[1], "pcor") == 0) pcor=val;
	else if  (strcmp(argv[1], "grav") == 0) grav=val;
	else if  (strcmp(argv[1], "dissip") == 0) dissip=val;
	else if  (strcmp(argv[1], "hmoy") == 0) hmoy=val;
	else if  (strcmp(argv[1], "alpha") == 0) alpha=val;
}

void savestate() {
 int it,ix,iy;
  for (it=0;it<YNBALLTIME_Toce;it++)
    for (iy=0;iy<YA1_Soce;iy++)
      for (ix=0;ix<YA2_Soce;ix++)
	{
	  state[0][it][iy][ix]=YS_Hfil(0,iy,ix,it);
	  state[1][it][iy][ix]=YS_Ufil(0,iy,ix,it);
	  state[2][it][iy][ix]=YS_Vfil(0,iy,ix,it);
	  state[3][it][iy][ix]=YS_Hphy(0,iy,ix,it);
	  state[4][it][iy][ix]=YS_Uphy(0,iy,ix,it);
	  state[5][it][iy][ix]=YS_Vphy(0,iy,ix,it);	 
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
    for (iy=0;iy<YA1_Soce;iy++)
      for (ix=0;ix<YA2_Soce;ix++)
	{
	  fprintf(fid,"%d %d %d ",it,iy,ix);
	  fprintf(fid,"%g %g %g %g %g %g\n",YS_Hfil(0,iy,ix,it),YS_Ufil(0,iy,ix,it),YS_Vfil(0,iy,ix,it),YS_Hphy(0,iy,ix,it),YS_Uphy(0,iy,ix,it),YS_Vphy(0,iy,ix,it));
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
     ERR(retval);
   if ((retval=nc_inq_varids(ncid,&nvar,varids)))
     ERR(retval);
   for (it=0;it<YNBALLTIME_Toce;it++)
     for (iy=0;iy<YA1_Soce;iy++)
       for (ix=0;ix<YA2_Soce;ix++)
	 {
	   start [0]=it;
	   for (ik=0;ik<6;ik++) 
	     if ((retval = nc_put_vara_double(ncid, varids[ik], start, count, 
					     &grads[ik][it][0][0])))
	       ERR(retval);
	       }
   
   /* Close the file. */
   if ((retval = nc_close(ncid)))
     ERR(retval);
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
#ifdef USE_NETCDF
int ncinit() {
  int ivar,retval;
  int ncid;
  int dimids[NDIMS];
  int varids[NVARS];
  /* Create the file */
  if ((retval = nc_create(FILENAME, NC_CLOBBER, &ncid)))
      ERR(retval);

  /* Define the dimensions. The record dimension is defined to have
    * unlimited length - it can grow as needed. In this example it is
    * the time dimension.*/
 
   if ((retval = nc_def_dim(ncid, Y_NAME, SZY, &dimids[1])))
      ERR(retval);
   if ((retval = nc_def_dim(ncid, X_NAME, SZX, &dimids[2])))
      ERR(retval);
   if ((retval = nc_def_dim(ncid, REC_NAME, NC_UNLIMITED, &dimids[0])))
      ERR(retval);

   /*Define the netcdf variables */
   for (ivar=0;ivar<NVARS;ivar++) {
     if ((retval = nc_def_var(ncid, Names[ivar], NC_DOUBLE, NDIMS, 
			      dimids, &varids[ivar])))
       ERR(retval);
     if ((retval = nc_put_att_text(ncid, varids[ivar], "units" , 
				   strlen(Units[ivar]), Units[ivar])))
     ERR(retval);
     if ((retval = nc_put_att_text(ncid, varids[ivar], "long_name" , 
				   strlen(Long_Names[ivar]), Long_Names[ivar])))
     ERR(retval);
   }
   /* End define mode. */
   if ((retval = nc_enddef(ncid)))
     ERR(retval);
   
   
   
   /* Close the file. */
   if ((retval = nc_close(ncid)))
      ERR(retval);
   return(0);
}

void xsavenc(int argc, char *argv[]) {
  if (argc !=3) {
    fprintf(stderr,"xsavegradnc : wrong number of arguments");
    return;
  }
  if (!strcmp(argv[2],"grad") )
    savenc(argv[1],Names,Units,Long_Names,grads);
  
  else {
    savestate();
    savenc(argv[1],SNames,SUnits,SLong_Names,state);
  }
}

int savenc(char filename[],char Names[NVARS][STRLEN],
	   char Units[NVARS][STRLEN], char Long_Names[NVARS][STRLEN], 
	   YREAL data[NVARS][YNBALLTIME_Toce][SZY][SZX]) {
  
  int ivar,retval;
  int it,ik;
  int ncid;
  int dimids[NDIMS];
  int varids[NVARS];
  size_t start[NDIMS], count[NDIMS];
  /* Create the file */
  if ((retval = nc_create(filename, NC_CLOBBER, &ncid)))
    ERR(retval);
  
  /* Define the dimensions. The record dimension is defined to have
   * unlimited length - it can grow as needed. In this example it is
    * the time dimension.*/
  
  if ((retval = nc_def_dim(ncid, Y_NAME, SZY, &dimids[1])))
      ERR(retval);
   if ((retval = nc_def_dim(ncid, X_NAME, SZX, &dimids[2])))
     ERR(retval);
   if ((retval = nc_def_dim(ncid, REC_NAME, NC_UNLIMITED, &dimids[0])))
     ERR(retval);

   /*Define the netcdf variables */
   for (ivar=0;ivar<NVARS;ivar++) {
     if ((retval = nc_def_var(ncid, Names[ivar], NC_DOUBLE, NDIMS, 
			      dimids, &varids[ivar])))
       ERR(retval);
     if ((retval = nc_put_att_text(ncid, varids[ivar], "units" , 
				   strlen(Units[ivar]), Units[ivar])))
       ERR(retval);
     if ((retval = nc_put_att_text(ncid, varids[ivar], "long_name" , 
				   strlen(Long_Names[ivar]), Long_Names[ivar])))
       ERR(retval);
   }
   /* End define mode. */
   if ((retval = nc_enddef(ncid)))
     ERR(retval);
   
   /* These settings tell netcdf to write one timestep of data. (The
    setting of start[0] inside the loop below tells netCDF which
     timestep to write.) */
   count[0]=1;
   count[1]=SZY;
   count[2]=SZX;
   start[0]=0;
   start[1]=0;
   start[2]=0;
   
   for (it=0;it<YNBALLTIME_Toce;it++)
     //  for (iy=0;iy<YA1_Soce;iy++)
     // for (ix=0;ix<YA2_Soce;ix++)
	 {
	   start [0]=it;
	   for (ik=0;ik<6;ik++) 
	     if ((retval = nc_put_vara_double(ncid, varids[ik], start, count, 
					      &data[ik][it][0][0])))
	       ERR(retval);
	 }
 
 /* Close the file. */
 if ((retval = nc_close(ncid)))
     ERR(retval);
   return(0);
   
}

#endif
