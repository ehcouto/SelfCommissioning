/*
 * File: ThermalModelCore.c
 *
 * Code generated for Simulink model 'ThermalModelCore'.
 *
 * Model version                  : 1.6
 * Simulink Coder version         : 8.11 (R2016b) 25-Aug-2016
 * C/C++ source code generated on : Thu Nov 02 16:15:00 2017
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "ThermalModelCore_private.h"
#include <math.h>
#include "ThermalModelCoreDD.h"
#define TIMESTEP                       2.500000e-02f
#define EVTITER                        10
#define EVTHYST                        1.000000e-06f
#define NDIFF                          6
#define NDFA                           6
#define NEQ                            42
#define NPAR                           48
#define NDPAR                          2
#define NINP                           5
#define NDISC                          4
#define NIX1                           32
#define NOUT                           5
#define NCON                           0
#define NEVT                           2
#ifdef EVTHYST
#define NZC                            2*NEVT
#else
#define NZC                            NEVT
#endif

#define INIDREF                        (2*NEVT+2*NZC)
#define sin                            sinf
#define cos                            cosf
#define tan                            tanf
#define asin                           asinf
#define acos                           acosf
#define atan                           atanf
#define atan2                          atan2f
#define sinh                           sinhf
#define cosh                           coshf
#define tanh                           tanhf
#define pow                            powf
#define exp                            expf
#define log                            logf
#define log10                          log10f
#define sqrt                           sqrtf
#define ceil                           ceilf
#define floor                          floorf

//static real32_T dsn_zero= 0.0f;
//static unsigned char dsn_undefC[4] = { 0, 0, 0xC0, 0x7F };
//
//static real32_T *dsn_undef = (real32_T *)&dsn_undefC;
//static unsigned char dsn_posinfC[4] = { 0, 0, 0x80, 0x7F };
//
//static real32_T *dsn_posinf = (real32_T *)&dsn_posinfC;
//static unsigned char dsn_neginfC[4] = { 0, 0, 0x80, 0xFF };
//
//static real32_T *dsn_neginf = (real32_T *)&dsn_neginfC;

#define trunc(v)                       ( (v>0.0f) ? floor(v) : ceil(v) )
//#define IS_UNDEF(a)                    (a-a!=0.f || (a!=0.f && a-2.f*a==0.f))

static real32_T TimeStep = 0.0f;

OTE2_PARAMS_DD_TYPE* ThermalModelCoreDD_Params;

/* Block signals (auto storage) */
B_ThermalModelCoreDD_T ThermalModelCoreDD_B;

/* Block states (auto storage) */
DW_ThermalModelCoreDD_T ThermalModelCoreDD_DW;

/* External inputs (root inport signals with auto storage) */
ExtU_ThermalModelCoreDD_T ThermalModelCoreDD_U;

