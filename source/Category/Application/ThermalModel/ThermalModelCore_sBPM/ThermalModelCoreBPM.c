/*
 * File: ThermalModelCore.c
 *
 * Code generated for Simulink model 'ThermalModelCore'.
 *
 * Model version                  : 1.12
 * Simulink Coder version         : 8.11 (R2016b) 25-Aug-2016
 * C/C++ source code generated on : Thu Nov 30 10:43:35 2017
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "ThermalModelCoreBPM.h"

#include "ThermalModelCore_private.h"
#include <math.h>
#define TIMESTEP                       2.500000e-02f
#define EVTITER                        10
#define EVTHYST                        1.000000e-06f
#define NDIFF                          7
#define NDFA                           7
#define NEQ                            54
#define NPAR                           51
#define NDPAR                          2
#define NINP                           5
#define NDISC                          4
#define NIX1                           43
#define NOUT                           8
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
#define pow                            fastpow
#define exp                            expf
#define log                            logf
#define log10                          log10f
#define sqrt                           sqrtf
#define ceil                           ceilf
#define floor                          floorf

//static real32_T dsn_zero= 0.0f;
//static unsigned char dsn_undefC[4] = { 0, 0, 0xC0, 0x7F };

//static real32_T *dsn_undef = (real32_T *)&dsn_undefC;
//static unsigned char dsn_posinfC[4] = { 0, 0, 0x80, 0x7F };

//static real32_T *dsn_posinf = (real32_T *)&dsn_posinfC;
//static unsigned char dsn_neginfC[4] = { 0, 0, 0x80, 0xFF };
//
//static real32_T *dsn_neginf = (real32_T *)&dsn_neginfC;

#define trunc(v)                       ( (v>0.0f) ? floor(v) : ceil(v) )
//#define IS_UNDEF(a)                    (a-a!=0.f || (a!=0.f && a-2.f*a==0.f))

static real32_T TimeStep = 0.0f;

OTE2_PARAMS_BPM_TYPE* ThermalModelCoreBPM_Params;

/* Block signals (auto storage) */
B_ThermalModelCoreBPM_T ThermalModelCoreBPM_B;

/* Block states (auto storage) */
DW_ThermalModelCoreBPM_T ThermalModelCoreBPM_DW;

/* External inputs (root inport signals with auto storage) */
ExtU_ThermalModelCoreBPM_T ThermalModelCoreBPM_U;

/* External outputs (root outports fed by signals with auto storage) */
ExtY_ThermalModelCoreBPM_T ThermalModelCoreBPM_Y;


