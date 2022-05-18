///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


// System Includes
#include <algorithm>
// External Includes
// Internal Includes
// Application Includes
#include "num2string.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "num2string"

// NOTE: Based on the maximum length of a IEEE double number is 24 + 1 for \0,
//       we should never get a buffer overflow.
//       Reference: https://en.wikipedia.org/wiki/IEEE_754-1985
const size_t MAX_BUF_LEN = 25;

// Namespaces


string itoa10(int64_t const p_value, u_int64_t const p_pad_length) noexcept(true)
{
  string    result(p_pad_length, ' ');
  int       sign;
  char      temp_str[MAX_BUF_LEN];
  char *    wstr  = temp_str;
  int64_t   value = p_value;

  // Take care of sign
  if (0 > (sign = value))
  {
    value = -value;
  }

  do // Conversion. Number is reversed.
  {
    *wstr++ = (48 + (value % 10));
  }
  while( value /= 10);

  if (0 > sign)
  {
    *wstr++ = '-';
  }
  *wstr  = '\0';

  result = temp_str;
  if (p_pad_length > result.length()) 
  {
    result.append(p_pad_length - result.length(), '0');
  }
  reverse(result.begin(), result.end());
  return(result);
}


string uitoa16(u_int32_t const p_value, u_int32_t const p_pad_length) noexcept(true)
{
  u_int8_t hexdigits[] = "0123456789ABCDEF";

  if (2 == p_pad_length)
  {
    static string   result("00");

    result.at(0)  = hexdigits[(p_value >>  4) & 0x0F];
    result.at(1)  = hexdigits[ p_value        & 0x0F];

    return(result);
  }

  if (4 == p_pad_length)
  {
    static string   result("0000");

    result.at(0)  = hexdigits[ p_value >> 12];         
    result.at(1)  = hexdigits[(p_value >>  8) & 0x0F];
    result.at(2)  = hexdigits[(p_value >>  4) & 0x0F];
    result.at(3)  = hexdigits[ p_value        & 0x0F];

    return(result);
  }

  if (8 == p_pad_length)
  {
    static string   result("00000000");

    result.at(0)  = hexdigits[ p_value >> 28];
    result.at(1)  = hexdigits[(p_value >> 24) & 0x0F];
    result.at(2)  = hexdigits[(p_value >> 20) & 0x0F];
    result.at(3)  = hexdigits[(p_value >> 16) & 0x0F];
    result.at(4)  = hexdigits[(p_value >> 12) & 0x0F];
    result.at(5)  = hexdigits[(p_value >>  8) & 0x0F];
    result.at(6)  = hexdigits[(p_value >>  4) & 0x0F];
    result.at(7)  = hexdigits[ p_value        & 0x0F];

    return(result);
  }

  return("*******WRONG PADDING SUPPLIED TO uitoa16*******");
}


string uitoa10(u_int64_t const p_value, u_int64_t const p_pad_length) noexcept(true)
{
  string    result(p_pad_length, ' ');
  char      temp_str[MAX_BUF_LEN] = "";
  char *    wstr  = temp_str;
  u_int64_t value = p_value;

  do // Conversion. Number is reversed.
  {
    *wstr++ = 48 + (value % 10);
  }
  while (value /= 10);
  *wstr  = '\0';

  result = temp_str;
  if (p_pad_length > result.length()) 
  {
    result.append(p_pad_length - result.length(), '0');
  }
  reverse(result.begin(), result.end());
  return(result);
}


