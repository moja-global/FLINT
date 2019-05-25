#ifndef LMCONTROL_INCLUDED
#define LMCONTROL_INCLUDED

#include "moja/flint/flint.h"
#include "moja/flint/modulemanager.h"

#include <array>
#include <vector>

namespace moja {
namespace flint {

enum class LMMinInfo {
   /// <summary>
   /// termination requested by user-supplied routine evaluate;
   /// </summary>
   Terminated = -1,
   /// <summary>
   ///  improper input parameters;
   /// </summary>
   InproperInput = 0,
   /// <summary>
   /// both actual and predicted relative reductions
   /// in the sum of squares are at most ftol;
   /// </summary>
   FTOL = 1,
   /// <summary>
   /// relative error between two consecutive iterates
   ///	is at most xtol;
   /// </summary>
   XTOL = 2,
   /// <summary>
   /// conditions for FTOL and XTOL both hold
   /// </summary>
   FTOLAndXTOL = 3,
   /// <summary>
   /// the cosine of the angle between fvec and any
   /// column of the jacobian is at most gtol in
   /// absolute value
   /// </summary>
   GTOLCosine = 4,
   /// <summary>
   /// number of calls to lm_fcn has reached or
   /// exceeded maxfev;
   /// </summary>
   MaxCalls = 5,
   /// <summary>
   /// ftol is too small. no further reduction in
   ///	    the sum of squares is possible;
   /// </summary>
   FTOLTooSmall = 6,
   /// <summary>
   /// info = 7  xtol is too small. no further improvement in
   ///	the approximate solution x is possible;
   /// </summary>
   XTOLTooSmall = 7,
   /// <summary>
   /// gtol is too small. fvec is orthogonal to the
   /// columns of the jacobian to machine precision;
   /// </summary>
   GTOLTooSmall = 8,
   a = 9,
   b = 10
};

/// <summary>
/// ADT - control object used in LMMin computing
/// </summary>
struct LMControl {
   double ftol;      /* relative error desired in the sum of squares. */
   double xtol;      /* relative error between last two approximations. */
   double gtol;      /* orthogonality desired between fvec and its derivs. */
   double epsilon;   /* step used to calculate the jacobian. */
   double stepbound; /* initial bound to steps in the outer loop. */
   double fnorm;     /* norm of the residue vector fvec. */
   int maxcall;      /* maximum number of iterations. */
   int nfev;         /* actual number of iterations. */
   LMMinInfo info;   /* status of minimization. */

   LMControl() {
      ftol = 1.0e-14;
      xtol = 1.0e-14;
      gtol = 1.0e-14;
      epsilon = 1.0e-14;
      stepbound = 100.0;
      maxcall = 1000;
   }
};

/// <summary>
/// Project LevenbergMarquardtLeastSquaresFitting
/// Release lmfit2.3
///
/// Based on lmdif and other routines from the public-domain library
/// netlib::Minpack, Argonne National Laboratories, March 1980,
/// by Burton S. Garbow, Kenneth E. Hillstrom, Jorge J. More.
/// C translation by Steve Moshier. Code converted into C++
/// compatible ANSI style and wrapped by Joachim Wuttke, 2004-
///
/// Web sites: http://www.messen-und-deuten.de/lmfit/index.html
///           http://sourceforge.net/projects/lmfit/
///
/// Bug reports, feature requests, and other comments: mail to
/// Joachim Wuttke, (first name).(last name)@messen-und-deuten.de
///
/// File lmmin.c
///
/// Solves or Minimizes the sum of squares of m nonlinear functions
/// of n variables.
///
/// Converted to C# by Scott Morken Nov 2010
/// </summary>
/// <typeparam name="T">The type of data to be fitted</typeparam>
class LMMin {
   static double Square(double x) { return x * x; }

   /* the following values seem good for an x86: */
   /// <summary>
   /// resolution of arithmetic
   /// </summary>
   const double LM_MACHEP = 0.555e-16;
   /// <summary>
   /// smallest nonzero number
   /// </summary>
   const double LM_DWARF = 9.9e-324;
   /// <summary>
   /// square should not underflow
   /// </summary>
   const double LM_SQRT_DWARF = 1.0e-160;
   /// <summary>
   /// square should not overflow
   /// </summary>
   const double LM_SQRT_GIANT = 1.0e150;

  public:
   /// <summary>
   ///
   /// </summary>
   /// <param name="m_dat"></param>
   /// <param name="n_par"></param>
   /// <param name="par"></param>
   /// <param name="evaluate"></param>
   /// <param name="printout"></param>
   /// <param name="control"></param>
   /// <param name="xValue"></param>
   /// <param name="yValue"></param>
   void lmMinimize(int m_dat, int n_par, double[] par, LmEvaluate evaluate, LmPrintOut printout, LMControl control,
                   double[] xValue, double[] yValue) {
      /*** allocate work space. ***/
      double[] fvec, diag, fjac, qtf, wa1, wa2, wa3, wa4;
      int[] ipvt;

      int n = n_par;
      int m = m_dat;

      fvec = new double[m];
      diag = new double[n];
      qtf = new double[m];
      fjac = new double[n * m];
      wa1 = new double[n];
      wa2 = new double[n];
      wa3 = new double[n];
      wa4 = new double[m];
      ipvt = new int[n];
      control.info = LMMinInfo.a;  // 9

      /*** perform fit. ***/

      control.info = 0;
      control.nfev = 0;

      /* this goes through the modified legacy interface: */
      lm_lmdif(m, n, par, fvec, control.ftol, control.xtol, control.gtol, control.maxcall * (n + 1), control.epsilon,
               diag, 1, control.stepbound, control.info, ref control.nfev, fjac, ipvt, qtf, wa1, wa2, wa3, wa4,
               evaluate, printout, xValue, yValue);

      printout(n, par, m, fvec, -1, 0, control.nfev, xValue, yValue);
      control.fnorm = lm_enorm(m, 0, fvec);

      if (control.info < 0) control.info = LMMinInfo::b;  // 10;

   } /*** lm_Minimize. ***/