/* External outputs (root outports fed by signals with auto storage) */
ExtY_ThermalModelCoreDD_T ThermalModelCoreDD_Y;
#pragma optimize=size no_inline no_unroll
static void fp(int_T N, real32_T T, real32_T *Y, real32_T *YP)
{
  real32_T Z[9];
  Z[0] = Y[39];
  Y[15] = Y[0]-Z[0]-273.15f;
  Z[1] = Y[37];
  Y[17] = fabsf((Z[1]*(Z[1]*Y[83]+Y[82])+Y[81])*Z[1]);
  Z[2] = Y[60]*Y[60];
  Z[3] = Y[66]*Y[66];
  Z[4] = Y[17]*Y[17];
  Z[2] = Z[2]*Z[2]*Z[4]*Z[4]+Z[3]*Z[3];
  Y[16] = 1e-06f*Y[42]*Y[47]*pow(Z[2],0.25f);
  Y[18] = -Y[16]*Y[15];
  Y[9] = Y[2]-Z[0]-273.15f;
  Y[11] = fabsf((Z[1]*(Z[1]*Y[80]+Y[79])+Y[78])*Z[1]);
  YP[0] = (1.f*(Y[18]+2.f*Y[46]*(Y[4]+Y[5])-4.f*Y[46]*Y[0]))/(Y[69]*Y[55]);
  Z[2] = (Y[3]-Y[1])*Y[77];
  Z[3] = (Y[2]-Y[1])*Y[73];
  Z[4] = 1.0f/Y[70];
  Z[5] = 1.0f/Y[57];
  Z[6] = 0.002469135802469135802f;
  YP[1] = Z[6]*(Z[2]+Z[3])*Z[5]*Z[4];
  Z[6] = Y[59]*Y[59];
  Z[7] = Y[65]*Y[65];
  Z[8] = Y[11]*Y[11];
  Z[6] = Z[6]*Z[6]*Z[8]*Z[8]+Z[7]*Z[7];
  Y[10] = 1e-06f*Y[45]*pow(Z[6],0.25f);
  Y[12] = -(Y[9]*Y[10]+Z[3]);
  Y[8] = fabsf((Z[1]*(Z[1]*Y[76]+Y[75])+Y[74])*Z[1]);
  YP[2] = 0.04444444444444444444f*Y[12]*Z[5]*Z[4];
  Y[7] = 1e-06f*Y[44]*Y[47]*Y[64]*Y[8]*(Y[3]-Y[4]);
  Z[3] = Y[3]*Y[3];
  Z[6] = Y[4]*Y[4];
  Y[13] = -(Z[2]+Y[7])+3.685738550000000334e-14f*Y[44]*Y[47]*(Z[6]*Z[6]-Z[3]*Z[3]);
  Y[6] = Z[0]-Y[4]+273.15f;
  Y[19] = fabsf((Z[1]*(Z[1]*Y[86]+Y[85])+Y[84])*Z[1]);
  YP[3] = 0.04444444444444444444f*Y[13]*Z[5]*Z[4];
  Y[14] = Y[58]*Y[19]-Y[13]-Z[2]+2.f*Y[46]*(Y[0]-Y[4])+1e-12f*Y[44]*Y[44]*Y[47]*
    Y[47]*(1.f-Y[64])*Y[64]*Y[8]*Y[6];
  Z[2] = Y[5]-Z[0]-273.15f;
  Y[20] = Z[2];
  Y[23] = fabsf((Z[1]*(Z[1]*Y[89]+Y[88])+Y[87])*Z[1]);
  Y[25] = -Z[2];
  YP[4] = Y[14]/(450.f*Y[71]);
  Y[24] = Y[67]+Y[62]*pow(fabsf(Y[25])+0.1f,Y[68]);
  Z[1] = Y[61]*Y[61];
  Z[2] = Y[24]*Y[24];
  Z[3] = Y[23]*Y[23];
  Z[1] = Z[1]*Z[1]*Z[3]*Z[3]+Z[2]*Z[2];
  Y[22] = 1e-06f*Y[43]*Y[47]*pow(Z[1],0.25f);
  Y[21] = Y[22]*Y[20];
  Z[0] = Z[0]+273.15f;
  Z[1] = Y[5]*Y[5];
  Z[0] = Z[0]*Z[0];
  Y[26] = Y[21]+3.685738550000000334e-14f*Y[43]*Y[47]*Y[63]*(Z[1]*Z[1]-Z[0]*Z[0]);
  Z[0] = Y[36];
  Z[0] = Z[0]*Z[0]*Y[53];
  Y[27] = -Y[26]+2.f*Y[46]*(Y[0]-Y[5])+3.f*Z[0]*(1.f+(Y[5]-Y[52])*Y[54])-819.45f*
    Z[0]*Y[54];
  YP[5] = Y[27]/(Y[72]*Y[56]);
}
//#pragma optimize=size no_inline no_unroll
//static void otp(real32_T T, real32_T *Y, real32_T *YP)
//{
//  Y[28] = Y[1]-273.15f;
//  Y[29] = Y[5]-273.15f;
//  Y[30] = Y[39];
//  Y[31] = Y[4]-273.15f;
//  Y[32] = Y[38];
//}
#pragma optimize=size no_inline no_unroll
static void eev(real32_T T, real32_T *Y, real32_T *Ypre, real32_T *EA)
{
  EA[0] = Y[34]-0.1f;
  EA[1] = Y[35]-0.1f;
}
#pragma optimize=size no_inline no_unroll
static int_T eex(int_T N, real32_T T, real32_T *Y, real32_T *Ypre)
{
  real32_T Z[1];
  if (N==1 )
    Y[40] = 1.f-Y[40];
  else if (N==2 )
    Y[41] = 1.f-Y[41];
  else if (N==3 ) {
    Z[0] = Y[33];
    if ((Y[40]==1.f) && (Y[40]!=Ypre[40]) ) {
      Y[38] = Y[5]-Y[33]-273.15f;
      Y[5] = Y[33]+273.15f;
      Y[4] = Y[4]-Y[38]*Y[51];
      Y[0] = Y[0]-Y[38]*Y[51];
      Y[1] = Y[1]-Y[38]*Y[50];
      Y[3] = Y[3]-Y[38]*Y[50];
      Y[2] = Y[2]-Y[38]*Y[50];
      Y[39] = Ypre[39]-Y[38];
    }

    if ((Y[41]==1.f) && (Y[41]!=Ypre[41]) ) {
      Y[38] = 0.f;
      Y[5] = Y[33]+273.15f;
      Y[4] = Y[33]-Y[49]+273.15f;
      Y[0] = Y[33]-Y[49]+273.15f;
      if (Z[0]<Y[48] )
        Y[39] = Z[0];
      else
        Y[39] = Y[48];
      Y[1] = Y[39]+273.15f;
      Y[3] = Y[39]+273.15f;
      Y[2] = Y[39]+273.15f;
    }
  }

  return(0);
}
//#pragma optimize=size no_inline no_unroll
//static int_T cpr(real32_T T, real32_T *Y)
//{
//  real32_T v;
//  int_T k;
//  k = 0;
//  if (Y[34]>=0.1f )
//    v = 1.f;
//  else
//    v = 0.f;
//  if (Y[40]!=v ) {
//    Y[40] = v;
//    k = 1;
//  }
//
//  if (Y[35]>=0.1f )
//    v = 1.f;
//  else
//    v = 0.f;
//  if (Y[41]!=v ) {
//    Y[41] = v;
//    k = 1;
//  }
//
//  return(k);
//}
static int savespace(real32_T *w, int_T *modes)
{
	real32_T t,*y,*yp,*ypre,*tval;
	int_T *oldmodes,*inc,*req;
	t= w[0];
	y= &w[1];
	yp= &y[NEQ+NPAR];
	ypre= &yp[NDFA];
	tval= &ypre[NEQ];
	oldmodes= &modes[NZC];
	inc= &oldmodes[NZC];
	req= &inc[NEVT];
	int i,flag;
	eev(t,y,ypre,tval);			//tval either -0.1 or 0.9
	flag= 0;
	for (i=0;i<NEVT;i++) {
	  if (tval[i]>EVTHYST && modes[2*i]==0 ) {
		modes[2*i]= 1;
		if (req[i]>0 && y[req[i]-1]==1.0f)
		  oldmodes[2*i]= 1;
		else
		  flag= 1;
	  } else if (tval[i]<EVTHYST && modes[2*i]==1 ) {
		modes[2*i]= 0;
		oldmodes[2*i]= 0;
	  }

	  if (tval[i]>-EVTHYST && modes[2*i+1]==0) {
		modes[2*i+1]= 1;
		oldmodes[2*i+1]= 1;
	  } else if (tval[i]<-EVTHYST && modes[2*i+1]==1 ) {
		modes[2*i+1]= 0;
		if (inc[i] || (req[i]>0 && y[req[i]-1]==0.0f))
		  oldmodes[2*i+1]= 0;
		else
		  flag= 1;
	  }
	}
	return flag;
}
#pragma optimize=size no_inline no_unroll
static int_T RunEvents(real32_T *w, int_T *modes, int_T first, int_T termvar,
  int_T maxiter)
{
  real32_T t,*y,*yp,*ypre,*tval;
  int_T *m= NULL,*oldmodes,*inc,*req;
  int_T i,flag,iter,evt,rc,failskip;
  t= w[0];
  y= &w[1];
  yp= &y[NEQ+NPAR];
  ypre= &yp[NDFA];
  tval= &ypre[NEQ];
  oldmodes= &modes[NZC];
  inc= &oldmodes[NZC];
  req= &inc[NEVT];
//  for (i=0;i<NEQ;i++)
//    if (IS_UNDEF(y[i])) {
//      return(-3);
//    }

  for (i=0;i<NZC;i++)
    oldmodes[i]= modes[i];
  for (i=0;i<NEQ;i++)
    ypre[i]= y[i];
  if (!first) {
	  flag = savespace(w,modes);
//    eev(t,y,ypre,tval);
//    flag= 0;
//    for (i=0;i<NEVT;i++) {
//      if (tval[i]>EVTHYST && modes[2*i]==0 ) {
//        modes[2*i]= 1;
//        if (req[i]>0 && y[req[i]-1]==1.0f)
//          oldmodes[2*i]= 1;
//        else
//          flag= 1;
//      } else if (tval[i]<EVTHYST && modes[2*i]==1 ) {
//        modes[2*i]= 0;
//        oldmodes[2*i]= 0;
//      }
//
//      if (tval[i]>-EVTHYST && modes[2*i+1]==0) {
//        modes[2*i+1]= 1;
//        oldmodes[2*i+1]= 1;
//      } else if (tval[i]<-EVTHYST && modes[2*i+1]==1 ) {
//        modes[2*i+1]= 0;
//        if (inc[i] || (req[i]>0 && y[req[i]-1]==0.0f))
//          oldmodes[2*i+1]= 0;
//        else
//          flag= 1;
//      }
//    }

    if (!flag) {
//      if (m)
//        for (i=0;i<NZC;i++)
//          m[i]= modes[i];
      return(0);
    }
  }

  failskip= 0;
  for (iter=0;;iter++) {
    if (!first || iter>0 )
      for (evt=0;evt<NEVT;evt++)
        if (modes[2*evt]!=oldmodes[2*evt] || modes[2*evt+1]!=oldmodes[2*evt+1])
        {
          rc= eex(evt+1,t,y,ypre);
          oldmodes[2*evt]= modes[2*evt];
          oldmodes[2*evt+1]= modes[2*evt+1];
        }

    rc= eex(NEVT+1,t,y,ypre);
//    if (rc==8 && !failskip) {
//      failskip= 1;
//      iter--;
//      if (termvar>=0)
//        y[termvar]= 0.0f;
//    } else
      failskip= 0;
    for (i=0;i<NEQ;i++)
      if (y[i]!=ypre[i])
        break;
    if (!failskip && i==NEQ ) {
//      if (m)
//        for (i=0;i<NZC;i++)
//          m[i]= modes[i];
      return(0);
    }

//    if (iter>=maxiter ) {
//      return(-1);
//    }

    if (!failskip && NIX1>0) {
      fp(NEQ,t,y,yp);
    }

//    for (i=0;i<NEQ;i++)
//      if (IS_UNDEF(y[i])) {
//        return(-4);
//      }

    for (i=0;i<NEQ;i++)
      ypre[i]= y[i];
//    if (failskip)
//      continue;
    savespace(w,modes);
//    eev(t,y,ypre,tval);
//    for (i=0;i<NEVT;i++) {
//      if (tval[i]>EVTHYST && modes[2*i]==0 ) {
//        modes[2*i]= 1;
//        if (req[i]>0 && y[req[i]-1]==1.0f)
//          oldmodes[2*i]= 1;
//      } else if (tval[i]<EVTHYST && modes[2*i]==1 ) {
//        modes[2*i]= 0;
//        oldmodes[2*i]= 0;
//      }
//
//      if (tval[i]>-EVTHYST && modes[2*i+1]==0) {
//        modes[2*i+1]= 1;
//        oldmodes[2*i+1]= 1;
//      } else if (tval[i]<-EVTHYST && modes[2*i+1]==1 ) {
//        modes[2*i+1]= 0;
//        if (inc[i] || (req[i]>0 && y[req[i]-1]==0.0f))
//          oldmodes[2*i+1]= 0;
//      }
//    }
  }

  //return(0);
}
#pragma optimize=size no_inline no_unroll
static void InitializeConditions()
{
  real32_T *w = (real32_T *)&ThermalModelCoreDD_DW.sfn_RWORK[0];
  int_T *iw = (int_T*)&ThermalModelCoreDD_DW.sfn_IWORK[0];
  real32_T *x = (real32_T*)&ThermalModelCoreDD_DW.sfn_DSTATE[0];
//  real32_T* i[NINP];
//  i[0] = (real32_T*)&ThermalModelCoreDD_U.Temperature;
//  i[1] = (real32_T*)&ThermalModelCoreDD_U.DriftCorrection;
//  i[2] = (real32_T*)&ThermalModelCoreDD_U.Reset;
//  i[3] = (real32_T*)&ThermalModelCoreDD_U.Current;
//  i[4] = (real32_T*)&ThermalModelCoreDD_U.Speed;
//  real32_T* p[NPAR];
//  for (int n = 0; n < NPAR; n++) {
//        p[n] = (real32_T*)&OTE2_params[n];
//  }
//  p[0] = (real32_T*)&OTE2_params[0];
//  p[1] = (real32_T*)&OTE2_params[1];
//  p[2] = (real32_T*)&OTE2_params[2];
//  p[3] = (real32_T*)&OTE2_params[3];
//  p[4] = (real32_T*)&OTE2_params[4];
//  p[5] = (real32_T*)&OTE2_params[5];
//  p[6] = (real32_T*)&OTE2_params[6];
//  p[7] = (real32_T*)&OTE2_params[7];
//  p[8] = (real32_T*)&OTE2_params[8];
//  p[9] = (real32_T*)&OTE2_params[9];
//  p[10] = (real32_T*)&OTE2_params[10];
//  p[11] = (real32_T*)&OTE2_params[11];
//  p[12] = (real32_T*)&OTE2_params[12];
//  p[13] = (real32_T*)&OTE2_params[13];
//  p[14] = (real32_T*)&OTE2_params[14];
//  p[15] = (real32_T*)&OTE2_params[15];
//  p[16] = (real32_T*)&OTE2_params[16];
//  p[17] = (real32_T*)&OTE2_params[17];
//  p[18] = (real32_T*)&OTE2_params[18];
//  p[19] = (real32_T*)&OTE2_params[19];
//  p[20] = (real32_T*)&OTE2_params[20];
//  p[21] = (real32_T*)&OTE2_params[21];
//  p[22] = (real32_T*)&OTE2_params[22];
//  p[23] = (real32_T*)&OTE2_params[23];
//  p[24] = (real32_T*)&OTE2_params[24];
//  p[25] = (real32_T*)&OTE2_params[25];
//  p[26] = (real32_T*)&OTE2_params[26];
//  p[27] = (real32_T*)&OTE2_params[27];
//  p[28] = (real32_T*)&OTE2_params[28];
//  p[29] = (real32_T*)&OTE2_params[29];
//  p[30] = (real32_T*)&OTE2_params[30];
//  p[31] = (real32_T*)&OTE2_params[31];
//  p[32] = (real32_T*)&OTE2_params[32];
//  p[33] = (real32_T*)&OTE2_params[33];
//  p[34] = (real32_T*)&OTE2_params[34];
//  p[35] = (real32_T*)&OTE2_params[35];
//  p[36] = (real32_T*)&OTE2_params[36];
//  p[37] = (real32_T*)&OTE2_params[37];
//  p[38] = (real32_T*)&OTE2_params[38];
//  p[39] = (real32_T*)&OTE2_params[39];
//  p[40] = (real32_T*)&OTE2_params[40];
//  p[41] = (real32_T*)&OTE2_params[41];
//  p[42] = (real32_T*)&OTE2_params[42];
//  p[43] = (real32_T*)&OTE2_params[43];
//  p[44] = (real32_T*)&OTE2_params[44];
//  p[45] = (real32_T*)&OTE2_params[45];
//  p[46] = (real32_T*)&OTE2_params[46];
//  p[47] = (real32_T*)&OTE2_params[47];
  int_T j;
  w[0] = 0.0f;
//  w[1] = 2.96750000000000000e+02f;
//  w[2] = 2.96750000000000000e+02f;
//  w[3] = 2.96750000000000000e+02f;
//  w[4] = 2.96750000000000000e+02f;
//  w[5] = 2.96750000000000000e+02f;
//  w[6] = 2.96750000000000000e+02f;
//  w[7] = 1.39999999999997730e+00f;
//  w[8] = 0.00000000000000000e+00f;
//  w[9] = 1.71926562631000010e+00f;
//  w[10] = -1.39999999999997730e+00f;
//  w[11] = 5.12731053250354400e-01f;
//  w[12] = 1.87144495539000010e+00f;
//  w[13] = 7.17823474550484450e-01f;
//  w[14] = 0.00000000000000000e+00f;
//  w[15] = 1.20888620478549820e+00f;
//  w[16] = -1.39999999999997730e+00f;
//  w[17] = 4.41097050602569370e-01f;
//  w[18] = 1.51750799069100030e+00f;
//  w[19] = 6.17535870843587080e-01f;
//  w[20] = 1.34282321084800000e+00f;
//  w[21] = -1.39999999999997730e+00f;
//  w[22] = -9.53482730316637990e-01f;
//  w[23] = 6.81059093083323910e-01f;
//  w[24] = 1.51750799069100030e+00f;
//  w[25] = 3.01010414518898050e+00f;
//  w[26] = 1.39999999999997730e+00f;
//  w[27] = -9.53482730316637990e-01f;
//  w[28] = 7.26564842343290990e+01f;
//  w[29] = 2.36000000000000230e+01f;
//  w[30] = 2.36000000000000230e+01f;
//  w[31] = 2.50000000000000000e+01f;
//  w[32] = 2.36000000000000230e+01f;
//  w[33] = 0.00000000000000000e+00f;
//  w[34] = 1.50000000000000000e+01f;
//  w[35] = 0.00000000000000000e+00f;
//  w[36] = 0.00000000000000000e+00f;
//  w[37] = 1.83103918650793810e+00f;
//  w[38] = 5.57000000000000030e+01f;
  w[39] = 0.00000000000000000e+00f;
  w[40] = 2.50000000000000000e+01f;
  w[41] = 0.00000000000000000e+00f;
  w[42] = 0.00000000000000000e+00f;
//  w[43] = 1.08909999999999990e+03f;
//  w[44] = 8.24979999999999930e+03f;
//  w[45] = 9.05529999999999970e+02f;
//  w[46] = 3.41800000000000000e+04f;
//  w[47] = 6.00000000000000000e+00f;
//  w[48] = 2.70000000000000000e+01f;
//  w[49] = 3.50000000000000000e+01f;
//  w[50] = 2.00000000000000000e+00f;
//  w[51] = 0.00000000000000000e+00f;
//  w[52] = 1.00000000000000000e+00f;
//  w[53] = 2.00000000000000000e+01f;
//  w[54] = 7.01999999999999960e+00f;
//  w[55] = 4.30800000000000020e-03f;
//  w[56] = 1.75000000000000000e+03f;
//  w[57] = 8.33000000000000000e+02f;
//  w[58] = 1.00000000000000000e+00f;
//  w[59] = 9.00000000000000020e-01f;
//  w[60] = 1.00000000000000000e+00f;
//  w[61] = 1.00000000000000000e+00f;
//  w[62] = 1.00000000000000000e+00f;
//  w[63] = 5.49999999999999970e-03f;
//  w[64] = 0.00000000000000000e+00f;
//  w[65] = 4.00000000000000020e-01f;
//  w[66] = 1.50000000000000000e+01f;
//  w[67] = 1.50000000000000000e+01f;
//  w[68] = 3.00000000000000000e+00f;
//  w[69] = 1.50000000000000000e+00f;
//  w[70] = 2.99999999999999990e-01f;
//  w[71] = 3.00000000000000000e+00f;
//  w[72] = 3.79999999999999980e+00f;
//  w[73] = 7.29999999999999980e-01f;
//  w[74] = 4.00000000000000000e+01f;
//  w[75] = 3.15840000000000010e-02f;
//  w[76] = -1.28809999999999990e-05f;
//  w[77] = 0.00000000000000000e+00f;
//  w[78] = 4.00000000000000000e+01f;
//  w[79] = 3.43110000000000010e-02f;
//  w[80] = -1.27890000000000010e-05f;
//  w[81] = 0.00000000000000000e+00f;
//  w[82] = 2.77230000000000010e-02f;
//  w[83] = -8.59409999999999960e-06f;
//  w[84] = 0.00000000000000000e+00f;
//  w[85] = 2.17288999999999990e-02f;
//  w[86] = 4.27152000000000010e-05f;
//  w[87] = 0.00000000000000000e+00f;
//  w[88] = 2.77230000000000010e-02f;
//  w[89] = -8.59409999999999960e-06f;
//  w[90] = 0.00000000000000000e+00f;
  iw[2*NZC+0] = 0;
  iw[2*NZC+NEVT+0] = 41;
  iw[2*NZC+1] = 0;
  iw[2*NZC+NEVT+1] = 42;
  eev(w[0],&w[1],&w[1],&w[2*NEQ+NPAR+NDFA+1]);
//  for (j=0;j<NEVT;j++) {
//    if (iw[2*NZC+NEVT+j]>0) {
//      if (w[iw[2*NZC+NEVT+j]]==0.0f) {
//        iw[2*j]= 0;
//        iw[2*j+1]= 0;
  iw[0] = 0;
  iw[1] = 0;
  iw[2] = 0;
  iw[3] = 0;
//      } else {
//        iw[2*j]= 1;
//        iw[2*j+1]= 1;
//      }
//    } else {
//      iw[2*j]= (w[2*NEQ+NPAR+NDFA+1+j]>EVTHYST ? 1 : 0);
//      iw[2*j+1]= (w[2*NEQ+NPAR+NDFA+1+j]>-EVTHYST ? 1 : 0);
//    }
//  }

//  for (j=0;j<NDIFF;j++)
//    w[NEQ+NPAR+j+1]= 0.0f;
//  for (j=0;j<NINP;j++)
//    w[j+NDIFF+NIX1-NINP+1]= i[j][0];
	w[0+NDIFF+NIX1-NINP+1]  =  ThermalModelCoreDD_U.Temperature;
	w[1+NDIFF+NIX1-NINP+1]  =  ThermalModelCoreDD_U.DriftCorrection;
	w[2+NDIFF+NIX1-NINP+1]  =  ThermalModelCoreDD_U.Reset;
	w[3+NDIFF+NIX1-NINP+1]  =  ThermalModelCoreDD_U.Current;
	w[4+NDIFF+NIX1-NINP+1]  =  ThermalModelCoreDD_U.Speed;
  for (j=0;j<NDIFF;j++)
    w[j+1]= x[j];
  for (j=0;j<NPAR;j++)
    w[j+NEQ+1]= (*ThermalModelCoreDD_Params)[j];//OTE2_params[j];//p[j][0];
  
  w[NEQ+NPAR+1]= 0.0f;
  if (NIX1>0) {
    fp(NEQ,w[0],&w[1],&w[NEQ+NPAR+1]);
  }

  /*if (*/RunEvents(w,iw,1,-1,EVTITER);//)
//    return;
  j= 0; /*cpr(w[0],&w[1]);
  if (j>0 && NIX1>0) {
    fp(NEQ,w[0],&w[1],&w[NEQ+NPAR+1]);
  }*/

  eev(w[0],&w[1],&w[1],&w[2*NEQ+NPAR+NDFA+1]);
  /*if (*/RunEvents(w,iw,1,-1,EVTITER);//)
  //    return;
  for (j=0;j<NDIFF;j++)
    x[j]= w[j+1];
}
#pragma optimize=size no_inline no_unroll
static real32_T *GetUpdatedWork(int_T getnofeed)
{
  int_T j,flag;
  real32_T *w = (real32_T *)&ThermalModelCoreDD_DW.sfn_RWORK[0];
  real32_T t = TimeStep;
  real32_T *x = (real32_T*)&ThermalModelCoreDD_DW.sfn_DSTATE[0];
  real32_T *i[NINP];
//  real32_T* p[NPAR];
//  for (int n = 0; n < NPAR; n++) {
//      p[n] = (real32_T*)&OTE2_params[n];
//  }
//  p[0] = (real32_T*)&OTE2_params[0];
//  p[1] = (real32_T*)&OTE2_params[1];
//  p[2] = (real32_T*)&OTE2_params[2];
//  p[3] = (real32_T*)&OTE2_params[3];
//  p[4] = (real32_T*)&OTE2_params[4];
//  p[5] = (real32_T*)&OTE2_params[5];
//  p[6] = (real32_T*)&OTE2_params[6];
//  p[7] = (real32_T*)&OTE2_params[7];
//  p[8] = (real32_T*)&OTE2_params[8];
//  p[9] = (real32_T*)&OTE2_params[9];
//  p[10] = (real32_T*)&OTE2_params[10];
//  p[11] = (real32_T*)&OTE2_params[11];
//  p[12] = (real32_T*)&OTE2_params[12];
//  p[13] = (real32_T*)&OTE2_params[13];
//  p[14] = (real32_T*)&OTE2_params[14];
//  p[15] = (real32_T*)&OTE2_params[15];
//  p[16] = (real32_T*)&OTE2_params[16];
//  p[17] = (real32_T*)&OTE2_params[17];
//  p[18] = (real32_T*)&OTE2_params[18];
//  p[19] = (real32_T*)&OTE2_params[19];
//  p[20] = (real32_T*)&OTE2_params[20];
//  p[21] = (real32_T*)&OTE2_params[21];
//  p[22] = (real32_T*)&OTE2_params[22];
//  p[23] = (real32_T*)&OTE2_params[23];
//  p[24] = (real32_T*)&OTE2_params[24];
//  p[25] = (real32_T*)&OTE2_params[25];
//  p[26] = (real32_T*)&OTE2_params[26];
//  p[27] = (real32_T*)&OTE2_params[27];
//  p[28] = (real32_T*)&OTE2_params[28];
//  p[29] = (real32_T*)&OTE2_params[29];
//  p[30] = (real32_T*)&OTE2_params[30];
//  p[31] = (real32_T*)&OTE2_params[31];
//  p[32] = (real32_T*)&OTE2_params[32];
//  p[33] = (real32_T*)&OTE2_params[33];
//  p[34] = (real32_T*)&OTE2_params[34];
//  p[35] = (real32_T*)&OTE2_params[35];
//  p[36] = (real32_T*)&OTE2_params[36];
//  p[37] = (real32_T*)&OTE2_params[37];
//  p[38] = (real32_T*)&OTE2_params[38];
//  p[39] = (real32_T*)&OTE2_params[39];
//  p[40] = (real32_T*)&OTE2_params[40];
//  p[41] = (real32_T*)&OTE2_params[41];
//  p[42] = (real32_T*)&OTE2_params[42];
//  p[43] = (real32_T*)&OTE2_params[43];
//  p[44] = (real32_T*)&OTE2_params[44];
//  p[45] = (real32_T*)&OTE2_params[45];
//  p[46] = (real32_T*)&OTE2_params[46];
//  p[47] = (real32_T*)&OTE2_params[47];
  flag= 0;
  if (fabsf(w[0]-t)>1e-6f*TIMESTEP )
    flag= 1;
  w[0]= t;
  for (j=0;j<NDIFF;j++)
    if (w[j+1]!=x[j] ) {
      flag= 1;
      w[j+1]= x[j];
    }

  i[0] = (real32_T*)&ThermalModelCoreDD_U.Temperature;
  i[1] = (real32_T*)&ThermalModelCoreDD_U.DriftCorrection;
  i[2] = (real32_T*)&ThermalModelCoreDD_U.Reset;
  i[3] = (real32_T*)&ThermalModelCoreDD_U.Current;
  i[4] = (real32_T*)&ThermalModelCoreDD_U.Speed;
  for (j=0;j<NINP;j++)
    if (w[j+NDIFF+NIX1-NINP+1]!=i[j][0] ) {
      flag= 1;
      w[j+NDIFF+NIX1-NINP+1]= i[j][0];
    }

  for (j=0;j<NPAR;j++)
    if (w[j+NEQ+1]!=(*ThermalModelCoreDD_Params)[j]) {
      flag= 1;
      w[j+NEQ+1]= (*ThermalModelCoreDD_Params)[j];
    }

  if (flag) {
    fp(NEQ,w[0],&w[1],&w[NEQ+NPAR+1]);
  }

  return(w);
}
#pragma optimize=size no_inline no_unroll
static void EulerStep(real32_T *w)
{
  int_T i;
  w[0]+=TIMESTEP;
  for (i=1;i<=NDIFF;i++)
    w[i]+=TIMESTEP*w[NEQ+NPAR+i];
}
/* Model step function */
#pragma optimize=size no_inline no_unroll
void ThermalModelCoreDD_step(void)
{
  /* S-Function (cOTE2_DDBPM_Single): '<S1>/sfn'
   *
   * Block description for '<S1>/sfn':
   *   Generated by MapleSim
   */
  real32_T *w/*, *y = (real32_T*)&ThermalModelCoreDD_B.sfn[0]*/;
  int_T *iw = (int_T*)&ThermalModelCoreDD_DW.sfn_IWORK[0];
  int_T i;
  real32_T *x;
  if (!iw[INIDREF]) {
    InitializeConditions();
    iw[INIDREF]= 1;
  }

  w= GetUpdatedWork(0);

  {
      /*if (*/RunEvents(w,iw,0,-1,EVTITER);//)
	  //    return;
    x= (real32_T*)&ThermalModelCoreDD_DW.sfn_DSTATE[0];
    for (i=0;i<NDIFF;i++)
      x[i]= w[i+1];
  }

//  otp(w[0],&w[1],&w[NEQ+NPAR+1]);
//  y[ 0]= w[29];
//  y[ 1]= w[32];
//  y[ 2]= w[30];
//  y[ 3]= w[31];
//  y[ 4]= w[33];

  /* Outport: '<Root>/MagnetTemperature' */
//  ThermalModelCoreDD_Y.MagnetTemperature = w[2] - 273.15;//ThermalModelCoreDD_B.sfn[0];
//
//  /* Outport: '<Root>/StatorTempareture' */
//  ThermalModelCoreDD_Y.StatorTempareture = w[5] - 273.15;//ThermalModelCoreDD_B.sfn[1];

  /* Outport: '<Root>/WindingTemperature' */
  ThermalModelCoreDD_Y.WindingTemperature = w[6] - 273.15; //ThermalModelCoreDD_B.sfn[2];

//  /* Outport: '<Root>/AmbientTemperature' */
//  ThermalModelCoreDD_Y.AmbientTemperature = w[40];//ThermalModelCoreDD_B.sfn[3];
//
//  /* Outport: '<Root>/error' */
//  ThermalModelCoreDD_Y.error = w[39];//ThermalModelCoreDD_B.sfn[4];

  /* Update for S-Function (cOTE2_DDBPM_Single): '<S1>/sfn' */
  w= GetUpdatedWork(1);
  EulerStep(w);
  fp(NEQ,w[0],&w[1],&w[NEQ+NPAR+1]);
  for (i=0;i<NDIFF;i++)
    x[i]= w[i+1];
  TimeStep = TimeStep + TIMESTEP;
}

