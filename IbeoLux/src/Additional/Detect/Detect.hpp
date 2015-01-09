/**
 * @file Detect.hpp
 *
 * Enthält die Klasse Detect im Namensraum detect, die alle notwendigen Funktionalitäten für das Identifizieren von bestimmten Sachverhalten bereitstellt.
 *
 * @author Christian Blumberg
 * @date 2012-06-04
 */

#ifndef DETECT_HPP_
#define DETECT_HPP_

namespace detect
{

/** @brief überprüfen von bestimmten Sachverhalten.
 *
 * Die Klasse Detect stellt Funktionalitäten bereit, die für das Identifizieren von bestimmten Sachverhalten dienen.
 */
class Detect
{
private:
	Detect();
	virtual ~Detect();


public:
	/**
	 * Gibt als Rückgabewert an, ob das System, auf dem die Bibliothek zur Laufzeit eingesetzt wird, ein bigEndian-System ist oder nicht.
	 *
	 *
	 */
	static inline bool isThisBigEndian()
	{
		// test for endianess
		union
		{
	        long int l;
	        char c[sizeof (long int)];
	    } u;

	    u.l = 1;

	    if (u.c[sizeof(long int)-1] == 1)
	    {
	    	return true;
	    }
	    else
	        return false;
	}
};

} /* namespace detect */
#endif /* DETECT_HPP_ */
