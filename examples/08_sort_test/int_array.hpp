//----------------------------------------------------------------------------
/// @file int_array.hpp
/// @brief
///
/// @author Copyright (c) 2010 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __INT_ARRAY_HPP
#define __INT_ARRAY_HPP

#include <cstdint>
#include <iostream>

template <uint32_t NN>
struct int_array
{   uint64_t M[NN];

    template <class generator >
    static int_array<NN> generate (generator &gen)
    {
      int_array<NN> result;
      for ( uint32_t i =0 ; i < NN ; ++i)
        result.M[i] = gen();
      return result;
    };

    uint64_t counter ( void) const
    {   uint64_t Acc =M[0] ;
        for ( uint32_t i =1 ; i < NN ; Acc += M[i++]) ;
        return Acc ;
    };
};

template <class IA >
struct H_comp
{	bool operator () ( const  IA &A1, const IA & A2)const
	{return (  A1.counter() < A2.counter());};
};

template <class IA >
struct L_comp
{
	bool operator () ( const  IA & A1, const IA & A2)const
	{return (  A1.M[0]< A2.M[0] );};
};

template <uint32_t NN>
std::ostream & operator << ( std::ostream & out, const int_array<NN> & IA)
{	out<<IA.counter() ;
	return out;
};
#endif // end of int_array.hpp
