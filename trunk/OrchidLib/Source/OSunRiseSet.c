/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：日出日落时间计算
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OSunRiseSet.c,v 1.3 2006/06/02 08:40:41 zjy Exp $
* 备    注：
*******************************************************************************/

#include <math.h>
#include "OSunRiseSet.h"

#define rads 0.0174532925

static double frac(double a)
{
  return a - (int)a;
}

static void hrsmin(double hours, OTIME *time)
{
  int seconds = (int)(hours * 60 * 60);
  time->hour = (seconds / 60 / 60) % 24;
  time->minute = (seconds / 60) % 60;
  time->second = seconds % 60;
}

static void zerotime(OTIME *time)
{
  time->hour = 0;
  time->minute = 0;
  time->second = 0;
}

static double ipart(double x)
{
  if (x > 0)
    return floor(x);
  else
    return ceil(x);
}

static double range(double x)
{
  double a = x / 360;
  double ret = 360 * (a - ipart(a));
  if (ret < 0)
    ret += 360;
  return ret;
}

static double mjd(int year, int month, int day, double hour)
{
  double a, b;

  if (month <= 2)
  {
    month += 12;
    year--;
  }

  a = 10000.0 * year + 100.0 * month + day;
  if (a <= 15821004.1)
  {
    b = -2 * floor((year + 4716) / 4) - 1179;
  }
  else
  {
    b = floor(year / 400) - floor(year / 100) + floor(year / 4);
  }

  a = 365.0 * year - 679004.0;
  return (a + b + floor(30.6001 * (month + 1)) + day + hour / 24.0);
}

static void quad(double ym, double yz, double yp, double *anz, double *az1, 
          double *az2, double *axe, double *aye)
{
  double a, b, c, dis, dx;
  double nz, z1, z2, xe, ye;

  nz = 0;
  a = 0.5 * (ym + yp) - yz;
  b = 0.5 * (yp - ym);
  c = yz;
  xe = -b / (2 * a);
  ye = (a * xe + b) * xe + c;
  dis = b * b - 4.0 * a * c;
  if (dis > 0)
  {
    dx = 0.5 * sqrt(dis) / fabs(a);
    z1 = xe - dx;
    z2 = xe + dx;
    if (fabs(z1) <= 1.0)
      nz++;
    if (fabs(z2) <= 1.0)
      nz++;
    if (z1 < -1.0)
      z1 = z2;
  }

  *anz = nz;
  *az1 = z1;
  *az2 = z2;
  *axe = xe;
  *aye = ye;
}

static double lmst(double mjd, double glong)
{
  double lst, t, d;

  d = mjd - 51544.5;
  t = d / 36525.0;
  lst = range(280.46061837 + 360.98564736629 * d + 0.000387933 * t * t -
    t * t * t / 38710000);
  return (lst / 15.0 + glong / 15);
}

static void minisun(double t, double *dec, double *ra)
{
  const double p2 = 6.283185307;
  const double coseps = 0.91748;
  const double sineps = 0.39778;
  double L, M, DL, SL, X, Y, Z, RHO;

  M = p2 * frac(0.993133 + 99.997361 * t);
  DL = 6893.0 * sin(M) + 72.0 * sin(2 * M);
  L = p2 * frac(0.7859453 + M / p2 + (6191.2 * t + DL) / 1296000);
  SL = sin(L);
  X = cos(L);
  Y = coseps * SL;
  Z = sineps * SL;
  RHO = sqrt(1 - Z * Z);
  *dec = (360.0 / p2) * atan2(Z, RHO);
  *ra = (48.0 / p2) * atan2(Y, (X + RHO));
  if (*ra < 0)
    *ra += 24;
}

static double sin_alt(double mjd0, double hour, double glong, double cglat, double sglat)
{
  double mjd, t, ra, dec, tau, salt;
  mjd = mjd0 + hour / 24.0;
  t = (mjd - 51544.5) / 36525.0;
  minisun(t, &dec, &ra);
  tau = 15.0 * (lmst(mjd, glong) - ra);
  salt = sglat * sin(rads * dec) + cglat * cos(rads * dec) * cos(rads * tau);
  return salt;
}

