#include "f2c.h"
#include "netlib.h"

/* Modified by Peter Vanroose, June 2001: manual optimisation and clean-up */

/* Table of constant values */
static integer c__1 = 1;

/* Subroutine */ void cqrsl_(x, ldx, n, k, qraux, y, qy, qty, b, rsd, xb, job, info)
const complex *x;
const integer *ldx, *n, *k;
const complex *qraux, *y;
complex *qy, *qty, *b, *rsd, *xb;
integer *job, *info;
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static complex temp;
    static logical cqty;
    static integer i, j;
    static complex t;
    static logical cb;
    static logical cr;
    static integer ju;
    static logical cxb, cqy;

/************************************************************************/
/*                                                                      */
/*     cqrsl applies the output of cqrdc to compute coordinate          */
/*     transformations, projections, and least squares solutions.       */
/*     for k .le. min(n,p), let xk be the matrix                        */
/*                                                                      */
/*            xk = (x(jpvt(1)),x(jpvt(2)), ... ,x(jpvt(k)))             */
/*                                                                      */
/*     formed from columns jpvt(1), ... ,jpvt(k) of the original        */
/*     n x p matrix x that was input to cqrdc (if no pivoting was       */
/*     done, xk consists of the first k columns of x in their           */
/*     original order).  cqrdc produces a factored unitary matrix q     */
/*     and an upper triangular matrix r such that                       */
/*                                                                      */
/*              xk = q * (r)                                            */
/*                       (0)                                            */
/*                                                                      */
/*     this information is contained in coded form in the arrays        */
/*     x and qraux.                                                     */
/*                                                                      */
/*     on entry                                                         */
/*                                                                      */
/*        x      complex(ldx,p).                                        */
/*               x contains the output of cqrdc.                        */
/*                                                                      */
/*        ldx    integer.                                               */
/*               ldx is the leading dimension of the array x.           */
/*                                                                      */
/*        n      integer.                                               */
/*               n is the number of rows of the matrix xk.  it must     */
/*               have the same value as n in cqrdc.                     */
/*                                                                      */
/*        k      integer.                                               */
/*               k is the number of columns of the matrix xk.  k        */
/*               must not be greater than min(n,p), where p is the      */
/*               same as in the calling sequence to cqrdc.              */
/*                                                                      */
/*        qraux  complex(p).                                            */
/*               qraux contains the auxiliary output from cqrdc.        */
/*                                                                      */
/*        y      complex(n)                                             */
/*               y contains an n-vector that is to be manipulated       */
/*               by cqrsl.                                              */
/*                                                                      */
/*        job    integer.                                               */
/*               job specifies what is to be computed.  job has         */
/*               the decimal expansion abcde, with the following        */
/*               meaning.                                               */
/*                                                                      */
/*                    if a.ne.0, compute qy.                            */
/*                    if b,c,d, or e .ne. 0, compute qty.               */
/*                    if c.ne.0, compute b.                             */
/*                    if d.ne.0, compute rsd.                           */
/*                    if e.ne.0, compute xb.                            */
/*                                                                      */
/*               note that a request to compute b, rsd, or xb           */
/*               automatically triggers the computation of qty,         */
/*               for which an array must be provided in the             */
/*               calling sequence.                                      */
/*                                                                      */
/*     on return                                                        */
/*                                                                      */
/*        qy     complex(n).                                            */
/*               qy contains q*y, if its computation has been           */
/*               requested.                                             */
/*                                                                      */
/*        qty    complex(n).                                            */
/*               qty contains ctrans(q)*y, if its computation has       */
/*               been requested.  here ctrans(q) is the conjugate       */
/*               transpose of the matrix q.                             */
/*                                                                      */
/*        b      complex(k)                                             */
/*               b contains the solution of the least squares problem   */
/*                                                                      */
/*                    minimize norm2(y - xk*b),                         */
/*                                                                      */
/*               if its computation has been requested.  (note that     */
/*               if pivoting was requested in cqrdc, the j-th           */
/*               component of b will be associated with column jpvt(j)  */
/*               of the original matrix x that was input into cqrdc.)   */
/*                                                                      */
/*        rsd    complex(n).                                            */
/*               rsd contains the least squares residual y - xk*b,      */
/*               if its computation has been requested.  rsd is         */
/*               also the orthogonal projection of y onto the           */
/*               orthogonal complement of the column space of xk.       */
/*                                                                      */
/*        xb     complex(n).                                            */
/*               xb contains the least squares approximation xk*b,      */
/*               if its computation has been requested.  xb is also     */
/*               the orthogonal projection of y onto the column space   */
/*               of x.                                                  */
/*                                                                      */
/*        info   integer.                                               */
/*               info is zero unless the computation of b has           */
/*               been requested and r is exactly singular.  in          */
/*               this case, info is the index of the first zero         */
/*               diagonal element of r and b is left unaltered.         */
/*                                                                      */
/*     the parameters qy, qty, b, rsd, and xb are not referenced        */
/*     if their computation is not requested and in this case           */
/*     can be replaced by dummy variables in the calling program.       */
/*     to save storage, the user may in some cases use the same         */
/*     array for different parameters in the calling sequence.  a       */
/*     frequently occuring example is when one wishes to compute        */
/*     any of b, rsd, or xb and does not need y or qty.  in this        */
/*     case one may identify y, qty, and one of b, rsd, or xb, while    */
/*     providing separate arrays for anything else that is to be        */
/*     computed.  thus the calling sequence                             */
/*                                                                      */
/*          call cqrsl(x,ldx,n,k,qraux,y,dum,y,b,y,dum,110,info)        */
/*                                                                      */
/*     will result in the computation of b and rsd, with rsd            */
/*     overwriting y.  more generally, each item in the following       */
/*     list contains groups of permissible identifications for          */
/*     a single callinng sequence.                                      */
/*                                                                      */
/*          1. (y,qty,b) (rsd) (xb) (qy)                                */
/*          2. (y,qty,rsd) (b) (xb) (qy)                                */
/*          3. (y,qty,xb) (b) (rsd) (qy)                                */
/*          4. (y,qy) (qty,b) (rsd) (xb)                                */
/*          5. (y,qy) (qty,rsd) (b) (xb)                                */
/*          6. (y,qy) (qty,xb) (b) (rsd)                                */
/*                                                                      */
/*     in any group the value returned in the array allocated to        */
/*     the group corresponds to the last member of the group.           */
/*                                                                      */
/*     linpack. this version dated 08/14/78 .                           */
/*     g.w. stewart, university of maryland, argonne national lab.      */
/*                                                                      */
/*     cqrsl uses the following functions and subprograms.              */
/*                                                                      */
/*     blas caxpy,ccopy,cdotc                                           */
/*     fortran aimag,min0,mod,real                                      */
/*                                                                      */
/************************************************************************/

