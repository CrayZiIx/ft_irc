#include "../inc/utils.h"

int to_int(char const *s)
{
     if ( s == NULL || *s == '\0' )
        throw (std::invalid_argument("null or empty string argument"));
     bool negate = (s[0] == '-');
     if ( *s == '+' || *s == '-' ) 
         ++s;
     if ( *s == '\0')
        throw std::invalid_argument("sign character only.");
     int result = 0;
     while(*s)
     {
          if ( *s < '0' || *s > '9' )
            throw std::invalid_argument("invalid input string");
          result = result * 10  - (*s - '0');  //assume negative number
          ++s;
     }
     return (negate ? result : -result); //-result is positive!
}