/**
 * @file Timestamp.hpp
 *
 * Enthält die Klasse Timestamp, die alle notwendigen Funktionalitäten für das Generieren eines Timestamps bereitstellt.
 *
 * @author Christian Blumberg
 * @date 2012-06-04
 */

#ifndef TIMESTAMP_HPP_
#define TIMESTAMP_HPP_

//#include <sys/time.h>
#include "../Defines/Defines.hpp"
#include <boost/chrono.hpp>



/** @brief generiert Timestamps im Millisekundenbereich.
 *
 * Diese Klasse stellt Funktionalitäten für das Generieren von Timestamps bereit.
 *
 * @author Christian Blumberg
 * @date 2012-06-04
 * @version 0.0.1
 * erstes Release
 */
class Timestamp
{
public:
	/**
	 * Statische Methode um einen Timestamp im Millisekundenbereich bereitzustellen.
	 * @return Ein Timestamp im Millisekundenbereich.
	 */
	static inline u_int_64_t getTimeStamp()
	{
		return(boost::chrono::high_resolution_clock::now().time_since_epoch().count());
	}


private:
	Timestamp() {};
	virtual ~Timestamp() {};
};


#endif /* TIMESTAMP_HPP_ */