float fastpow (float x,float p)
{
  union {
    float f;
    unsigned int i;
  } vx = { x };

  union {
    unsigned int i;
    float f;
  } mx = { (vx.i & 0x007FFFFF) | (0x7e << 23) };

  float y = vx.i;
  y *= 1.0 / (1 << 23);
  y= y - 124.22544637f - 1.498030302f * mx.f - 1.72587999f / (0.3520887068f +
    mx.f);

  //exp2_part
  union {
    float f;
    unsigned int i;
  } vp = { p*y };

  int sign = (vp.i >> 31);
  int w = p*y;
  float z = p*y - w + sign;
  union {
    unsigned int i;
    float f;
  } v = { (1 << 23) * (p*y + 121.2740838f + 27.7280233f / (4.84252568f - z) -
                       1.49012907f * z) };

  return v.f;
}
#pragma optimize=size no_inline no_unroll
static void fp(int_T N, real32_T T, real32_T *Y, real32_T *YP)
{
  real32_T Z[4];
  if (Y[49]>=0.f )
    Y[32] = Y[49];
  else
    Y[32] = -Y[49];
  Z[0] = (Y[6]-Y[0])*Y[69];
  Z[1] = (Y[4]-Y[0])*Y[71];
  YP[0] = (Z[1]+Z[0])/Y[59];
  Y[17] = Y[97]+(Y[32]*Y[99]+Y[98])*Y[32];
  Y[16] = Y[91]+(Y[32]*Y[93]+Y[92])*Y[32];
  Y[18] = 1e-06f*Y[58]*Y[17]*(Y[3]-Y[2]);
  Z[2] = (Y[4]-Y[5])*Y[74];
  Y[34] = Z[2]-Y[54]*Y[16]*(Y[5]-Y[2]);
  Y[15] = Y[94]+(Y[32]*Y[96]+Y[95])*Y[32];
  Z[3] = (Y[4]-Y[2])*Y[70];
  YP[2] = (Z[2]+Z[3]+Y[18]-Y[34])/Y[62];
  Y[19] = (Y[3]-Y[4])*Y[15]*Y[57]-Z[2]-Z[1]-Z[3];
  Z[1] = Y[51];
  Z[2] = 0.5f;
  Y[27] = Z[2]*(Z[1]+Y[4])-136.575f;
  Z[2] = Y[27]+273.15f;
  Z[2] = 1.0f/Z[2];
  Y[24] = Z[2];
  Z[2] = Z[1]-Y[4]+273.15f;
  Y[28] = fabsf(Z[2])+0.01f;
  Y[26] = Y[24]*Y[28]*Y[80];
//  if (Y[26]<0.f ) {
//    YP[0] = (*dsn_undef);
//    return;
//  }

  Y[25] = 0.387f*pow(Y[26],0.1666666666666666667f);
  Z[2] = -Z[2];
  Y[20] = Z[2];
  Z[3] = 0.6f+0.8292569616863719f*Y[25];
  Y[29] = Z[3]*Z[3]*Y[89]*Y[56];
  Y[21] = Y[29]*Y[20];
  Y[22] = Z[2];
  Y[30] = Y[32]*(Y[32]*Y[104]+Y[103])*Y[56];
  Y[23] = Y[30]*Y[22];
  Z[1] = Z[1]+273.15f;
  Z[2] = Y[4]*Y[4];
  Z[1] = Z[1]*Z[1];
  Y[31] = Y[21]+Y[23]+5.670367000000000514e-08f*Y[85]*Y[56]*(Z[2]*Z[2]-Z[1]*Z[1]);
  Z[1] = Y[6]+Y[3];
  Z[1] = 1.0f/Z[1];
  Y[8] = 2.f*Z[1];
  Z[1] = Y[6]-Y[3];
  Y[11] = fabsf(Z[1]+0.01f);
  Y[13] = Y[100]+(Y[32]*Y[102]+Y[101])*Y[32];
  YP[4] = (Y[19]-Y[31])/Y[63];
  Y[10] = Y[81]*Y[8]*Y[11];
//  if (Y[10]<0.f ) {
//    YP[0] = (*dsn_undef);
//    return;
//  }

  Y[9] = 0.324074819988109413f*pow(Y[10],0.1667f)+0.825000000000000002f;
  Y[12] = Y[9]*Y[9]*Y[90];
  Z[2] = Y[13]*Y[13];
  Z[3] = Y[12]*Y[12];
  Y[7] = Z[2]*Z[2]+Z[3]*Z[3];
//  if (Y[7]<0.f ) {
//    YP[0] = (*dsn_undef);
//    return;
//  }

  Y[14] = Y[55]*pow(Y[7],0.25f);
  Z[2] = (Y[6]-Y[1])*Y[73];
  Y[35] = -(Z[1]*Y[14]+Z[0]+Z[2]);
  Z[0] = Y[48];
  Z[0] = Z[0]*Z[0]*Y[82];
  Y[36] = 3.f*Z[0]*(1.f+(Y[6]-Y[83])*Y[84])-819.45f*Z[0]*Y[84];
  Z[0] = (Y[5]-Y[1])*Y[72];
  YP[1] = (Z[2]+Z[0])/Y[61];
  YP[6] = (Y[35]+Y[36])/Y[65];
  Y[33] = (Y[76]+(Y[77]+(Y[32]*Y[79]+Y[78])*Y[32])*Y[32])*Y[32];
  YP[3] = (1.f*(Y[70]*Y[2]+Y[74]*Y[5]-Y[69]*Y[6]-(-(Y[71]+Y[69]))*Y[0]-(Y[71]+Y
             [70]+Y[74])*Y[4]-Y[18]-Y[19]-Y[35]-Z[2]))/Y[60];
  YP[5] = (Y[33]+Y[34]-Z[0])/Y[64];

}
//#pragma optimize=size no_inline no_unroll
//static void otp(real32_T T, real32_T *Y, real32_T *YP)
//{
//  Y[37] = Y[1]-273.15f;
//  Y[38] = Y[3]-273.15f;
//  Y[39] = Y[2]-273.15f;
//  Y[40] = Y[4]-273.15f;
//  Y[41] = Y[5]-273.15f;
//  Y[42] = Y[6]-273.15f;
//  Y[43] = Y[51];
//  Y[44] = Y[50];
//}
#pragma optimize=size no_inline no_unroll
static void eev(real32_T T, real32_T *Y, real32_T *Ypre, real32_T *EA)
{
  EA[0] = Y[46]-0.1f;
  EA[1] = Y[47]-0.1f;
}
#pragma optimize=size no_inline no_unroll
static int_T eex(int_T N, real32_T T, real32_T *Y, real32_T *Ypre)
{
  real32_T Z[3]; //, td1;
  if (N==1 )
    Y[52] = 1.f-Y[52];
  else if (N==2 )
    Y[53] = 1.f-Y[53];
  else if (N==3 ) {
//    Z[0] = Y[45]; //input temperature
    if (Y[45]<Y[75] )
      Z[2] = Y[45]+273.15f; //td1 = Y[45];
    else
      Z[2] = Y[75]+273.15f; //td1 = Y[75];
 //   Z[1] = td1+273.15f;
 //   Z[2] = Z[1];
    if ((Y[53]==1.f) && (Y[53]!=Ypre[53]) ) { //reset flag
      Y[50] = 0.f;
//      if (Z[0]<Y[75] )
//        Y[51] = Z[0];
//      else
//        Y[51] = Y[75];
      Y[51] = Z[2]-273.15f;

      Y[6] = Y[45]+273.15f;			// winding temperature
      if (Z[2]<Y[45]-Y[68]+273.15f )
        Z[0] /*Y[5]*/ = Y[45]-Y[68]+273.15f; //stator lamination temperature
      else
        Z[0] /*Y[5]*/ = Z[2];
      Y[5] = Z[0];
      Y[1] = Z[0];
      Y[0] = Z[0];
//      if (Z[2]<Y[45]-Y[68]+273.15f )
//        Y[1] = Y[45]-Y[68]+273.15f; //stator insulation temperature
//      else
//        Y[1] = Z[2];
//      if (Z[2]<Y[45]-Y[68]+273.15f )
//        Y[0] = Y[45]-Y[68]+273.15f; //bmc insulation temperature
//      else
//        Y[0] = Z[2];
      if (Z[2]<Y[45]-Y[66]+273.15f )
        Z[0]/*Y[2]*/ = Y[45]-Y[66]+273.15f;//rotor temperature
      else
        Z[0]/*Y[2]*/ = Z[2];
      Y[2] = Z[0];
      Y[3] = Z[0];
//      if (Z[2]<Y[45]-Y[66]+273.15f )
//        Y[3] = Y[45]-Y[66]+273.15f; //internal ambient temperature
//      else
//        Y[3] = Z[2];
      if (Z[2]<Y[45]-Y[67]+273.15f )
        Y[4] = Y[45]-Y[67]+273.15f; // shell temperature
      else
        Y[4] = Z[2];
    } else if ((Y[52]==1.f) && (Y[52]!=Ypre[52]) ) { //drift flag
      Y[50] = Y[6]-Y[45]-273.15f;
      Y[6] = Y[45]+273.15f;
      Y[5] = Y[5]-Y[50]*Y[88];
      Y[1] = Y[1]-Y[50]*Y[88];
      Y[0] = Y[0]-Y[50]*Y[88];
      Y[2] = Y[2]-Y[50]*Y[86];
      Y[3] = Y[3]-Y[50]*Y[86];
      Y[4] = Y[4]-Y[50]*Y[87];
      Y[51] = Ypre[51]-Y[50];
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
//  if (Y[46]>=0.1f )//DriftCorrection
//    v = 1.f;
//  else
//    v = 0.f;
//  if (Y[52]!=v ) {
//    Y[52] = v;
//    k = 1;
//  }
//
//  if (Y[47]>=0.1f )//Reset
//    v = 1.f;
//  else
//    v = 0.f;
//  if (Y[53]!=v ) {
//    Y[53] = v;
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
static int_T RunEvents(real32_T *w, int_T *modes, int_T first, int_T termvar, int_T maxiter)
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
//    eev(t,y,ypre,tval);			//tval either -0.1 or 0.9
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
  real32_T *w = (real32_T *)&ThermalModelCoreBPM_DW.sfn_RWORK[0];
  int_T *iw = (int_T*)&ThermalModelCoreBPM_DW.sfn_IWORK[0];
  real32_T *x = (real32_T*)&ThermalModelCoreBPM_DW.sfn_DSTATE[0];
//  real32_T* i[NINP];
//  i[0] = (real32_T*)&ThermalModelCoreBPM_U.Temperature;
//  i[1] = (real32_T*)&ThermalModelCoreBPM_U.DriftCorrection;
//  i[2] = (real32_T*)&ThermalModelCoreBPM_U.Reset;
//  i[3] = (real32_T*)&ThermalModelCoreBPM_U.Current;
//  i[4] = (real32_T*)&ThermalModelCoreBPM_U.Speed;
//  real32_T* p[NPAR];
//  for (int n = 0; n < NPAR; n++) {
//      p[n] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[n];
//    }
//  p[0] = (real32_T*)ThermalModelCoreBPM_ConstP.MapleSimParameters_Value;
//  p[1] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[1];
//  p[2] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[2];
//  p[3] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[3];
//  p[4] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[4];
//  p[5] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[5];
//  p[6] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[6];
//  p[7] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[7];
//  p[8] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[8];
//  p[9] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[9];
//  p[10] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[10];
//  p[11] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[11];
//  p[12] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[12];
//  p[13] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[13];
//  p[14] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[14];
//  p[15] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[15];
//  p[16] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[16];
//  p[17] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[17];
//  p[18] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[18];
//  p[19] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[19];
//  p[20] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[20];
//  p[21] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[21];
//  p[22] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[22];
//  p[23] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[23];
//  p[24] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[24];
//  p[25] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[25];
//  p[26] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[26];
//  p[27] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[27];
//  p[28] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[28];
//  p[29] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[29];
//  p[30] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[30];
//  p[31] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[31];
//  p[32] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[32];
//  p[33] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[33];
//  p[34] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[34];
//  p[35] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[35];
//  p[36] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[36];
//  p[37] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[37];
//  p[38] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[38];
//  p[39] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[39];
//  p[40] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[40];
//  p[41] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[41];
//  p[42] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[42];
//  p[43] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[43];
//  p[44] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[44];
//  p[45] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[45];
//  p[46] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[46];
//  p[47] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[47];
//  p[48] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[48];
//  p[49] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[49];
//  p[50] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[50];
  int_T j;
  w[0] = 0.0f;
//  w[1] = 2.98149999999999980e+02f;
//  w[2] = 2.98149999999999980e+02f;
//  w[3] = 2.98149999999999980e+02f;
//  w[4] = 2.98149999999999980e+02f;
//  w[5] = 2.98149999999999980e+02f;
//  w[6] = 2.98149999999999980e+02f;
//  w[7] = 2.98149999999999980e+02f;
//  w[8] = 1.77190390364524430e+02f;
//  w[9] = 3.35401643468053030e-03f;
//  w[10] = 1.16945363247448840e+00f;
//  w[11] = 1.44172061043099120e+00f;
//  w[12] = 1.00000000000000000e-02f;
//  w[13] = 2.73524359701555180e+00f;
//  w[14] = 3.31810865000000010e+00f;
//  w[15] = 3.64846379735259480e-16f;
//  w[16] = 3.63434577499999990e+00f;
//  w[17] = 2.24229160000000020e+00f;
//  w[18] = 4.62228520000000030e+00f;
//  w[19] = -0.00000000000000000e+00f;
//  w[20] = 0.00000000000000000e+00f;
//  w[21] = -0.00000000000000000e+00f;
//  w[22] = -0.00000000000000000e+00f;
//  w[23] = -0.00000000000000000e+00f;
//  w[24] = -0.00000000000000000e+00f;
//  w[25] = 3.35401643468053030e-03f;
//  w[26] = 1.16099286887889000e+00f;
//  w[27] = 7.28973134328358240e+02f;
//  w[28] = 2.50000000000000000e+01f;
//  w[29] = 1.00000000000000000e-02f;
//  w[30] = 4.10246751041957600e-02f;
//  w[31] = 3.34569899999999980e-04f;
//  w[32] = -0.00000000000000000e+00f;
//  w[33] = 5.00000000000000000e+02f;
//  w[34] = 2.28802559749999990e+00f;
//  w[35] = 0.00000000000000000e+00f;
//  w[36] = 0.00000000000000000e+00f;
//  w[37] = 8.60999999999999770e+00f;
//  w[38] = 2.50000000000000000e+01f;
//  w[39] = 2.50000000000000000e+01f;
//  w[40] = 2.50000000000000000e+01f;
//  w[41] = 2.50000000000000000e+01f;
//  w[42] = 2.50000000000000000e+01f;
//  w[43] = 2.50000000000000000e+01f;
//  w[44] = 2.50000000000000000e+01f;
//  w[45] = 0.00000000000000000e+00f;
//  w[46] = 3.50000000000000000e+01f;
//  w[47] = 0.00000000000000000e+00f;
//  w[48] = 0.00000000000000000e+00f;
//  w[49] = 1.00000000000000000e+00f;
//  w[50] = 5.00000000000000000e+02f;
//  w[51] = 0.00000000000000000e+00f;
  w[52] = 2.50000000000000000e+01f;
//  w[53] = 0.00000000000000000e+00f;
//  w[54] = 0.00000000000000000e+00f;
//  w[55] = 5.40000000000000030e-03f;
//  w[56] = 9.99999999999999980e-17f;
//  w[57] = 6.69139799999999980e-02f;
//  w[58] = 1.00000000000000000e-02f;
//  w[59] = 1.54083180000000010e-02f;
//  w[60] = 8.12000000000000000e+02f;
//  w[61] = 2.50000000000000000e+01f;
//  w[62] = 3.50000000000000000e+01f;
//  w[63] = 3.54060000000000000e+02f;
//  w[64] = 1.01500000000000000e+03f;
//  w[65] = 3.28500000000000000e+02f;
//  w[66] = 9.99599999999999940e+01f;
//  w[67] = 1.50000000000000000e+01f;
//  w[68] = 1.00000000000000000e+01f;
//  w[69] = 5.00000000000000000e+00f;
//  w[70] = 8.94079999999999940e+00f;
//  w[71] = 2.00000000000000010e-01f;
//  w[72] = 8.94079999999999940e+00f;
//  w[73] = 2.93299999999999980e+00f;
//  w[74] = 3.50349999999999980e+00f;
//  w[75] = 2.88799999999999990e+01f;
//  w[76] = 3.50000000000000000e+01f;
//  w[77] = 4.08263000000000030e-03f;
//  w[78] = 1.06600000000000000e-06f;
//  w[79] = -1.61367000000000010e-10f;
//  w[80] = 6.10356000000000010e-15f;
//  w[81] = 2.17343340000000000e+07f;
//  w[82] = 4.29849000000000010e+04f;
//  w[83] = 2.87000000000000010e+00f;
//  w[84] = 2.50000000000000000e+01f;
//  w[85] = 4.30800000000000020e-03f;
//  w[86] = 8.00000000000000040e-01f;
//  w[87] = 1.00000000000000000e+00f;
//  w[88] = 1.00000000000000000e+00f;
//  w[89] = 1.00000000000000000e+00f;
//  w[90] = 2.51039999999999990e-01f;
//  w[91] = 2.00000000000000000e+00f;
//  w[92] = 2.00000000000000000e+00f;
//  w[93] = 4.76354999999999980e-04f;
//  w[94] = 1.64564000000000000e-08f;
//  w[95] = 2.00000000000000000e+00f;
//  w[96] = 3.30519000000000000e-03f;
//  w[97] = -7.29968999999999960e-08f;
//  w[98] = 2.00000000000000000e+00f;
//  w[99] = 5.27068999999999980e-03f;
//  w[100] = -5.22392000000000000e-08f;
//  w[101] = 2.00000000000000000e+00f;
//  w[102] = 2.66796000000000000e-03f;
//  w[103] = -6.34853999999999980e-08f;
//  w[104] = 0.00000000000000000e+00f;
//  w[105] = 2.00000000000000000e-08f;
  iw[2*NZC+0] = 0;			//iw[8]
  iw[2*NZC+NEVT+0] = 53;	//iw[10]
  iw[2*NZC+1] = 0;			//iw[9]
  iw[2*NZC+NEVT+1] = 54;	//iw[11]
  eev(w[0],&w[1],&w[1],&w[2*NEQ+NPAR+NDFA+1]);
//  for (j=0;j<NEVT;j++) {
//    if (iw[2*NZC+NEVT+j]>0) {
//      if (w[iw[2*NZC+NEVT+j]]==0.0f) {

//        iw[2*j]= 0;			//iw[0],iw[2]
//        iw[2*j+1]= 0;		//iw[1],iw[3]
        iw[0] = 0;
        iw[1] = 0;
        iw[2] = 0;
        iw[3] = 0;

//      } else {
//        iw[2*j]= 1;
//        iw[2*j+1]= 1;
//      }
//    } else {
//      iw[2*j]= (w[2*NEQ+NPAR+NDFA+1+j]>EVTHYST ? 1 : 0);	//w[167],w[168] - tval
//      iw[2*j+1]= (w[2*NEQ+NPAR+NDFA+1+j]>-EVTHYST ? 1 : 0);	//w[167],w[168] - tval
//    }
//  }

//  for (j=0;j<NDIFF;j++)
//    w[NEQ+NPAR+j+1]= 0.0f;
// for (j=0;j<NINP;j++)
//    w[j+NDIFF+NIX1-NINP+1]= i[j][0];
	w[0+NDIFF+NIX1-NINP+1]  =  ThermalModelCoreBPM_U.Temperature;
	w[1+NDIFF+NIX1-NINP+1]  =  ThermalModelCoreBPM_U.DriftCorrection;
	w[2+NDIFF+NIX1-NINP+1]  =  ThermalModelCoreBPM_U.Reset;
	w[3+NDIFF+NIX1-NINP+1]  =  ThermalModelCoreBPM_U.Current;
	w[4+NDIFF+NIX1-NINP+1]  =  ThermalModelCoreBPM_U.Speed;

  for (j=0;j<NDIFF;j++)
    w[j+1]= x[j];
  for (j=0;j<NPAR;j++)
    w[j+NEQ+1]= (*ThermalModelCoreBPM_Params)[j];//ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[j];//p[j][0];
  w[NEQ+NPAR+1]= 0.0f;
  if (NIX1>0) {
    fp(NEQ,w[0],&w[1],&w[NEQ+NPAR+1]);
  }

  /*if (*/RunEvents(w,iw,1,-1,EVTITER)/*)*/
  /*  return*/;
  j= 0; //cpr(w[0],&w[1]);
//  if (j>0 && NIX1>0) {
//    fp(NEQ,w[0],&w[1],&w[NEQ+NPAR+1]);
//  }

  eev(w[0],&w[1],&w[1],&w[2*NEQ+NPAR+NDFA+1]);
  /*if (*/RunEvents(w,iw,0,-1,EVTITER)/*)*/
    /*  return*/;
  for (j=0;j<NDIFF;j++)
    x[j]= w[j+1];
}
#pragma optimize=size no_inline no_unroll
static real32_T *GetUpdatedWork(int_T getnofeed)
{
  int_T j,flag;
  real32_T *w = (real32_T *)&ThermalModelCoreBPM_DW.sfn_RWORK[0];
  real32_T t = TimeStep;
  real32_T *x = (real32_T*)&ThermalModelCoreBPM_DW.sfn_DSTATE[0];
  real32_T *i[NINP];
//  real32_T* p[NPAR];
//  for (int n = 0; n < NPAR; n++) {
//        p[n] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[n];
//      }
//  p[0] = (real32_T*)ThermalModelCoreBPM_ConstP.MapleSimParameters_Value;
//  p[1] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[1];
//  p[2] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[2];
//  p[3] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[3];
//  p[4] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[4];
//  p[5] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[5];
//  p[6] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[6];
//  p[7] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[7];
//  p[8] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[8];
//  p[9] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[9];
//  p[10] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[10];
//  p[11] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[11];
//  p[12] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[12];
//  p[13] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[13];
//  p[14] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[14];
//  p[15] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[15];
//  p[16] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[16];
//  p[17] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[17];
//  p[18] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[18];
//  p[19] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[19];
//  p[20] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[20];
//  p[21] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[21];
//  p[22] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[22];
//  p[23] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[23];
//  p[24] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[24];
//  p[25] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[25];
//  p[26] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[26];
//  p[27] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[27];
//  p[28] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[28];
//  p[29] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[29];
//  p[30] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[30];
//  p[31] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[31];
//  p[32] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[32];
//  p[33] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[33];
//  p[34] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[34];
//  p[35] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[35];
//  p[36] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[36];
//  p[37] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[37];
//  p[38] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[38];
//  p[39] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[39];
//  p[40] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[40];
//  p[41] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[41];
//  p[42] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[42];
//  p[43] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[43];
//  p[44] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[44];
//  p[45] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[45];
//  p[46] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[46];
//  p[47] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[47];
//  p[48] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[48];
//  p[49] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[49];
//  p[50] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[50];
  flag= 0;
  if (fabsf(w[0]-t)>1e-6f*TIMESTEP )
    flag= 1;
  w[0]= t;
  for (j=0;j<NDIFF;j++)
    if (w[j+1]!=x[j] ) {
      flag= 1;
      w[j+1]= x[j];
    }

  i[0] = (real32_T*)&ThermalModelCoreBPM_U.Temperature;
  i[1] = (real32_T*)&ThermalModelCoreBPM_U.DriftCorrection;
  i[2] = (real32_T*)&ThermalModelCoreBPM_U.Reset;
  i[3] = (real32_T*)&ThermalModelCoreBPM_U.Current;
  i[4] = (real32_T*)&ThermalModelCoreBPM_U.Speed;
  for (j=0;j<NINP;j++)
    if (w[j+NDIFF+NIX1-NINP+1]!=i[j][0] ) {
      flag= 1;
      w[j+NDIFF+NIX1-NINP+1]= i[j][0];
    }

  for (j=0;j<NPAR;j++)
    if (w[j+NEQ+1]!= (*ThermalModelCoreBPM_Params)[j]/*ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[j]*/ ) {
      flag= 1;
      w[j+NEQ+1]= (*ThermalModelCoreBPM_Params)[j];//ThermalModelCoreBPM_ConstP.MapleSimParameters_Value[j];
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
void ThermalModelCoreBPM_step(void)
{
  /* S-Function (csBPM_final_code): '<S1>/sfn'
   *
   * Block description for '<S1>/sfn':
   *   Generated by MapleSim
   */
  real32_T *w;//, *y = (real32_T*)&ThermalModelCoreBPM_B.sfn[0];
  int_T *iw = (int_T*)&ThermalModelCoreBPM_DW.sfn_IWORK[0];
  int_T i;
  real32_T *x;
  if (!iw[INIDREF]) {
    InitializeConditions();
    iw[INIDREF]= 1;			//iw[12]
  }

  w= GetUpdatedWork(0);

  {
	  /*if (*/RunEvents(w,iw,0,-1,EVTITER)/*)*/
	    /*  return*/;
    x= (real32_T*)&ThermalModelCoreBPM_DW.sfn_DSTATE[0];
    for (i=0;i<NDIFF;i++)
      x[i]= w[i+1];
  }

//  otp(w[0],&w[1],&w[NEQ+NPAR+1]);
//  y[ 0]= w[38];
//  y[ 1]= w[39];
//  y[ 2]= w[40];
//  y[ 3]= w[41];
//  y[ 4]= w[42];
//  y[ 5]= w[43];
//  y[ 6]= w[44];
//  y[ 7]= w[45];

  /* Outport: '<Root>/MagnetTemperature' */
//  ThermalModelCoreBPM_Y.MagnetTemperature = w[3]-273.15f; //w[40];//ThermalModelCoreBPM_B.sfn[2];

  /* Outport: '<Root>/StatorTempareture' */
//  ThermalModelCoreBPM_Y.StatorTempareture = w[6]-273.15f; //w[42];//ThermalModelCoreBPM_B.sfn[4];

  /* Outport: '<Root>/WindingTemperature' */
  ThermalModelCoreBPM_Y.WindingTemperature = w[7]-273.15f; //w[43];//ThermalModelCoreBPM_B.sfn[5];

  /* Outport: '<Root>/AmbientTemperature' */
//  ThermalModelCoreBPM_Y.AmbientTemperature = w[52]; //w[44];//ThermalModelCoreBPM_B.sfn[6];

  /* Outport: '<Root>/error' */
 // ThermalModelCoreBPM_Y.error = w[51]; //w[45];//ThermalModelCoreBPM_B.sfn[7];

  /* Outport: '<Root>/InsulationTemperature' */
 // ThermalModelCoreBPM_Y.InsulationTemperature = w[2]-273.15f; //w[38];//ThermalModelCoreBPM_B.sfn[0];

  /* Outport: '<Root>/InternalAmbTemperature' */
//  ThermalModelCoreBPM_Y.InternalAmbTemperature = w[4]-273.15f; //w[39];//ThermalModelCoreBPM_B.sfn[1];

  /* Outport: '<Root>/ShellTemperature' */
//  ThermalModelCoreBPM_Y.ShellTemperature = w[5]-273.15f; //w[41];//ThermalModelCoreBPM_B.sfn[3];

  /* Update for S-Function (csBPM_final_code): '<S1>/sfn' */
  w= GetUpdatedWork(1);
  EulerStep(w);
  fp(NEQ,w[0],&w[1],&w[NEQ+NPAR+1]);
  for (i=0;i<NDIFF;i++)
    x[i]= w[i+1];
  TimeStep = TimeStep + TIMESTEP;
}

/* Model initialize function */
#pragma optimize=size no_inline no_unroll
void ThermalModelCoreBPM_initialize(void)
{
  /* Registration code */

  /* block I/O */
//  (void) memset(((void *) &ThermalModelCoreBPM_B), 0,
//                sizeof(B_ThermalModelCoreBPM_T));
//
//  /* states (dwork) */
//  (void) memset((void *)&ThermalModelCoreBPM_DW, 0,
//                sizeof(DW_ThermalModelCoreBPM_T));
//
//  /* external inputs */
//  (void)memset((void *)&ThermalModelCoreBPM_U, 0, sizeof(ExtU_ThermalModelCoreBPM_T));
//
//  /* external outputs */
//  (void) memset((void *)&ThermalModelCoreBPM_Y, 0,
//                sizeof(ExtY_ThermalModelCoreBPM_T));

  /* InitializeConditions for S-Function (csBPM_final_code): '<S1>/sfn' */
//  real32_T* ic[NDIFF];
//  for(int n = 0; n < NDIFF; n++)
//    {
//      ic[n] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimICs_Value[n];
//    }
//  ic[0] = (real32_T*)ThermalModelCoreBPM_ConstP.MapleSimICs_Value;
//  ic[1] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimICs_Value[1];
//  ic[2] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimICs_Value[2];
//  ic[3] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimICs_Value[3];
//  ic[4] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimICs_Value[4];
//  ic[5] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimICs_Value[5];
//  ic[6] = (real32_T*)&ThermalModelCoreBPM_ConstP.MapleSimICs_Value[6];
  real32_T *x = (real32_T*)&ThermalModelCoreBPM_DW.sfn_DSTATE[0];
  int_T * iw = (int_T*)&ThermalModelCoreBPM_DW.sfn_IWORK[0];
  int_T j;
  iw[INIDREF]= 0;
  for (j=0;j<NDIFF;j++)
    x[j]= 298.15F;//ThermalModelCoreBPM_ConstP.MapleSimICs_Value[j];//ic[j][0];
}

/* Model terminate function */
void ThermalModelCoreBPM_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