static double getzttime(double mjd, double decho, double tz, double glong)
{
  double sinho, date, ym, yz, utrise, utset;
  double yp, ye, nz, hour, z1, z2, xe, ret;
  sinho = sin(rads * decho);
  date = mjd - tz / 24.0;
  hour = 1.0;
  ym = sin_alt(date, hour - 1.0, glong, 1, 0) - sinho;

  utrise = 0;
  utset = 0;
  while (hour < 25)
  {
    yz = sin_alt(date, hour, glong, 1, 0) - sinho;
    yp = sin_alt(date, hour + 1.0, glong, 1, 0) - sinho;
    quad(ym, yz, yp, &nz, &z1, &z2, &xe, &ye);

    if (nz == 1)
    {
      if (ym < 0.0)
        utrise = hour + z1;
      else
        utset = hour + z1;
    }

    if (nz == 2)
    {
      if (ye < 0.0)
      {
        utrise = hour + z2;
        utset = hour + z1;
      }
      else
      {
        utrise = hour + z1;
        utset = hour + z2;
      }
    }
    ym = yp;
    hour = hour + 2.0;
  }

  ret = (utrise + utset) / 2;
  if (ret < utrise)
    ret += 12;
  if (ret > 24)
    ret -= 24;
  return ret;
}

static SUNTYPE calc(double mjd, double decho, double glong, double glat, int tz, 
                    OTIME *risetime, OTIME *transittime, OTIME *settime)
{
  double sinho, sglat, cglat;
  double yz, yp, ym, nz, z1, z2, xe, ye;
  double utrise, utset, zt;
  double date, hour;
  int rise, sett, above;

  sinho = sin(rads * decho);
  sglat = sin(rads * glat);
  cglat = cos(rads * glat);
  date = mjd - tz / 24.0;

  rise = 0;
  sett = 0;
  above = 0;
  hour = 1.0;
  utrise = 0;
  utset = 0;
  ym = sin_alt(date, hour - 1.0, glong, cglat, sglat) - sinho;
  if (ym > 0.0)
    above = 1;

  while ((hour < 25) && (!sett || !rise))
  {
    yz = sin_alt(date, hour, glong, cglat, sglat) - sinho;
    yp = sin_alt(date, hour + 1.0, glong, cglat, sglat) - sinho;
    quad(ym, yz, yp, &nz, &z1, &z2, &xe, &ye);
    if (nz == 1)
    {
      if (ym < 0.0)
      {
        utrise = hour + z1;
        rise = 1;
      }
      else
      {
        utset = hour + z1;
        sett = 1;
      }
    }

    if (nz == 2)
    {
      if (ye < 0.0)
      {
        utrise = hour + z2;
        utset = hour + z1;
      }
      else
      {
        utrise = hour + z1;
        utset = hour + z2;
      }
    }

    ym = yp;
    hour = hour + 2.0;
  }

  if (risetime) zerotime(risetime);
  if (transittime) zerotime(transittime);
  if (settime) zerotime(settime);
  if (rise || sett)
  {
    if (rise && risetime)
      hrsmin(utrise, risetime);

    zt = getzttime(mjd, decho, tz, glong);
    if (transittime)
      hrsmin(zt, transittime);

    if (sett && settime)
      hrsmin(utset, settime);
      
    return stNormal;
  }
  else if (above)
  {
    zt = getzttime(mjd, decho, tz, glong);
    if (transittime)
      hrsmin(zt, transittime);
    
    return stAllwaysUp;
  }
  else
  {
    return stAllwaysDown;
  }
}

static int isvaliddate(int year, int month, int day)
{
  static const int MonthDays[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
  };
  int leap = (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)) ? 1 : 0;

  return ((year >= 1) && (year <= 9999) && (month >= 1) && (month <= 12) &&
      (day >= 1) && (day <= MonthDays[leap][month - 1]));
}

SUNTYPE GetSunRiseSetTime(int year, int month, int day, double longitude, 
                          double latitude, int zonetime, SUNTIME *suntime)
{
  double mj;
  if (isvaliddate(year, month, day))
  {
    mj = mjd(year, month, day, 0);
    // 计算天亮天黑时间
    calc(mj, -6, longitude, latitude, zonetime, &suntime->lighttime, NULL, &suntime->darktime);
    // 计算日出日落时间
    return calc(mj, -50.0 / 60.0, longitude, latitude, zonetime, &suntime->risetime, 
      &suntime->transittime, &suntime->settime);
  } else {
    return stError;
  }
}
