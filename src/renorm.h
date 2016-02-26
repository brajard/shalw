#ifdef RENORM
void compute_Hinv(Matrix<YREAL,Symmetric, RowSymPacked>& Hinv,const Vector<YREAL>& phi, Matrix<YREAL>& Radj, Matrix<YREAL>& Rphi);
void compute_Conv (Vector <YREAL> &Conv, Matrix<YREAL,Symmetric, RowSymPacked>& Hinv, Vector <YREAL> &phi,  Matrix<YREAL>& Rphi);
void compute_minmax(YREAL *pconv_min, YREAL *pconv_max, Vector <YREAL> &Conv);
void compute_phi (Vector <YREAL> &phi, Vector <YREAL> &Conv);
void compute_Rphi ( Matrix<YREAL>& Rphi, Vector <YREAL> &phi, const Matrix<YREAL>& Radj);
int get_indok (const Matrix <YREAL>& Radj, Vector <int> &indok);
void extract_R (Matrix <YREAL> &Rext, const Matrix <YREAL>& Radj, Vector <int> &indok);
void extract_R (Matrix <YREAL> &Rext, const Matrix <YREAL>& Radj, Vector <int> &indok, int nex);
void compute_sol(Vector <YREAL> &X, const Matrix <YREAL>& Rphi, const Matrix <YREAL,Symmetric, RowSymPacked>& Hinv, const Vector <YREAL> &Yobs);
#define SMALL 1e-7


void test_seldon() {
    cout << "Seldon: compilation test with Blas" << endl;

  Vector<double> U(3), V(3);
  U.Fill(1.3);
  V.Fill();

  cout << "U = " << U << endl;
  cout << "V = " << V << endl;

  // 2.0 * U + V -> V
  Add(2.0, U, V);

  // Note: 'Add' calls the Blas function 'daxpy'.

  cout << "2. * U + V = " << V << endl;
}
 

void get_adjoint(int i) {
  //Copy the adjoint corresponding to the ieme observation
  if (Radj.GetN() < i) {
    fprintf(stderr,"error: get_adjoint of observation %d without setting previsous observations\n",i);
  }
  
  if (Radj.GetM() == i) 
    Radj.Resize(i+1,TotS);
  int ind =0;
  for (int iy = 0; iy < SZY ; iy++ ) 
    for (int ix = 0 ; ix < SZX ; ix++ ) {
      Radj(i,ind) = grads[0][0][iy][ix] ;
      ind ++;
    }
}

void set_sol() {
  //Put Sol field in YAO
  int iall =0;
  int iext = 0;
     for (int iy=0;iy<YA1_Soce;iy++)
      for (int ix=0;ix<YA2_Soce;ix++)
	{
	  if (indok(iall) == 1) {
	    YS_Hfil(0,iy,ix,0) = Xa(iext);
	    iext++;
	  }
	  else
	    YS_Hfil(0,iy,ix,0) = 0;
	  iall ++;
	  YS_Hphy(0,iy,ix,0) = 0;
	  YS_Uphy(0,iy,ix,0) = 0;
	  YS_Vphy(0,iy,ix,0) = 0;
	  YS_Vfil(0,iy,ix,0) = 0;
	  YS_Ufil(0,iy,ix,0) = 0;
	}
}

void renorm() {

  //Reallocate memory 
  Yobs.Reallocate(Radj.GetM());
  Xc.Reallocate(TotS);

  //Init Yobs
  for (int i = 0; i<Yobs.GetM();i++)
    Yobs(i) = lobs[i]->val;

  //Check adjoint
  Vector<YREAL> Y(Yobs.GetLength());
  
  //debug
  //cout << "Radj(0,49:50):" << Radj(0,49) << "," << Radj(0,50) << endl;

  int ind=0;
   for (int iy = 0; iy < SZY ; iy++ ) 
     for (int ix = 0 ; ix < SZX ; ix++ ) {
       Xc(ind) = state[0][0][iy][ix] ;
       ind ++;
     }

   //cout << "Xc(49:50):" << Xc(49) << "," << Xc(50) << endl;
   Y.Fill(1);
   //Compute Y = 1.*R*X + 0
   // (-1 because obs is one below the real value)
   MltAdd(1.0,Radj,Xc,0.0,Y) ;
   cout << "computed: " << Y << endl ;
   cout << "loaded:" << Yobs << endl ;

  
   // Select elements in the field (max of adjoint > SMALL)   
   Matrix <YREAL> Rext ;
   int nex = get_indok(Radj,indok);
   extract_R (Rext,Radj,indok, nex);


   Radj.WriteText("Radj.dat");
   Rext.WriteText("Rext.dat");
   indok.WriteText("indok.dat");
   Xc.WriteText("Xt.dat");
   Y.WriteText("Y.dat");
   
   Xc.Fill(0);
   Xa.Reallocate(nex);
//Inverse of H
   Matrix <YREAL,Symmetric, RowSymPacked> Hinv(Yobs.GetM(),Yobs.GetM());
   Vector <YREAL> phi(nex);
   Matrix <YREAL> Rphi(Rext);
   Vector <YREAL> Ecl(nex);
   //Conv = r*H-1*r
   Vector <YREAL> Conv(nex);
   
   //Solution without renormalization
   phi.Fill(1);
   compute_Hinv ( Hinv, phi, Rext , Rext) ;
   compute_sol(Xa,Rphi,Hinv,Yobs);
   Xa.WriteText("Xa_init.dat");
   if (maxiter!=0) {
     

   //Initialisation
   int iter =1 ;
   YREAL conv_min = 0 ;
   YREAL conv_max = 1 ;
 
   compute_Conv ( Conv, Hinv, phi, Rext );
   compute_minmax (&conv_min, &conv_max, Conv);
   cout << "min=" << conv_min <<", max=" << conv_max <<endl;
   


   while((conv_min < 0.98 || conv_max>0.99) && iter <  maxiter ) {
     compute_phi  ( phi, Conv) ;
     compute_Rphi ( Rphi, phi, Rext) ;
     compute_Hinv ( Hinv, phi, Rext , Rphi ) ;
     compute_Conv ( Conv, Hinv, phi, Rphi );
     compute_minmax (&conv_min, &conv_max, Conv);

     iter ++;
   cout << "iter " << iter << " min=" << conv_min <<", max=" << conv_max <<endl;

   } //while
   compute_sol(Xa,Rphi,Hinv,Yobs);
   phi.WriteText("Phi.dat");
   Xa.WriteText("Xa_fin.dat");
   } // if Y3nsim !=0
}