   /// <summary>
   /// the purpose of lmdif is to minimize the sum of the squares of
   /// m nonlinear functions in n variables by a modification of
   /// the levenberg-marquardt algorithm. the user must provide a
   /// subroutine evaluate which calculates the functions. the jacobian
   /// is then calculated by a forward-difference approximation.
   ///
   /// the multi-parameter interface lm_lmdif is for users who want
   /// full control and flexibility. most users will be better off using
   /// the simpler interface lm_minimize provided above.
   /// the parameters are the same as in the legacy FORTRAN implementation,
   /// with the following exceptions:
   ///
   ///   the old parameter ldfjac which gave leading dimension of fjac has
   ///     been deleted because this C translation makes no use of two-
   ///     dimensional arrays;
   ///   the old parameter nprint has been deleted; printout is now controlled
   ///     by the user-supplied routine *printout;
   ///   the parameter field *data and the function parameters *evaluate and
   ///     *printout have been added; they help avoiding global variables.
   /// </summary>
   /// <param name="m">m is a positive integer input variable set to the number
   ///  of functions.</param>
   /// <param name="n">n is a positive integer input variable set to the number
   ///  of variables. n must not exceed m.</param>
   /// <param name="x">x is an array of length n. on input x must contain
   ///  an initial estimate of the solution vector. on output x
   ///  contains the final estimate of the solution vector.</param>
   /// <param name="fvec">fvec is an output array of length m which contains
   ///  the functions evaluated at the output x.</param>
   /// <param name="ftol">ftol is a nonnegative input variable. termination
   ///  occurs when both the actual and predicted relative
   ///  reductions in the sum of squares are at most ftol.
   ///  therefore, ftol measures the relative error desired
   ///  in the sum of squares.</param>
   /// <param name="xtol">xtol is a nonnegative input variable. termination
   ///  occurs when the relative error between two consecutive
   ///  iterates is at most xtol. therefore, xtol measures the
   ///  relative error desired in the approximate solution.</param>
   /// <param name="gtol">gtol is a nonnegative input variable. termination
   ///  occurs when the cosine of the angle between fvec and
   ///  any column of the jacobian is at most gtol in absolute
   ///  value. therefore, gtol measures the orthogonality
   ///  desired between the function vector and the columns
   ///  of the jacobian.</param>
   /// <param name="maxfev">maxfev is a positive integer input variable. termination
   ///  occurs when the number of calls to lm_fcn is at least
   ///  maxfev by the end of an iteration.</param>
   /// <param name="epsfcn">epsfcn is an input variable used in determining a suitable
   ///  step length for the forward-difference approximation. this
   ///  approximation assumes that the relative errors in the
   ///  functions are of the order of epsfcn. if epsfcn is less
   ///  than the machine precision, it is assumed that the relative
   ///  errors in the functions are of the order of the machine
   ///  precision.</param>
   /// <param name="diag">diag is an array of length n. if mode = 1 (see below), diag is
   /// internally set. if mode = 2, diag must contain positive entries
   /// that serve as multiplicative scale factors for the variables.</param>
   /// <param name="mode">mode is an integer input variable. if mode = 1, the
   ///  variables will be scaled internally. if mode = 2,
   ///  the scaling is specified by the input diag. other
   ///  values of mode are equivalent to mode = 1.</param>
   /// <param name="factor">factor is a positive input variable used in determining the
   ///  initial step bound. this bound is set to the product of
   ///  factor and the euclidean norm of diag*x if nonzero, or else
   ///  to factor itself. in most cases factor should lie in the
   ///  interval (.1,100.0). 100. is a generally recommended value.</param>
   /// <param name="info">ndicates the termination status of lm_lmdif</param>
   /// <param name="nfev">nfev is an output variable set to the number of calls to the
   /// user-supplied routine *evaluate.</param>
   /// <param name="fjac">fjac is an output m by n array. the upper n by n submatrix
   ///  of fjac contains an upper triangular matrix r with
   ///  diagonal elements of nonincreasing magnitude such that
   ///	 t     t	   t
   ///	p *(jac *jac)*p = r *r,
   ///  where p is a permutation matrix and jac is the final
   ///  calculated jacobian. column j of p is column ipvt(j)
   ///  (see below) of the identity matrix. the lower trapezoidal
   ///  part of fjac contains information generated during
   ///  the computation of r.</param>
   /// <param name="ipvt">ipvt is an integer output array of length n. ipvt
   ///  defines a permutation matrix p such that jac*p = q*r,
   ///  where jac is the final calculated jacobian, q is
   ///  orthogonal (not stored), and r is upper triangular
   ///  with diagonal elements of nonincreasing magnitude.
   ///  column j of p is column ipvt(j) of the identity matrix.</param>
   /// <param name="qtf">qtf is an output array of length n which contains
   ///  the first n elements of the vector (q transpose)*fvec.</param>
   /// <param name="wa1">work array of length n.</param>
   /// <param name="wa2">work array of length n.</param>
   /// <param name="wa3">work array of length n.</param>
   /// <param name="wa4">wa4 is a work array of length m.</param>
   /// <param name="evaluate">subroutine which calculates the functions</param>
   /// <param name="printout">the subroutine which nforms about fit progress</param>
   /// <param name="data">the data to be fitted</param>
   void lm_lmdif(int m, int n, double* x, double* fvec, double ftol, double xtol, double gtol, int maxfev,
                 double epsfcn, double* diag, int mode, double factor, LMMinInfo info, int& nfev, double* fjac,
                 int* ipvt, double* qtf, double* wa1, double* wa2, double* wa3, double* wa4, LmEvaluate evaluate,
                 LmPrintOut printout, double[] xValue, double[] yValue) {
      int i, iter, j;
      double actred, delta, dirder, eps, fnorm, fnorm1, gnorm, par, pnorm, prered, ratio, step, sum, temp, temp1, temp2,
          temp3, xnorm;
      const double p1 = 0.1;
      const double p5 = 0.5;
      const double p25 = 0.25;
      const double p75 = 0.75;
      const double p0001 = 1.0e-4;

      nfev = 0;  /* function evaluation counter */
      iter = 1;  /* outer loop counter */
      par = 0;   /* levenberg-marquardt parameter */
      delta = 0; /* to prevent a warning (initialization within if-clause) */
      xnorm = 0; /* ditto */
      temp = std::max(epsfcn, LM_MACHEP);
      eps = std::sqrt(temp); /* for calculating the Jacobian by forward differences */

      /*** lmdif: check input parameters for errors. ***/

      if ((n <= 0) || (m < n) || (ftol < 0.0) || (xtol < 0.0) || (gtol < 0.0) || (maxfev <= 0) || (factor <= 0.0)) {
         info = LMMinInfo::InproperInput;  // invalid parameter
         return;
      }
      if (mode == 2) {             /* scaling by diag[] */
         for (j = 0; j < n; j++) { /* check for nonpositive elements */
            if (diag[j] <= 0.0) {
               info = LMMinInfo::InproperInput;  // invalid parameter
               return;
            }
         }
      }
#if BUG
      printf("lmdif\n");
#endif

      /*** lmdif: evaluate function at starting point and calculate norm. ***/

      info = LMMinInfo::InproperInput;
      evaluate(x, m, fvec, xValue, yValue);
      printout(n, x, m, fvec, 0, 0, nfev++, xValue, yValue);
      if (info < LMMinInfo::InproperInput) return;
      fnorm = lm_enorm(m, 0, fvec);

      /*** lmdif: the outer loop. ***/

      do {
#if BUG
         printf("lmdif/ outer loop iter=%d nfev=%d fnorm=%.10e\n", iter, nfev, fnorm);
#endif

         /*** outer: calculate the jacobian matrix. ***/

         for (j = 0; j < n; j++) {
            temp = x[j];
            step = eps * std::abs(temp);
            if (step == 0.0) step = eps;
            x[j] = temp + step;
            info = LMMinInfo::InproperInput;
            evaluate(x, m, wa4, xValue, yValue);
            printout(n, x, m, wa4, 1, iter, nfev++, xValue, yValue);
            if (info < LMMinInfo::InproperInput) return; /* user requested break */
            for (i = 0; i < m; i++)                      /* changed in 2.3, Mark Bydder */
               fjac[j * m + i] = (wa4[i] - fvec[i]) / (x[j] - temp);
            x[j] = temp;
         }
         //#if BUG>1
         //    /* DEBUG: print the entire matrix */
         //    for (i = 0; i < m; i++) {
         //        for (j = 0; j < n; j++)
         //        printf("%.5e ", fjac[j * m + i]);
         //        printf("\n");
         //    }
         //#endif

         /*** outer: compute the qr factorization of the jacobian. ***/

         lm_qrfac(m, n, fjac, true, ipvt, wa1, wa2, wa3);

         if (iter == 1) { /* first iteration */
            if (mode != 2) {
               /* diag := norms of the columns of the initial jacobian */
               for (j = 0; j < n; j++) {
                  diag[j] = wa2[j];
                  if (wa2[j] == 0.0) diag[j] = 1.0;
               }
            }
            /* use diag to scale x, then calculate the norm */
            for (j = 0; j < n; j++) wa3[j] = diag[j] * x[j];
            xnorm = lm_enorm(n, 0, wa3);
            /* initialize the step bound delta. */
            delta = factor * xnorm;
            if (delta == 0.0) delta = factor;
         }

         /*** outer: form (q transpose)*fvec and store first n components in qtf. ***/

         for (i = 0; i < m; i++) wa4[i] = fvec[i];

         for (j = 0; j < n; j++) {
            temp3 = fjac[j * m + j];
            if (temp3 != 0.0) {
               sum = 0;
               for (i = j; i < m; i++) sum += fjac[j * m + i] * wa4[i];
               temp = -sum / temp3;
               for (i = j; i < m; i++) wa4[i] += fjac[j * m + i] * temp;
            }
            fjac[j * m + j] = wa1[j];
            qtf[j] = wa4[j];
         }

         /** outer: compute norm of scaled gradient and test for convergence. ***/

         gnorm = 0;
         if (fnorm != 0) {
            for (j = 0; j < n; j++) {
               if (wa2[ipvt[j]] == 0) continue;

               sum = 0.0;
               for (i = 0; i <= j; i++) sum += fjac[j * m + i] * qtf[i] / fnorm;
               gnorm = std::max(gnorm, std::abs(sum / wa2[ipvt[j]]));
            }
         }

         if (gnorm <= gtol) {
            info = LMMinInfo::GTOLCosine;
            return;
         }

         /*** outer: rescale if necessary. ***/

         if (mode != 2) {
            for (j = 0; j < n; j++) diag[j] = std::max(diag[j], wa2[j]);
         }

         /*** the inner loop. ***/
         do {
#if BUG
            printf("lmdif/ inner loop iter=%d nfev=%d\n", iter, nfev);
#endif

            /*** inner: determine the levenberg-marquardt parameter. ***/

            lm_lmpar(n, fjac, m, ipvt, diag, qtf, delta, par, wa1, wa2, wa3, wa4);

            /*** inner: store the direction p and x + p; calculate the norm of p. ***/

            for (j = 0; j < n; j++) {
               wa1[j] = -wa1[j];
               wa2[j] = x[j] + wa1[j];
               wa3[j] = diag[j] * wa1[j];
            }
            pnorm = lm_enorm(n, 0, wa3);

            /*** inner: on the first iteration, adjust the initial step bound. ***/

            if (nfev <= 1 + n) delta = std::min(delta, pnorm);

            /* evaluate the function at x + p and calculate its norm. */

            info = LMMinInfo::InproperInput;
            evaluate(wa2, m, wa4, xValue, yValue);
            printout(n, x, m, wa4, 2, iter, nfev++, xValue, yValue);
            if (info < LMMinInfo::InproperInput) return; /* user requested break. */

            fnorm1 = lm_enorm(m, 0, wa4);
#if BUG
            printf(
                "lmdif/ pnorm %.10e  fnorm1 %.10e  fnorm %.10e"
                " delta=%.10e par=%.10e\n",
                pnorm, fnorm1, fnorm, delta, par);
#endif

            /*** inner: compute the scaled actual reduction. ***/

            if (p1 * fnorm1 < fnorm)
               actred = 1 - Square(fnorm1 / fnorm);
            else
               actred = -1;

            /*** inner: compute the scaled predicted reduction and
            the scaled directional derivative. ***/

            for (j = 0; j < n; j++) {
               wa3[j] = 0;
               for (i = 0; i <= j; i++) wa3[i] += fjac[j * m + i] * wa1[ipvt[j]];
            }
            temp1 = lm_enorm(n, 0, wa3) / fnorm;
            temp2 = std::sqrt(par) * pnorm / fnorm;
            prered = Square(temp1) + 2 * Square(temp2);
            dirder = -(Square(temp1) + Square(temp2));

            /*** inner: compute the ratio of the actual to the predicted reduction. ***/

            ratio = prered != 0 ? actred / prered : 0;
#if BUG
            printf(
                "lmdif/ actred=%.10e prered=%.10e ratio=%.10e"
                " sq(1)=%.10e sq(2)=%.10e dd=%.10e\n",
                actred, prered, prered != 0 ? ratio : 0.0, SQR(temp1), SQR(temp2), dirder);
#endif

            /*** inner: update the step bound. ***/

            if (ratio <= p25) {
               if (actred >= 0.0)
                  temp = p5;
               else
                  temp = p5 * dirder / (dirder + p5 * actred);
               if (p1 * fnorm1 >= fnorm || temp < p1) temp = p1;
               delta = temp * std::min(delta, pnorm / p1);
               par /= temp;
            } else if (par == 0.0 || ratio >= p75) {
               delta = pnorm / p5;
               par *= p5;
            }

            /*** inner: test for successful iteration. ***/

            if (ratio >= p0001) {
               /* yes, success: update x, fvec, and their norms. */
               for (j = 0; j < n; j++) {
                  x[j] = wa2[j];
                  wa2[j] = diag[j] * x[j];
               }
               for (i = 0; i < m; i++) fvec[i] = wa4[i];
               xnorm = lm_enorm(n, 0, wa2);
               fnorm = fnorm1;
               iter++;
            }
#if BUG
            else {
               printf("ATTN: iteration considered unsuccessful\n");
            }
#endif

            /*** inner: tests for convergence ( otherwise info = 1, 2, or 3 ). ***/

            info = LMMinInfo::InproperInput; /* do not terminate (unless overwritten by nonzero) */
            if (std::abs(actred) <= ftol && prered <= ftol && p5 * ratio <= 1) info = LMMinInfo::FTOL;
            if (delta <= xtol * xnorm) info += 2;
            if (info != LMMinInfo::InproperInput) return;

            /*** inner: tests for termination and stringent tolerances. ***/

            if (nfev >= maxfev) info = (LMMinInfo)5;
            if (std::abs(actred) <= LM_MACHEP && prered <= LM_MACHEP && p5 * ratio <= 1) info = (LMMinInfo)6;
            if (delta <= LM_MACHEP * xnorm) info = (LMMinInfo)7;
            if (gnorm <= LM_MACHEP) info = (LMMinInfo)8;
            if (info != LMMinInfo::InproperInput) return;

            /*** inner: end of the loop. repeat if iteration unsuccessful. ***/

         } while (ratio < p0001);

         /*** outer: end of the loop. ***/

      } while (true);

   } /*** lm_lmdif. ***/

