void compute_Hinv(Matrix<YREAL,Symmetric, RowSymPacked>& Hinv,const Vector<YREAL>& phi, Matrix<YREAL>& Radj);

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
   //Compute Y = 1.*R*X + 1 
   // (-1 because obs is one below the real value)
   MltAdd(1.0,Radj,Xc,-1.0,Y) ;
   cout << "computed: " << Y << endl ;
   cout << "loaded:" << Yobs << endl ;

   Xc.Fill(0);
   //Inverse of H
   Matrix <YREAL,Symmetric, RowSymPacked> Hinv(Yobs.GetM(),Yobs.GetM());
   Vector <YREAL> phi(TotS);
   Vector <YREAL> Ecl(TotS);
   //Conv = r*H-1*r
   Vector <YREAL> Conv(TotS);
   
   //Initialisation
   int iter =1 ;
   int conv_min = 0 ;
   int conv_max = 1 ;
   phi.Fill(1);
   compute_Hinv ( Hinv, phi, Radj ) ;
   /*
   compute_Conv ( Conv, Hinv, phi, Radj );
   compute_minmax (&conv_min, &conv_max, Conv);

   while(conv_min < 0.98 && conv_max>0.99) {
     compute_phi  ( phi, Conv) ;
     compute_Hinv ( Hinv, phi, Radj ) ;
     compute_Conv ( Conv, Hinv, phi, Radj );
     compute_minmax (&conv_min, &conv_max, Conv);
     iter ++;
   } //while
   */
}
  
void compute_Hinv (Matrix<YREAL,Symmetric, RowSymPacked>& Hinv,const Vector<YREAL>& phi, Matrix<YREAL>& Radj) {
  Matrix <YREAL,Symmetric, RowSymPacked> H(Hinv.GetM(),Hinv.GetN());
  Vector <YREAL> rowi(phi.GetLength()), rowj(phi.GetLength()) ;
  for (int i = 0 ; i< Hinv.GetM() ; i++) 
    for (int j= 0 ; j<= i ; j++ ) {
      GetRow(Radj,i,rowi);
      GetRow(Radj,j,rowj);
      
      H(i,j) = DotProd(rowi,rowj); 
    }
  cout << H << endl;
  GetInverse(H);
  cout << H << endl;

  
}

