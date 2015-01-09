/**
 * @file Defines.hpp
 *
 * Enthält die Anweisungen, die für das Kompilieren von Bedeutung sind.
 *
 * @author Christian Blumberg
 * @date 2012-06-04
 */

#ifndef DEFINES_HPP_
#define DEFINES_HPP_

#include <iostream>

#ifdef DEBUG
	#define dbgcout(str) std::cerr << str
#else
	#define dbgcout(str) (void)0
#endif

#include <boost/cstdint.hpp>
#include <climits>

//#define BOOST_THREAD_USE_LIB		///< Die Thread Library von Boost wird hiermit statisch gelinkt

typedef boost::uint64_t u_int_64_t;	///< ein 64-Bit-Unsigned Integer der exakt diese Länge aufweist
typedef boost::int64_t int_64_t;	///< ein 64-Bit Signed Integer der exakt diese Länge aufweist
typedef boost::uint32_t u_int_32_t;	///< ein 32-Bit-Unsigned Integer der exakt diese Länge aufweist
typedef boost::int32_t int_32_t;	///< ein 32-Bit Signed Integer der exakt diese Länge aufweist
typedef boost::uint16_t u_int_16_t;	///< ein 16-Bit-Unsigned Integer der exakt diese Länge aufweist
typedef boost::int16_t int_16_t;	///< ein 16-Bit Signed Integer der exakt diese Länge aufweist

#define ALL_MESSAGES USHRT_MAX

#if defined(_MSC_VER) || defined(__MSVCRT__)
	#define _WIN32_WINNT 0x0501			///< unterdrückt eine Meldung, die beim Kompilieren unter Windows auftritt

	#define PRI64d  "I64d"
	#define PRI64u  "I64u"
	#define PRI64x  "I64x"
#else
	#define PRI64d  "lld"
	#define PRI64u  "llu"
	#define PRI64x  "llx"
#endif

#endif /* DEFINES_HPP_ */
