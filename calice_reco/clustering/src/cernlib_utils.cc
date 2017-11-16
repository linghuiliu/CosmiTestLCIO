#include <cmath>
#include <cstring>

using namespace std;

/*
 * RNDM function is taken from CERNLIB/kernlib to make
 * possible results comparision. It generate exactly the same
 * sequence of numbers as original.
 *
 * C adoptation: A. Zhelezov
 */
float rndm(int iseed){
  #define MSK1 0x0c000000
  #define MSK2 0x33000000
  static int MCGN=12345;

  MCGN=MCGN*69069;
  union {
    float AMAN;
    int   MANT;
  } u;

  u.MANT=(MCGN>>8)&0x00ffffff;
  
  if(u.MANT){
    u.AMAN=(float)u.MANT;
    u.MANT-=MSK1;
  } else 
    u.MANT=MSK2;
  return u.AMAN;
}

/*
 * CPP equivalant for CERNLIB/GENLIB function EISRS1
 *
 * Translated by A.Zhelezov, DESY/ITEP, 2007
 * NOTE: tested with 3x3 matrix only!
 */

void TRED2(int NM,int N,float *A,float *D,float *E,float *Z){
  // A(NM,N),D(N),E(N),Z(NM,N)
  ::memcpy(Z,A,sizeof(*A)*(N*NM));

  for(int II=1;II<N;II++){
    int I = N - II;
    int L = I - 1;
    float H = 0.;
    float SCALE = 0.;
    if(L > 0) {
      for(int K=0;K<I;K++)
	SCALE += ::fabs( Z[I*N+K] ); // Z[I][K]
    }
    if(SCALE == 0.){
      E[I] = Z[I*N+L];
      D[I] = H;
      continue;
    }
    for(int K=0;K<I;K++){
      Z[I*N+K] /= SCALE;
      H+=Z[I*N+K]*Z[I*N+K];
    }
    float F = Z[I*N+L]; // Z[I][L]
    float G = (F>=0.)?-::sqrt(H) : ::sqrt(H);
    E[I] = SCALE * G;
    H -= F * G;
    Z[I*N+L] = F - G;
    F = 0.;
    for(int J=0;J<I;J++){
      Z[J*N+I]=Z[I*N+J] / (SCALE*H);
      G = 0.;
      for(int K=0;K<=J;K++)
	G += Z[J*N+K] * Z[I*N+K];
      for(int K=J+1;K<I;K++)
	G += Z[K*N+J] * Z[I*N+K];
      E[J] = G / H;
      F += E[J] * Z[I*N+J];
    }
    float HH = F / (H + H);
    for(int J=0;J<I;J++){
      F = Z[I*N+J];
      G = E[J] - HH * F;
      E[J] = G;
      for(int K=0;K<=J;K++)
	Z[J*N+K] = Z[J*N+K] - F * E[K] - G * Z[I*N+K];
    }
    for(int K=0;K<I;K++)
      Z[I*N+K] *= SCALE;
    D[I] = H;
  }
  D[0] = E[0] = 0.;
  for(int I=0;I<N;I++){
    if(D[I] != 0.) {
      for(int J=0;J<I;J++){
	float G=0.;
	for(int K=0;K<I;K++)
	  G+=Z[I*N+K]*Z[K*N+J];
	for(int K=0;K<I;K++)
	  Z[K*N+J] -= G*Z[K*N+I];
      }
    }
    D[I] = Z[I*N+I];
    Z[I*N+I] = 1.;
    for(int J=0;J<I;J++){
      Z[I*N+J] = 0.;
      Z[J*N+I] = 0.;
    }
  }
}

void TQL2(int NM,int N,float *D,float *E,float *Z,int &IERR){
  // D(N),E(N),Z(NM,N)
  float MACHEP=::pow(2.,-23);
  IERR = 0;
  if (N == 1)
    return;
  for(int I=1;I<N;I++)
    E[I-1] = E[I];
  float F = 0.;
  float B = 0.;
  E[N-1] = 0.;
  for(int L=0;L<N;L++){
    int J = 0;
    float H = MACHEP * (::fabs(D[L]) + ::fabs(E[L]));
    if(B < H)
      B = H;
    int M;
    for(M=L;M<N;M++)
      if(::fabs(E[M])<=B)
	break;
    if(M==N)
      M--;
    if(M != L) {
      do {
	if(J == 30){
	  IERR=L;
	  return;
	}
	J++;
	float P = (D[L+1] - D[L]) / (2.0 * E[L]);
	float R = ::sqrt(P*P+1.0);
	H = D[L] - E[L] / (P + (P>=0?R:-R));
	for(int I=L;I<N;I++)
	  D[I] -= H;
	F += H;
	P = D[M];
	float C = 1.0;
	float S = 0.0;
	int MML = M - L;
	for(int II=0;II<MML;II++){
	  int I = M - II - 1;
	  float G = C * E[I];
	  H = C * P;
	  if(::fabs(P) >= ::fabs(E[I])){
	    C = E[I] / P;
	    R = ::sqrt(C*C+1.);
	    E[I+1] = S * P * R;
	    S = C / R;
	    C = 1.0 / R;
	  } else {
	    C = P / E[I];
	    R = ::sqrt(C*C+1.0);
	    E[I+1] = S * E[I] * R;
	    S = 1.0 / R;
	    C = C * S;
	  }
	  P = C * D[I] - S * G;
	  D[I+1] = H + S * (C * G + S * D[I]);
	  for(int K=0;K<N;K++){
	    H = Z[K*N+I+1];
	    Z[K*N+I+1] = S * Z[K*N+I] + C * H;
	    Z[K*N+I] = C * Z[K*N+I] - S * H;
	  }
	}
	E[L] = S * P;
	D[L] = C * P;
      } while(::fabs(E[L]) > B);
    }
    D[L] += F;
  }
  for(int II=1;II<N;II++){
    int I = II - 1;
    int K = I;
    float P = D[I];
    for(int J=II;J<N;J++){
      if(D[J] >= P)
	continue;
      K = J;
      P = D[J];
    }
    if(K==I)
      continue;
    D[K] = D[I];
    D[I] = P;
    for(int J=0;J<N;J++){
      P = Z[J*N+I];
      Z[J*N+I] = Z[J*N+K];
      Z[J*N+K] = P;
    }
  }
}

void EISRS1(int NM,int N,float *AR,float *WR,float *ZR,int &IERR,float *WORK){
  // ALL EIGENVALUES AND CORRESPONDING EIGENVECTORS OF A REAL
  // SYMMETRIC MATRIX
  // AR(NM,NM),WR(N),ZR(NM,NM),WORK(1)
  TRED2(NM,N,AR,WR,WORK,ZR);
  TQL2(NM,N,WR,WORK,ZR,IERR);
}
