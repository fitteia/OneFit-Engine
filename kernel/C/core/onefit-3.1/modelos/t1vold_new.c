/* t1vold_new_alex.f -- translated by f2c (version 19960717).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/
#ifdef MacOSX
#include "/sw/include/f2c.h"
#endif

/* Common Block Declarations */

struct {
    doublereal szz, dperp, dpar, k1, p, afact0, afact1, afact2;
} bb_;

#define bb_1 bb_


/* Subroutine */ int t1vold_new__(f, delta, s, tau_s__, tau_l__, k_1__, p_p__,
	 a, b, c__, t1_ang__, j0, j1, j2, a_fact0__, a_fact1__, a_fact2__)
doublereal *f, *delta, *s, *tau_s__, *tau_l__, *k_1__, *p_p__, *a, *b, *c__, *
	t1_ang__, *j0, *j1, *j2, *a_fact0__, *a_fact1__, *a_fact2__;
{
    /* System generated locals */
    real r__1;
    doublereal d__1;

    /* Builtin functions */
    double sin();

    /* Local variables */
    static integer alfa;
    extern doublereal taul_();
    static doublereal vjor[1200]	/* was [3][200][2] */;
    static integer i__, k, l;
    static doublereal cklal[100]	/* was [10][10] */, vjota[300]	/* 
	    was [3][100] */;
    static real w2, ff;
    extern doublereal al_();
    static integer ij, kk, ll, ko;
    extern doublereal tl_();
    static real ct2;
    static doublereal vj0[2], vj1[2], vj2[2];
    static real aaa, del, tbb, tcc;
    static doublereal tau[1000]	/* was [10][10][10] */;
    static integer nnn;
    static real som, sum, sen2, sen4;





/*        type *,'ola1' */
    /* Parameter adjustments */
    c__ -= 11;
    b -= 111;
    a -= 111;

    /* Function Body */
    bb_1.szz = *s;
    bb_1.k1 = *k_1__;
    bb_1.p = *p_p__;
    bb_1.dperp = (float)1. / (*tau_s__ * (float)6.);
    bb_1.dpar = (float)1. / (*tau_l__ * (float)6.);
    bb_1.afact0 = *a_fact0__;
    bb_1.afact1 = *a_fact1__;
    bb_1.afact2 = *a_fact2__;

/*        type *,f,delta,S,Tau_S */
    for (k = 0; k <= 2; ++k) {
	kk = k + 1;
	if (k == 0) {
	    ct2 = (float).66666666666666663;
	} else if (k == 1) {
	    ct2 = (float).1111111111111111;
	} else if (k == 2) {
	    ct2 = (float).44444444444444442;
	} else {
	}
	for (ko = 1; ko <= 2; ++ko) {
	    ff = ko * *f;
/* Computing 2nd power */
	    r__1 = ff * (float)6.2831799999999998;
	    w2 = r__1 * r__1;
	    som = (float)0.;

	    for (l = -2; l <= 2; ++l) {
		ll = l + 3;
		sum = (float)0.;

		for (alfa = 1; alfa <= 3; ++alfa) {
		    if (k == 0 && l == 0 && alfa != 3) {
			goto L444;
		    } else if (k == 1 && l == 0 && alfa != 3) {
			goto L444;
		    } else if (k == 1 && abs(l) == 1) {
			goto L444;
		    } else if (k == 1 && abs(l) == 2) {
			goto L444;
		    } else if (k == 2 && l == 0 && alfa != 3) {
			goto L444;
		    } else if (k == 2 && abs(l) == 2 && alfa == 1) {
			goto L444;
		    } else {
			goto L420;
		    }
L444:
		    cklal[kk + ll * 10 - 11] = c__[kk + ll * 10] * al_(&l);
		    if (l == 0) {
			tau[kk + (ll + alfa * 10) * 10 - 111] = b[kk + (ll + 
				alfa * 10) * 10] * taul_(&l);
		    } else {
			tbb = b[kk + (ll + alfa * 10) * 10] * taul_(&l) * tl_(
				&l);
			tcc = tbb / (b[kk + (ll + alfa * 10) * 10] * taul_(&l)
				 + tl_(&l));
			tau[kk + (ll + alfa * 10) * 10 - 111] = tcc;
		    }
		    aaa = tau[kk + (ll + alfa * 10) * 10 - 111] * (float)2.;
/* Computing 2nd power */
		    d__1 = tau[kk + (ll + alfa * 10) * 10 - 111];
		    aaa /= w2 * (d__1 * d__1) + (float)1.;
		    aaa = aaa * ct2 * a[kk + (ll + alfa * 10) * 10];
		    sum += aaa;
L420:
		    ;
		}
		som += sum * cklal[kk + ll * 10 - 11];
/* L410: */
	    }
	    vjota[kk + ko * 3 - 4] = som * (float)6.406e11;
/* 	   aww = alog(w) */
/*          ajj = alog(vjota(kk,ko)) */
/* L405: */
	}
/* L400: */
    }
/*        type *,'ola f' */

    for (ij = 1; ij <= 2; ++ij) {
	vj0[ij - 1] = vjota[ij * 3 - 3];
	vj1[ij - 1] = vjota[ij * 3 - 2];
	vj2[ij - 1] = vjota[ij * 3 - 1];
/* L331: */
    }
    nnn = 1;
    del = *delta * (float)3.14159236 / (float)180.;
/* Computing 2nd power */
    r__1 = sin(del);
    sen2 = r__1 * r__1;
/* Computing 4th power */
    r__1 = sin(del), r__1 *= r__1;
    sen4 = r__1 * r__1;

    for (i__ = 1; i__ <= 2; ++i__) {
	vjor[(nnn + i__ * 200) * 3 - 603] = ((float)1. - sen2 * (float)3. + 
		sen4 * (float)2.25) * vj0[i__ - 1] + (sen2 - sen4) * (float)
		18. * vj1[i__ - 1] + sen4 * (float)1.125 * vj2[i__ - 1];
	vjor[(nnn + i__ * 200) * 3 - 602] = (sen2 - sen4) * (float).25 * vj0[
		i__ - 1] + ((float)1. - sen2 * (float)2.5 + sen4 * (float)2.) 
		* vj1[i__ - 1] + (sen2 * (float)2. - sen4) * (float).125 * 
		vj2[i__ - 1];
	vjor[(nnn + i__ * 200) * 3 - 601] = sen4 * (float).25 * vj0[i__ - 1] 
		+ (sen2 * (float)2. - sen4) * (float)2. * vj1[i__ - 1] + ((
		float)1. - sen2 + sen4 * (float).125) * vj2[i__ - 1];

/* ---- Para um policristal: ------ */

/* 	vjor(2,nnn,i) = vj0(i)/30. + vj1(i)*0.4 + vj2(i)/10.0 */
/* 	vjor(3,nnn,i) = vj0(i)*(2./15.) + vj1(i)*(8./5.) + vj2(i)*0.4 */

/* L333: */
    }
    *j0 = vjor[0];
    *j1 = vjor[1];
    *j2 = vjor[2];
    *t1_ang__ = vjor[(nnn + 200) * 3 - 602] + vjor[(nnn + 400) * 3 - 601];

    return 0;
} /* t1vold_new__ */