   void lm_lmpar(int n, double* r, int ldr, int* ipvt, double* diag, double* qtb, double delta, double& par, double* x,
                 double* sdiag, double* wa1, double* wa2) {
      /*     given an m by n matrix a, an n by n nonsingular diagonal
       *     matrix d, an m-vector b, and a positive number delta,
       *     the problem is to detemine a value for the parameter
       *     par such that if x solves the system
       *
       *	    a*x = b ,	  std::sqrt(par)*d*x = 0 ,
       *
       *     in the least squares sense, and dxnorm is the euclidean
       *     norm of d*x, then either par is 0. and
       *
       *	    (dxnorm-delta) .le. 0.1*delta ,
       *
       *     or par is positive and
       *
       *	    abs(dxnorm-delta) .le. 0.1*delta .
       *
       *     this subroutine completes the solution of the problem
       *     if it is provided with the necessary information from the
       *     qr factorization, with column pivoting, of a. that is, if
       *     a*p = q*r, where p is a permutation matrix, q has orthogonal
       *     columns, and r is an upper triangular matrix with diagonal
       *     elements of nonincreasing magnitude, then lmpar expects
       *     the full upper triangle of r, the permutation matrix p,
       *     and the first n components of (q transpose)*b. on output
       *     lmpar also provides an upper triangular matrix s such that
       *
       *	     t	 t		     t
       *	    p *(a *a + par*d*d)*p = s *s .
       *
       *     s is employed within lmpar and may be of separate interest.
       *
       *     only a few iterations are generally needed for convergence
       *     of the algorithm. if, however, the limit of 10 iterations
       *     is reached, then the output par will contain the best
       *     value obtained so far.
       *
       *     parameters:
       *
       *	n is a positive integer input variable set to the order of r.
       *
       *	r is an n by n array. on input the full upper triangle
       *	  must contain the full upper triangle of the matrix r.
       *	  on output the full upper triangle is unaltered, and the
       *	  strict lower triangle contains the strict upper triangle
       *	  (transposed) of the upper triangular matrix s.
       *
       *	ldr is a positive integer input variable not less than n
       *	  which specifies the leading dimension of the array r.
       *
       *	ipvt is an integer input array of length n which defines the
       *	  permutation matrix p such that a*p = q*r. column j of p
       *	  is column ipvt(j) of the identity matrix.
       *
       *	diag is an input array of length n which must contain the
       *	  diagonal elements of the matrix d.
       *
       *	qtb is an input array of length n which must contain the first
       *	  n elements of the vector (q transpose)*b.
       *
       *	delta is a positive input variable which specifies an upper
       *	  bound on the euclidean norm of d*x.
       *
       *	par is a nonnegative variable. on input par contains an
       *	  initial estimate of the levenberg-marquardt parameter.
       *	  on output par contains the final estimate.
       *
       *	x is an output array of length n which contains the least
       *	  squares solution of the system a*x = b, std::sqrt(par)*d*x = 0,
       *	  for the output par.
       *
       *	sdiag is an output array of length n which contains the
       *	  diagonal elements of the upper triangular matrix s.
       *
       *	wa1 and wa2 are work arrays of length n.
       *
       */
      int i, iter, j, nsing;
      double dxnorm, fp, fp_old, gnorm, parc, parl, paru;
      double sum, temp;
      const double p1 = 0.1;
      const double p001 = 0.001;

#if BUG
      printf("lmpar\n");
#endif

      /*** lmpar: compute and store in x the gauss-newton direction. if the
      jacobian is rank-deficient, obtain a least squares solution. ***/

      nsing = n;
      for (j = 0; j < n; j++) {
         wa1[j] = qtb[j];
         if (r[j * ldr + j] == 0 && nsing == n) nsing = j;
         if (nsing < n) wa1[j] = 0;
      }
#if BUG
      printf("nsing %d ", nsing);
#endif
      for (j = nsing - 1; j >= 0; j--) {
         wa1[j] = wa1[j] / r[j + ldr * j];
         temp = wa1[j];
         for (i = 0; i < j; i++) wa1[i] -= r[j * ldr + i] * temp;
      }

      for (j = 0; j < n; j++) x[ipvt[j]] = wa1[j];

      // lmpar: initialize the iteration counter, evaluate the function at the
      //     origin, and test for acceptance of the gauss-newton direction.

      iter = 0;
      for (j = 0; j < n; j++) wa2[j] = diag[j] * x[j];
      dxnorm = lm_enorm(n, 0, wa2);
      fp = dxnorm - delta;
      if (fp <= p1 * delta) {
#if BUG
         printf("lmpar/ terminate (fp<p1*delta)\n");
#endif
         par = 0;
         return;
      }

      // lmpar: if the jacobian is not rank deficient, the newton
      //    step provides a lower bound, parl, for the 0. of
      //    the function. otherwise set this bound to 0..

      parl = 0;
      if (nsing >= n) {
         for (j = 0; j < n; j++) wa1[j] = diag[ipvt[j]] * wa2[ipvt[j]] / dxnorm;

         for (j = 0; j < n; j++) {
            sum = 0.0;
            for (i = 0; i < j; i++) sum += r[j * ldr + i] * wa1[i];
            wa1[j] = (wa1[j] - sum) / r[j + ldr * j];
         }
         temp = lm_enorm(n, 0, wa1);
         parl = fp / delta / temp / temp;
      }

      // lmpar: calculate an upper bound, paru, for the 0. of the function.

      for (j = 0; j < n; j++) {
         sum = 0;
         for (i = 0; i <= j; i++) sum += r[j * ldr + i] * qtb[i];
         wa1[j] = sum / diag[ipvt[j]];
      }
      gnorm = lm_enorm(n, 0, wa1);
      paru = gnorm / delta;
      if (paru == 0.0) paru = LM_DWARF / std::min(delta, p1);

      // lmpar: if the input par lies outside of the interval (parl,paru),
      //     set par to the closer endpoint.

      par = std::max(par, parl);
      par = std::min(par, paru);
      if (par == 0.0) par = gnorm / dxnorm;
#if BUG
      printf("lmpar/ parl %.4e  par %.4e  paru %.4e\n", parl, par, paru);
#endif

      // lmpar: iterate.

      for (;; iter++) {
         // evaluate the function at the current value of par.

         if (par == 0.0) par = std::max(LM_DWARF, p001 * paru);
         temp = std::sqrt(par);
         for (j = 0; j < n; j++) wa1[j] = temp * diag[j];
         lm_qrsolv(n, r, ldr, ipvt, wa1, qtb, x, sdiag, wa2);
         for (j = 0; j < n; j++) wa2[j] = diag[j] * x[j];
         dxnorm = lm_enorm(n, 0, wa2);
         fp_old = fp;
         fp = dxnorm - delta;

         // if the function is small enough, accept the current value
         //    of par. also test for the exceptional cases where parl
         //   is 0. or the number of iterations has reached 10.

         if (std::abs(fp) <= p1 * delta || (parl == 0.0 && fp <= fp_old && fp_old < 0.0) || iter == 10)
            break; /* the only exit from the iteration. */

         // compute the Newton correction.

         for (j = 0; j < n; j++) wa1[j] = diag[ipvt[j]] * wa2[ipvt[j]] / dxnorm;

         for (j = 0; j < n; j++) {
            wa1[j] = wa1[j] / sdiag[j];
            for (i = j + 1; i < n; i++) wa1[i] -= r[j * ldr + i] * wa1[j];
         }
         temp = lm_enorm(n, 0, wa1);
         parc = fp / delta / temp / temp;

         // depending on the sign of the function, update parl or paru.

         if (fp > 0)
            parl = std::max(parl, par);
         else if (fp < 0)
            paru = std::min(paru, par);
         // the case fp==0 is precluded by the break condition

         // compute an improved estimate for par.

         par = std::max(parl, par + parc);
      }

   }  // lm_lmpar.