string dtoa(double const p_value, u_int64_t const p_prec) noexcept(true)
{
  // if input is larger than thres_max, revert to exponential
  double const thres_max = (double)(0x7FFFFFFF);
  double       diff  = 0.0;
  string       result(p_prec, ' ');
  char         temp_str[MAX_BUF_LEN];
  char *       wstr  = temp_str;
  u_int64_t    prec  = p_prec;
  double       value = p_value;

  if (9 < prec)
  {
    // precision of >= 10 can lead to overflow errors
    prec = 9;
  }

  // we'll work in positive values and deal with the negative sign issue later
  int neg = 0;
  if (0 > value)
  {
    neg     = 1;
    value = -value;
  }

  u_int64_t whole = (u_int64_t) value;
  double    tmp   = (value - whole) * ::g_pow10[prec];
  u_int64_t frac  = (u_int64_t)(tmp);
  diff            = tmp - frac;

  if (0.5 < diff)
  {
    ++frac;
    // handle rollover, e.g.  case 0.99 with prec 1 is 1.0
    if (::g_pow10[prec] <= frac)
    {
      frac = 0;
      ++whole;
    }
  }
  else if ((0.5 == diff) && ((0 == frac) || (1 & frac)))
  {
    // if halfway, round up if odd, OR
    // if last digit is 0.  That last part is strange
    ++frac;
  }

  // for very large numbers switch back to native sprintf for exponentials.
  // normal printf behavior is to print EVERY whole number digit
  // which can be 100s of characters overflowing your buffers == bad
  //
  if (thres_max < value)
  {
    if (0 < snprintf(temp_str, sizeof(temp_str), "%e", neg ? -value : value))
    {
      result = temp_str;
      return(result);
    }
    return("NaN");
  }

  if (0 == prec)
  {
    diff = value - whole;
    if (0.5 < diff)
    {
      // greater than 0.5, round up, e.g. 1.6 -> 2
      ++whole;
    }
    else if ((0.5 == diff) && (whole & 1))
    {
      // exactly 0.5 and ODD, then round up
      // 1.5 -> 2, but 2.5 -> 2
      ++whole;
    }
  }
  else
  {
    // now do fractional part, as an unsigned number
    do
    {
      --prec;
      *wstr++ = 48 + (frac % 10);
    }
    while (frac /= 10);
    // add extra 0s
    while (0 < prec--)
    {
      *wstr++ = '0';
    }
    // add decimal
    *wstr++ = '.';
  }

  // do whole part, Take care of sign Conversion. Number is reversed.
  do
  {
    *wstr++ = 48 + (whole % 10);
  }
  while (whole /= 10);
  if (neg)
  {
    *wstr++ = '-';
  }
  *wstr  = '\0';

  result = temp_str;
  reverse(result.begin(), result.end());
  return(result);
}


string ftoa(float const p_value, u_int64_t const p_prec) noexcept(true)
{
  // if input is larger than thres_max, revert to exponential
  float  const thres_max = (float)(0x7FFFFFFF);
  float        diff  = 0.0;
  string       result(p_prec, ' ');
  char         temp_str[MAX_BUF_LEN];
  char *       wstr  = temp_str;
  u_int64_t    prec  = p_prec;
  float        value = p_value;

  if (9 < prec)
  {
    // precision of >= 10 can lead to overflow errors
    prec = 9;
  }

  // we'll work in positive values and deal with the negative sign issue later
  int neg = 0;
  if (0 > value)
  {
    neg     = 1;
    value = -value;
  }

  u_int64_t whole = (u_int64_t) value;
  float     tmp   = (value - whole) * ::g_pow10[prec];
  u_int64_t frac  = (u_int64_t)(tmp);
  diff            = tmp - frac;

  if (0.5 < diff)
  {
    ++frac;
    // handle rollover, e.g.  case 0.99 with prec 1 is 1.0
    if (::g_pow10[prec] <= frac)
    {
      frac = 0;
      ++whole;
    }
  }
  else if ((0.5 == diff) && ((0 == frac) || (1 & frac)))
  {
    // if halfway, round up if odd, OR
    // if last digit is 0.  That last part is strange
    ++frac;
  }

  // for very large numbers switch back to native sprintf for exponentials.
  // normal printf behavior is to print EVERY whole number digit
  // which can be 100s of characters overflowing your buffers == bad
  //
  if (thres_max < value)
  {
    if (0 < snprintf(temp_str, sizeof(temp_str), "%e", neg ? -value : value))
    {
      result = temp_str;
      return(result);
    }
    return("NaN");
  }

  if (0 == prec)
  {
    diff = value - whole;
    if (0.5 < diff)
    {
      // greater than 0.5, round up, e.g. 1.6 -> 2
      ++whole;
    }
    else if ((0.5 == diff) && (whole & 1))
    {
      // exactly 0.5 and ODD, then round up
      // 1.5 -> 2, but 2.5 -> 2
      ++whole;
    }
  }
  else
  {
    // now do fractional part, as an unsigned number
    do
    {
      --prec;
      *wstr++ = 48 + (frac % 10);
    }
    while (frac /= 10);
    // add extra 0s
    while (0 < prec--)
    {
      *wstr++ = '0';
    }
    // add decimal
    *wstr++ = '.';
  }

  // do whole part, Take care of sign Conversion. Number is reversed.
  do
  {
    *wstr++ = 48 + (whole % 10);
  }
  while (whole /= 10);
  if (neg)
  {
    *wstr++ = '-';
  }
  *wstr  = '\0';

  result = temp_str;
  reverse(result.begin(), result.end());
  return(result);
}