/* Model initialize function */
#pragma optimize=size no_inline no_unroll
void ThermalModelCoreDD_initialize(void)
{
  /* Registration code */

  /* block I/O */
  (void) memset(((void *) &ThermalModelCoreDD_B), 0,
                sizeof(B_ThermalModelCoreDD_T));

  /* states (dwork) */
  (void) memset((void *)&ThermalModelCoreDD_DW, 0,
                sizeof(DW_ThermalModelCoreDD_T));

  /* external inputs */
  (void)memset((void *)&ThermalModelCoreDD_U, 0, sizeof(ExtU_ThermalModelCoreDD_T));

  /* external outputs */
  (void) memset((void *)&ThermalModelCoreDD_Y, 0,
                sizeof(ExtY_ThermalModelCoreDD_T));

  /* InitializeConditions for S-Function (cOTE2_DDBPM_Single): '<S1>/sfn' */
  real32_T* ic[NDIFF];
  for (int n = 0; n < NDIFF; n++) {
        ic[n] = (real32_T*)&ThermalModelCoreDD_ConstP.MapleSimICs_Value[n];
  }
//  ic[0] = (real32_T*)ThermalModelCoreDD_ConstP.MapleSimICs_Value;
//  ic[1] = (real32_T*)&ThermalModelCoreDD_ConstP.MapleSimICs_Value[1];
//  ic[2] = (real32_T*)&ThermalModelCoreDD_ConstP.MapleSimICs_Value[2];
//  ic[3] = (real32_T*)&ThermalModelCoreDD_ConstP.MapleSimICs_Value[3];
//  ic[4] = (real32_T*)&ThermalModelCoreDD_ConstP.MapleSimICs_Value[4];
//  ic[5] = (real32_T*)&ThermalModelCoreDD_ConstP.MapleSimICs_Value[5];
  real32_T *x = (real32_T*)&ThermalModelCoreDD_DW.sfn_DSTATE[0];
  int_T * iw = (int_T*)&ThermalModelCoreDD_DW.sfn_IWORK[0];
  int_T j;
  iw[INIDREF]= 0;
  for (j=0;j<NDIFF;j++)
    x[j]= ic[j][0];
}

/* Model terminate function */
#pragma optimize=size no_inline no_unroll
void ThermalModelCoreDD_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