   void lm_qrfac(int m, int n, double* a, bool pivot, int* ipvt, double* rdiag, double* acnorm, double* wa) {
      /*
       *     this subroutine uses householder transformations with column
       *     pivoting (optional) to compute a qr factorization of the
       *     m by n matrix a. that is, qrfac determines an orthogonal
       *     matrix q, a permutation matrix p, and an upper trapezoidal
       *     matrix r with diagonal elements of nonincreasing magnitude,
       *     such that a*p = q*r. the householder transformation for
       *     column k, k = 1,2,...,min(m,n), is of the form
       *
       *			    t
       *	    i - (1/u(k))*u*u
       *
       *     where u has 0.s in the first k-1 positions. the form of
       *     this transformation and the method of pivoting first
       *     appeared in the corresponding linpack subroutine.
       *
       *     parameters:
       *
       *	m is a positive integer input variable set to the number
       *	  of rows of a.
       *
       *	n is a positive integer input variable set to the number
       *	  of columns of a.
       *
       *	a is an m by n array. on input a contains the matrix for
       *	  which the qr factorization is to be computed. on output
       *	  the strict upper trapezoidal part of a contains the strict
       *	  upper trapezoidal part of r, and the lower trapezoidal
       *	  part of a contains a factored form of q (the non-trivial
       *	  elements of the u vectors described above).
       *
       *	pivot is a logical input variable. if pivot is set true,
       *	  then column pivoting is enforced. if pivot is set false,
       *	  then no column pivoting is done.
       *
       *	ipvt is an integer output array of length lipvt. ipvt
       *	  defines the permutation matrix p such that a*p = q*r.
       *	  column j of p is column ipvt(j) of the identity matrix.
       *	  if pivot is false, ipvt is not referenced.
       *
       *	rdiag is an output array of length n which contains the
       *	  diagonal elements of r.
       *
       *	acnorm is an output array of length n which contains the
       *	  norms of the corresponding columns of the input matrix a.
       *	  if this information is not needed, then acnorm can coincide
       *	  with rdiag.
       *
       *	wa is a work array of length n. if pivot is false, then wa
       *	  can coincide with rdiag.
       *
       */
      int i, j, k, kmax, MINmn;
      double ajnorm, sum, temp;
      const double p05 = 0.05;

      /*** qrfac: compute initial column norms and initialize several arrays. ***/

      for (j = 0; j < n; j++) {
         acnorm[j] = lm_enorm(m, j * m, a);
         rdiag[j] = acnorm[j];
         wa[j] = rdiag[j];
         if (pivot) ipvt[j] = j;
      }
#if BUG
      printf("qrfac\n");
#endif

      /*** qrfac: reduce a to r with householder transformations. ***/

      MINmn = std::min(m, n);
      for (j = 0; j < MINmn; j++) {
         if (!pivot) goto pivot_ok;

         /** bring the column of largest norm into the pivot position. **/

         kmax = j;
         for (k = j + 1; k < n; k++)
            if (rdiag[k] > rdiag[kmax]) kmax = k;
         if (kmax == j) goto pivot_ok;

         for (i = 0; i < m; i++) {
            temp = a[j * m + i];
            a[j * m + i] = a[kmax * m + i];
            a[kmax * m + i] = temp;
         }
         rdiag[kmax] = rdiag[j];
         wa[kmax] = wa[j];
         k = ipvt[j];
         ipvt[j] = ipvt[kmax];
         ipvt[kmax] = k;

      pivot_ok:
         /** compute the Householder transformation to reduce the
         j-th column of a to a multiple of the j-th unit vector. **/

         ajnorm = lm_enorm(m - j, j * m + j, a);
         if (ajnorm == 0.0) {
            rdiag[j] = 0;
            continue;
         }

         if (a[j * m + j] < 0.0) ajnorm = -ajnorm;
         for (i = j; i < m; i++) a[j * m + i] /= ajnorm;
         a[j * m + j] += 1;

         /** apply the transformation to the remaining columns
         and update the norms. **/

         for (k = j + 1; k < n; k++) {
            sum = 0;

            for (i = j; i < m; i++) sum += a[j * m + i] * a[k * m + i];

            temp = sum / a[j + m * j];

            for (i = j; i < m; i++) a[k * m + i] -= temp * a[j * m + i];

            if (pivot && rdiag[k] != 0.0) {
               temp = a[m * k + j] / rdiag[k];
               temp = std::max(0.0, 1 - temp * temp);
               rdiag[k] *= std::sqrt(temp);
               temp = rdiag[k] / wa[k];
               if (p05 * Square(temp) <= LM_MACHEP) {
                  rdiag[k] = lm_enorm(m - j - 1, m * k + j + 1, a);
                  wa[k] = rdiag[k];
               }
            }
         }

         rdiag[j] = -ajnorm;
      }
   }