void extract_R (Matrix <YREAL> &Rext, const Matrix <YREAL>& Radj, Vector <int> &indok) {
  int nex = DotProd(indok,indok);
  extract_R (Rext,Radj,indok, nex);
}

void compute_sol(Vector <YREAL> &X, const Matrix <YREAL>& Rphi, const Matrix <YREAL,Symmetric, RowSymPacked>& Hinv, const Vector <YREAL> &Yobs){
  Matrix <YREAL> M(Rphi.GetM(),Rphi.GetN());
  MltAdd(1.0,Hinv,Rphi,0.0,M) ;
  MltAdd(1.0,SeldonTrans,M,Yobs,0.0,X);
}

void extract_R (Matrix <YREAL> &Rext, const Matrix <YREAL>& Radj, Vector <int> &indok, int nex) {
  cout << "Number of grod point seen : " << nex << endl;
  int n = Radj.GetN();
  int m = Radj.GetM();
  
  Rext.Reallocate(m,nex);
  Vector <YREAL> col;
  int   jex = 0;
  for (int j = 0 ; j < n ; j++) 
    if (indok(j) == 1) {
      GetCol(Radj,j,col);
      SetCol(col,jex,Rext);
      jex++;
  }
}

int get_indok (const Matrix <YREAL>& Radj, Vector <int> &indok) {
  //Return a vector of size n with 0 if the grid point is not seen and 1 otherwise
int n = Radj.GetN(); //nb gridpoint
  indok.Reallocate(n);
  Vector <YREAL> col ;
  int nex=0;
  int imax;
  for (int j = 0 ; j < n ; j++) {
     GetCol(Radj,j,col);
     imax = GetMaxAbsIndex(col);
     if (fabs(col(imax)) <= SMALL)
       indok(j) = 0 ;
     else {
       indok(j) = 1 ;  
       nex++;
     }
  }
  return(nex);
}

void compute_Rphi ( Matrix<YREAL>& Rphi, Vector <YREAL> &phi, const Matrix<YREAL>& Radj) {
  int n = Radj.GetN(); //nb gridpoint
  Vector <YREAL> col;
  for (int j = 0 ; j < n ; j++) {
    GetCol(Radj,j,col);
    Mlt(1./phi(j),col);
    SetCol(col,j,Rphi);
  }
}

void compute_phi (Vector <YREAL> &phi, Vector <YREAL> &Conv) {
  for (int i=0 ; i < phi.GetLength() ; i++) {
    phi(i) = phi(i)*sqrt(Conv(i));
  }
}

void compute_minmax(YREAL *pconv_min, YREAL *pconv_max, Vector <YREAL> &Conv) {
  *pconv_min = Conv(0);
  *pconv_max = Conv(0);
  for (int i = 1 ; i < Conv.GetLength() ; i++) {
    if (Conv(i) < *pconv_min) *pconv_min = Conv(i);
    if (Conv(i) > *pconv_max) *pconv_max = Conv(i);
  }
}

void compute_Conv (Vector <YREAL> &Conv, Matrix<YREAL,Symmetric, RowSymPacked>& Hinv, Vector <YREAL> &phi,  Matrix<YREAL>& Rphi) {
  int m = Rphi.GetM(); //nb obs
  int n = Rphi.GetN(); //nb gridpoint
  Vector <YREAL> row1, col2 ;
  Matrix <YREAL, General, ColMajor> HinvR(m,n);
  MltAdd(1.0,Hinv,Rphi,0,HinvR);
  for (int i = 0 ; i < n ; i++) {
    GetCol(Rphi,i,row1);
    GetCol(HinvR,i,col2);
    Conv(i) = sqrt(DotProd(row1,col2));
  }  

}

void compute_Hinv (Matrix<YREAL,Symmetric, RowSymPacked>& Hinv,const Vector<YREAL>& phi, Matrix<YREAL>& Radj, Matrix<YREAL>& Rphi) {
  
  Vector <YREAL> rowi, rowj;
  
  Hinv.Zero();

  int m = Radj.GetM(); //nb obs
  int n = Radj.GetN(); //nb gridpoint
  for (int i = 0 ; i< m ; i++ ) 
  for (int j= 0 ; j<= i ; j++ ) {
    //Don't copy memory
    rowi.SetData(n, &Radj(i,0));
    rowj.SetData(n, &Rphi(j,0));
    Hinv(i,j) = DotProd(rowi,rowj);

    //Make the pointer null to avoid free of memory in Radj
    rowi.Nullify();
    rowj.Nullify();
  }


  GetInverse(Hinv);

  
}

#else
void renorm(){}
void set_sol(){}

#endif