/*     set info flag. */
    *info = 0;

/*     determine what is to be computed. */

    cqy = *job / 10000 != 0;
    cqty = *job % 10000 != 0;
    cb = *job % 1000 / 100 != 0;
    cr = *job % 100 / 10 != 0;
    cxb = *job % 10 != 0;
    ju = min(*k, *n - 1);

/*     special action when n=1. */

    if (ju == 0) {
        if (cqy) {
            qy[0].r = y[0].r, qy[0].i = y[0].i;
        }
        if (cqty) {
            qty[0].r = y[0].r, qty[0].i = y[0].i;
        }
        if (cxb) {
            xb[0].r = y[0].r, xb[0].i = y[0].i;
        }
        if (cb) {
            if (x[0].r == 0.f && x[0].i == 0.f) {
                *info = 1;
            }
            else {
                c_div(b, y, x);
            }
        }
        if (cr) {
            rsd[0].r = 0.f, rsd[0].i = 0.f;
        }
        return;
    }

/*        set up to compute qy or qty. */

    if (cqy) {
        ccopy_(n, y, &c__1, qy, &c__1);
    }
    if (cqty) {
        ccopy_(n, y, &c__1, qty, &c__1);
    }

/*           compute qy. */

    if (cqy)
    for (j = ju-1; j >= 0; --j) {
        if (qraux[j].r == 0.f && qraux[j].i == 0.f) {
            continue; /* next j */
        }
        i__1 = j * *ldx + j; /* index [j,j] */
        temp.r = x[i__1].r, temp.i = x[i__1].i;
        ((complex*)x)[i__1].r = qraux[j].r, ((complex*)x)[i__1].i = qraux[j].i; /* temporarily */
        i__2 = *n - j;
        cdotc_(&t, &i__2, &x[i__1], &c__1, &qy[j], &c__1);
        c_div(&t, &t, &x[i__1]);
        t.r = -t.r, t.i = -t.i;
        caxpy_(&i__2, &t, &x[i__1], &c__1, &qy[j], &c__1);
        ((complex*)x)[i__1].r = temp.r, ((complex*)x)[i__1].i = temp.i; /* restore original */
    }