   /// <summary>
   /// given an m by n matrix a, an n by n diagonal matrix d,
   /// and an m-vector b, the problem is to determine an x which
   /// solves the system
   ///
   ///  a*x = b ,	  d*x = 0 ,
   ///
   /// in the least squares sense.
   ///
   /// this subroutine completes the solution of the problem
   /// if it is provided with the necessary information from the
   /// qr factorization, with column pivoting, of a. that is, if
   /// a*p = q*r, where p is a permutation matrix, q has orthogonal
   /// columns, and r is an upper triangular matrix with diagonal
   /// elements of nonincreasing magnitude, then qrsolv expects
   /// the full upper triangle of r, the permutation matrix p,
   /// and the first n components of (q transpose)*b. the system
   /// a*x = b, d*x = 0, is then equivalent to
   ///
   ///  t	   t
   /// r*z = q *b ,  p *d*p*z = 0 ,
   ///
   /// where x = p*z. if this system does not have full rank,
   /// then a least squares solution is obtained. on output qrsolv
   /// also provides an upper triangular matrix s such that
   ///
   ///  t	 t		 t
   /// p *(a *a + d*d)*p = s *s .
   ///
   /// s is computed within qrsolv and may be of separate interest.
   /// </summary>
   /// <param name="n">n is a positive integer input variable set to the order of r.</param>
   /// <param name="r">r is an n by n array. on input the full upper triangle
   /// must contain the full upper triangle of the matrix r.
   /// on output the full upper triangle is unaltered, and the
   /// strict lower triangle contains the strict upper triangle
   /// (transposed) of the upper triangular matrix s.</param>
   /// <param name="ldr">ldr is a positive integer input variable not less than n
   /// which specifies the leading dimension of the array r.</param>
   /// <param name="ipvt">ipvt is an integer input array of length n which defines the
   ///	  permutation matrix p such that a*p = q*r. column j of p
   ///	  is column ipvt(j) of the identity matrix.</param>
   /// <param name="diag">diag is an input array of length n which must contain the
   /// diagonal elements of the matrix d.</param>
   /// <param name="qtb"> qtb is an input array of length n which must contain the first
   /// n elements of the vector (q transpose)*b.</param>
   /// <param name="x">x is an output array of length n which contains the least
   /// squares solution of the system a*x = b, d*x = 0.</param>
   /// <param name="sdiag">sdiag is an output array of length n which contains the
   /// diagonal elements of the upper triangular matrix s.</param>
   /// <param name="wa">wa is a work array of length n.</param>
   void lm_qrsolv(int n, double* r, int ldr, int* ipvt, double* diag, double* qtb, double* x, double* sdiag,
                  double* wa) {
      int i, kk, j, k, nsing;
      double qtbpj, sum, temp;
      double _sin, _cos, _tan, _cot; /* local variables, not functions */
      const double p25 = 0.25;
      const double p5 = 0.5;

      /*** qrsolv: copy r and (q transpose)*b to preserve input and initialize s.
      in particular, save the diagonal elements of r in x. ***/

      for (j = 0; j < n; j++) {
         for (i = j; i < n; i++) r[j * ldr + i] = r[i * ldr + j];
         x[j] = r[j * ldr + j];
         wa[j] = qtb[j];
      }
#if BUG
      printf("qrsolv\n");
#endif

      /*** qrsolv: eliminate the diagonal matrix d using a givens rotation. ***/

      for (j = 0; j < n; j++) {
         /*** qrsolv: prepare the row of d to be eliminated, locating the
         diagonal element using p from the qr factorization. ***/

         if (diag[ipvt[j]] == 0.0) goto L90;
         for (k = j; k < n; k++) sdiag[k] = 0.0;
         sdiag[j] = diag[ipvt[j]];

         /*** qrsolv: the transformations to eliminate the row of d modify only
         a single element of (q transpose)*b beyond the first n, which is
         initially 0.. ***/

         qtbpj = 0.0;
         for (k = j; k < n; k++) {
            /** determine a givens rotation which eliminates the
            appropriate element in the current row of d. **/

            if (sdiag[k] == 0.0) continue;
            kk = k + ldr * k;
            if (std::abs(r[kk]) < std::abs(sdiag[k])) {
               _cot = r[kk] / sdiag[k];
               _sin = p5 / std::sqrt(p25 + p25 * Square(_cot));
               _cos = _sin * _cot;
            } else {
               _tan = sdiag[k] / r[kk];
               _cos = p5 / std::sqrt(p25 + p25 * Square(_tan));
               _sin = _cos * _tan;
            }

            /** compute the modified diagonal element of r and
            the modified element of ((q transpose)*b,0). **/

            r[kk] = _cos * r[kk] + _sin * sdiag[k];
            temp = _cos * wa[k] + _sin * qtbpj;
            qtbpj = -_sin * wa[k] + _cos * qtbpj;
            wa[k] = temp;

            /** accumulate the tranformation in the row of s. **/

            for (i = k + 1; i < n; i++) {
               temp = _cos * r[k * ldr + i] + _sin * sdiag[i];
               sdiag[i] = -_sin * r[k * ldr + i] + _cos * sdiag[i];
               r[k * ldr + i] = temp;
            }
         }

      L90:
         /** store the diagonal element of s and restore
         the corresponding diagonal element of r. **/

         sdiag[j] = r[j * ldr + j];
         r[j * ldr + j] = x[j];
      }

      /*** qrsolv: solve the triangular system for z. if the system is
      singular, then obtain a least squares solution. ***/

      nsing = n;
      for (j = 0; j < n; j++) {
         if (sdiag[j] == 0.0 && nsing == n) nsing = j;
         if (nsing < n) wa[j] = 0;
      }

      for (j = nsing - 1; j >= 0; j--) {
         sum = 0;
         for (i = j + 1; i < nsing; i++) sum += r[j * ldr + i] * wa[i];
         wa[j] = (wa[j] - sum) / sdiag[j];
      }

      /*** qrsolv: permute the components of z back to components of x. ***/

      for (j = 0; j < n; j++) x[ipvt[j]] = wa[j];

   } /*** lm_qrsolv. ***/

