/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include "rational.h"
#include "gnulib.h"
#include <math.h>
#include <values.h>
#include <float.h>
#include <assert.h>
#include <ctype.h>

void gRational::error(const char* msg) const
{
  gerr << "gRational class error: " << msg << '\n';
  assert(0);
}

static const gInteger _Int_One(1);

void gRational::normalize()
{
  int s = sign(den);
  if (s == 0)
    error("Zero denominator.");
  else if (s < 0)
  {
    den.negate();
    num.negate();
  }

  gInteger g = gcd(num, den);
  if (ucompare(g, _Int_One) != 0)
  {
    num /= g;
    den /= g;
  }
}

void      add(const gRational& x, const gRational& y, gRational& r)
{
  if (&r != &x && &r != &y)
  {
    mul(x.num, y.den, r.num);
    mul(x.den, y.num, r.den);
    add(r.num, r.den, r.num);
    mul(x.den, y.den, r.den);
  }
  else
  {
    gInteger tmp;
    mul(x.den, y.num, tmp);
    mul(x.num, y.den, r.num);
    add(r.num, tmp, r.num);
    mul(x.den, y.den, r.den);
  }
  r.normalize();
}

void      sub(const gRational& x, const gRational& y, gRational& r)
{
  if (&r != &x && &r != &y)
  {
    mul(x.num, y.den, r.num);
    mul(x.den, y.num, r.den);
    sub(r.num, r.den, r.num);
    mul(x.den, y.den, r.den);
  }
  else
  {
    gInteger tmp;
    mul(x.den, y.num, tmp);
    mul(x.num, y.den, r.num);
    sub(r.num, tmp, r.num);
    mul(x.den, y.den, r.den);
  }
  r.normalize();
}

void      mul(const gRational& x, const gRational& y, gRational& r)
{
  mul(x.num, y.num, r.num);
  mul(x.den, y.den, r.den);
  r.normalize();
}

void      div(const gRational& x, const gRational& y, gRational& r)
{
  if (&r != &x && &r != &y)
  {
    mul(x.num, y.den, r.num);
    mul(x.den, y.num, r.den);
  }
  else
  {
    gInteger tmp;
    mul(x.num, y.den, tmp);
    mul(y.num, x.den, r.den);
    r.num = tmp;
  }
  r.normalize();
}




void gRational::invert()
{
  gInteger tmp = num;  
  num = den;  
  den = tmp;  
  int s = sign(den);
  if (s == 0)
    error("Zero denominator.");
  else if (s < 0)
  {
    den.negate();
    num.negate();
  }
}

int compare(const gRational& x, const gRational& y)
{
  int xsgn = sign(x.num);
  int ysgn = sign(y.num);
  int d = xsgn - ysgn;
  if (d == 0 && xsgn != 0) d = compare(x.num * y.den, x.den * y.num);
  return d;
}

gRational::gRational(double x)
{
  num = 0;
  den = 1;
  if (x != 0.0)
  {
    int neg = x < 0;
    if (neg)
      x = -x;

    const long shift = 15;         // a safe shift per step
    const double width = 32768.0;  // = 2^shift
    const int maxiter = 20;        // ought not be necessary, but just in case,
                                   // max 300 bits of precision
    int expt;
    double mantissa = frexp(x, &expt);
    long exponent = expt;
    double intpart;
    int k = 0;
    while (mantissa != 0.0 && k++ < maxiter)
    {
      mantissa *= width;
      mantissa = modf(mantissa, &intpart);
      num <<= shift;
      num += (long)intpart;
      exponent -= shift;
    }
    if (exponent > 0)
      num <<= exponent;
    else if (exponent < 0)
      den <<= -exponent;
    if (neg)
      num.negate();
  }
  normalize();
}


gInteger trunc(const gRational& x)
{
  return x.num / x.den ;
}


gRational pow(const gRational& x, const gInteger& y)
{
  long yy = y.as_long();
  return pow(x, yy);
}               

#if defined(__GNUG__) && !defined(NO_NRV)

gRational operator - (const gRational& x) return r(x)
{
  r.negate();
}

gRational abs(const gRational& x) return r(x)
{
  if (sign(r.num) < 0) r.negate();
}


gRational sqr(const gRational& x) return r
{
  mul(x.num, x.num, r.num);
  mul(x.den, x.den, r.den);
  r.normalize();
}

