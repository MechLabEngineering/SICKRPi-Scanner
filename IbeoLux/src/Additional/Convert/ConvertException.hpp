/**
 * @file ConvertException.hpp
 *
 * Enthält die Exceptionklassen für die verschiedenen Konvertierungen.
 *
 * @author Christian Blumberg
 * @date 2012-06-04
 */

#ifndef CONVERTEXCEPTION_H_
#define CONVERTEXCEPTION_H_

#include <exception>

namespace convert
{
	/** @brief allgemeine Exception.
	 *
	 * Die allgemeinste Exception, die bei einer Konvertierung auftreten kann. Von ihr erben alle KindExceptions.
	 */
	class ConvertException : public std::exception {};

	/** @brief Wertebereich überschritten bei Konvertierung.
	 *
	 * Die Exception wird dann geworfen, wenn eine Umwandlung nicht stattfinden kann, weil der Wertebereich des angegebenen Datentyps in den konvertiert werden soll nicht ausreicht.
	 */
	class ConvertOutOfRangeException : public ConvertException {};

	/** @brief übergebener Parameter fehlerhaft.
	 *
	 * Die Exception wird dann geworfen, wenn ein übergebener Parameter eine falsche Syntax aufweist. Z.B. ein leerer String.
	 */
	class ConvertInvalidArgumentException : public ConvertException {};

} /* namespace convert */
#endif /* CONVERTEXCEPTION_H_ */