   /// <summary>
   /// given an n-vector x, this function calculates the euclidean norm of x.
   ///
   ///      the euclidean norm is computed by accumulating the sum of
   ///      squares in three different sums. the sums of squares for the
   ///      small and large components are scaled so that no overflows
   ///      occur. non-destructive underflows are permitted. underflows
   ///      and overflows do not occur in the computation of the unscaled
   ///      sum of squares for the intermediate components.
   ///      the definitions of small, intermediate and large components
   ///      depend on two constants, LM_SQRT_DWARF and LM_SQRT_GIANT. the main
   ///      restrictions on these constants are that LM_SQRT_DWARF**2 not
   ///      underflow and LM_SQRT_GIANT**2 not overflow.
   /// </summary>
   /// <param name="Length">Length is a positive integer input variable.</param>
   /// <param name="Offset">The offset into array x.</param>
   /// <param name="x">x is an input array of length n.</param>
   /// <returns></returns>
   double lm_enorm(int Length, int Offset, double[] x) {
      int i;
      double agiant, s1, s2, s3, xabs, x1max, x3max, temp;

      s1 = 0;
      s2 = 0;
      s3 = 0;
      x1max = 0;
      x3max = 0;
      agiant = LM_SQRT_GIANT / ((double)Length);

      /** sum squares. **/
      for (i = Offset; i < Offset + Length; i++) {
         xabs = std::abs(x[i]);
         if (xabs > LM_SQRT_DWARF && xabs < agiant) {
            /*  sum for intermediate components. */
            s2 += xabs * xabs;
            continue;
         }

         if (xabs > LM_SQRT_DWARF) {
            /*  sum for large components. */
            if (xabs > x1max) {
               temp = x1max / xabs;
               s1 = 1 + s1 * Square(temp);
               x1max = xabs;
            } else {
               temp = xabs / x1max;
               s1 += Square(temp);
            }
            continue;
         }
         /*  sum for small components. */
         if (xabs > x3max) {
            temp = x3max / xabs;
            s3 = 1 + s3 * Square(temp);
            x3max = xabs;
         } else {
            if (xabs != 0.0) {
               temp = xabs / x3max;
               s3 += Square(temp);
            }
         }
      }

      /** calculation of norm. **/

      if (s1 != 0) return x1max * std::sqrt(s1 + (s2 / x1max) / x1max);
      if (s2 != 0) {
         if (s2 >= x3max)
            return std::sqrt(s2 * (1 + (x3max / s2) * (x3max * s3)));
         else
            return std::sqrt(x3max * ((s2 / x3max) + (x3max * s3)));
      }

      return x3max * std::sqrt(s3);

   } /*** lm_enorm. ***/
};
}  // namespace flint
}  // namespace moja
#endif  // LMCONTROL_INCLUDED