gInteger floor(const gRational& x) return q
{
  gInteger r;
  divide(x.num, x.den, q, r);
  if (sign(x.num) < 0 && sign(r) != 0) --q;
}

gInteger ceil(const gRational& x) return q
{
  gInteger  r;
  divide(x.num, x.den, q, r);
  if (sign(x.num) >= 0 && sign(r) != 0) ++q;
}

gInteger round(const gRational& x) return q
{
  gInteger r;
  divide(x.num, x.den, q, r);
  r <<= 1;
  if (ucompare(r, x.den) >= 0)
  {
    if (sign(x.num) >= 0)
      ++q;
    else
      --q;
  }
}

// power: no need to normalize since num & den already relatively prime

gRational pow(const gRational& x, long y) return r
{
  if (y >= 0)
  {
    pow(x.num, y, r.num);
    pow(x.den, y, r.den);
  }
  else
  {
    y = -y;
    pow(x.num, y, r.den);
    pow(x.den, y, r.num);
    if (sign(r.den) < 0)
    {
      r.num.negate();
      r.den.negate();
    }
  }
}

#else

gRational operator - (const gRational& x) 
{
  gRational r(x); r.negate(); return r;
}

gRational abs(const gRational& x) 
{
  gRational r(x);
  if (sign(r.num) < 0) r.negate();
  return r;
}


gRational sqr(const gRational& x)
{
  gRational r;
  mul(x.num, x.num, r.num);
  mul(x.den, x.den, r.den);
  r.normalize();
  return r;
}

gInteger floor(const gRational& x)
{
  gInteger q;
  gInteger r;
  divide(x.num, x.den, q, r);
  if (sign(x.num) < 0 && sign(r) != 0) --q;
  return q;
}

gInteger ceil(const gRational& x)
{
  gInteger q;
  gInteger  r;
  divide(x.num, x.den, q, r);
  if (sign(x.num) >= 0 && sign(r) != 0) ++q;
  return q;
}

gInteger round(const gRational& x) 
{
  gInteger q;
  gInteger r;
  divide(x.num, x.den, q, r);
  r <<= 1;
  if (ucompare(r, x.den) >= 0)
  {
    if (sign(x.num) >= 0)
      ++q;
    else
      --q;
  }
  return q;
}

gRational pow(const gRational& x, long y)
{
  gRational r;
  if (y >= 0)
  {
    pow(x.num, y, r.num);
    pow(x.den, y, r.den);
  }
  else
  {
    y = -y;
    pow(x.num, y, r.den);
    pow(x.den, y, r.num);
    if (sign(r.den) < 0)
    {
      r.num.negate();
      r.den.negate();
    }
  }
  return r;
}

#endif

gOutput& operator << (gOutput& s, const gRational& y)
{
  if (y.denominator() == 1L)
    s << y.numerator();
  else
  {
    s << y.numerator();
    s << "/";
    s << y.denominator();
  }
  return s;
}

gInput &operator>>(gInput &f, gRational &y)
{
  char ch = ' ';
  int sign = 1;
  gInteger num = 0, denom = 1;

  while (isspace(ch))    f >> ch;
  
  if (ch == '-')  { 
    sign = -1;
    f >> ch;
  }
  
  while (ch >= '0' && ch <= '9')   {
    num *= 10;
    num += (int) (ch - '0');
    f >> ch;
  }
  
  if (ch == '/')  {
    denom = 0;
    f >> ch;
    while (ch >= '0' && ch <= '9')  {
      denom *= 10;
      denom += (int) (ch - '0');
      f >> ch;
    }
  }
  else if (ch == '.')  {
    denom = 1;
    f >> ch;
    while (ch >= '0' && ch <= '9')  {
      denom *= 10;
      num *= 10;
      num += (int) (ch - '0');
      f >> ch;
    }
  }

  f.unget(ch);

  y = gRational(sign * num, denom);
  y.normalize();

  return f;
}

int gRational::OK() const
{
  int v = num.OK() && den.OK(); // have valid num and denom
  if (v)
    {
      v &= sign(den) > 0;           // denominator positive;
      v &=  ucompare(gcd(num, den), _Int_One) == 0; // relatively prime
    }
  if (!v) error("invariant failure");
  return v;
}

int
gRational::fits_in_float() const
{
    return gRational (FLT_MIN) <= *this && *this <= gRational (FLT_MAX);
}

int
gRational::fits_in_double() const
{
    return gRational (DBL_MIN) <= *this && *this <= gRational (DBL_MAX);
}