/*           compute ctrans(q)*y. */

    if (cqty)
    for (j = 0; j < ju; ++j) {
        if (qraux[j].r == 0.f && qraux[j].i == 0.f) {
            continue; /* next j */
        }
        i__1 = j * *ldx + j; /* index [j,j] */
        temp.r = x[i__1].r, temp.i = x[i__1].i;
        ((complex*)x)[i__1].r = qraux[j].r, ((complex*)x)[i__1].i = qraux[j].i; /* temporarily */
        i__2 = *n - j;
        cdotc_(&t, &i__2, &x[i__1], &c__1, &qty[j], &c__1);
        c_div(&t, &t, &x[i__1]);
        t.r = -t.r, t.i = -t.i;
        caxpy_(&i__2, &t, &x[i__1], &c__1, &qty[j], &c__1);
        ((complex*)x)[i__1].r = temp.r, ((complex*)x)[i__1].i = temp.i; /* restore original */
    }

/*        set up to compute b, rsd, or xb. */

    if (cb) {
        ccopy_(k, qty, &c__1, b, &c__1);
    }
    if (cxb) {
        ccopy_(k, qty, &c__1, xb, &c__1);
    }
    if (cr && *k < *n) {
        i__2 = *n - *k;
        ccopy_(&i__2, &qty[*k], &c__1, &rsd[*k], &c__1);
    }
    if (cxb && *k < *n)
    for (i = *k; i < *n; ++i) {
        xb[i].r = 0.f, xb[i].i = 0.f;
    }
    if (cr)
    for (i = 0; i < *k; ++i) {
        rsd[i].r = 0.f, rsd[i].i = 0.f;
    }

/*           compute b. */

    if (cb)
    for (j = *k-1; j >= 0; --j) {
        i__1 = j * *ldx + j; /* index [j,j] */
        if (x[i__1].r == 0.f && x[i__1].i == 0.f) {
            *info = j+1;
            break; /* last j */
        }
        c_div(&b[j], &b[j], &x[i__1]);
        if (j == 0) {
            break; /* last j */
        }
        t.r = -b[j].r, t.i = -b[j].i;
        caxpy_(&j, &t, &x[j* *ldx], &c__1, b, &c__1);
    }

/*           compute rsd or xb as required. */

    if (cr || cxb)
    for (j = ju-1; j >= 0; --j) {
        if (qraux[j].r == 0.f && qraux[j].i == 0.f) {
            continue; /* next j */
        }
        i__1 = j * *ldx + j; /* index [j,j] */
        temp.r = x[i__1].r, temp.i = x[i__1].i;
        ((complex*)x)[i__1].r = qraux[j].r, ((complex*)x)[i__1].i = qraux[j].i; /* temporarily */
        i__2 = *n - j;
        if (cr) {
            cdotc_(&t, &i__2, &x[i__1], &c__1, &rsd[j], &c__1);
            c_div(&t, &t, &x[i__1]);
            t.r = -t.r, t.i = -t.i;
            caxpy_(&i__2, &t, &x[i__1], &c__1, &rsd[j], &c__1);
        }
        if (cxb) {
            cdotc_(&t, &i__2, &x[i__1], &c__1, &xb[j], &c__1);
            c_div(&t, &t, &x[i__1]);
            t.r = -t.r, t.i = -t.i;
            caxpy_(&i__2, &t, &x[i__1], &c__1, &xb[j], &c__1);
        }
        ((complex*)x)[i__1].r = temp.r, ((complex*)x)[i__1].i = temp.i; /* restore original */
    }
} /* cqrsl_ */