doublereal al_(l)
integer *l;
{
    /* System generated locals */
    real ret_val;


    if (*l == -2 || *l == 2) {
	ret_val = bb_1.afact2 * (float)3.;
    } else if (*l == -1 || *l == 1) {
	ret_val = bb_1.afact1 * (float)3.;
    } else if (*l == 0) {
	ret_val = bb_1.afact0 * (float)6.;
    } else {
    }

    return ret_val;
} /* al_ */




doublereal tl_(l)
integer *l;
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    static doublereal k2;


    k2 = (bb_1.p * (float)3. + (float)1.) * bb_1.k1;
    if (*l == -2 || *l == 2) {
	ret_val = (float)1. / k2;
    } else if (*l == -1 || *l == 1) {
	ret_val = (float)1. / bb_1.k1;
    } else {
	ret_val = (float)0.;
    }

    return ret_val;
} /* tl_ */




doublereal taul_(l)
integer *l;
{
    /* System generated locals */
    real ret_val;


    if (*l == -2 || *l == 2) {
	ret_val = (float)1. / (bb_1.dperp * (float)2. + bb_1.dpar * (float)4.)
		;
    } else if (*l == -1 || *l == 1) {
	ret_val = (float)1. / (bb_1.dperp * (float)5. + bb_1.dpar);
    } else if (*l == 0) {
	ret_val = (float)1. / (bb_1.dperp * (float)6.);
    } else {
/* 	      type *,'Tau(l) nao definido' */
    }

    return ret_val;
} /* taul_ */

