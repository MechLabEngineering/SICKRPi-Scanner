/**
 * @file ProcessAndCaptureAbstract.hpp
 *
 * Beherbergt die abstrakte Funktionalität für das Loggen von Daten die weitervererbt wird.
 *
 * @author Christian Blumberg
 * @date 2012-06-08
 * @version 0.0.1
 * erstes Release
 */

#ifndef PROCESSANDCAPTUREABSTRACT_HPP_
#define PROCESSANDCAPTUREABSTRACT_HPP_

#include "../Additional/Defines/Defines.hpp"

#include <boost/thread/mutex.hpp>

#include <sstream>
#include <string>

namespace ibeo{ class IbeoLaserDataAbstract; }

/** @brief Capturing und Ausgaben von Scandaten.
 *
 * Der Namespace capture kapselt die gesamte Funktionalität, die für das Capturing und die Ausgabe von Scandaten dient.
 */
namespace capture
{

/** @brief abstrakte Basisklasse für alle abgeleiteten Capturingklassen.
 *
 * Die allgemeinste Basisklasse für das Capturing dient für Ableitungen und ist abstrakt.
 */
class ProcessAndCaptureAbstract
{
public:
//	virtual std::ostream& operator<<(std::ostream& value) = 0;
//	virtual std::ostream& operator<<(const std::string& value) = 0;
//	virtual std::ostream& operator<<(const char* value) = 0;
//	virtual std::ostream& operator<<(const unsigned char* value) = 0;
//	virtual std::ostream& operator<<(char& value) = 0;
//	virtual std::ostream& operator<<(unsigned char& value) = 0;
//	virtual std::ostream& operator<<(int& value) = 0;
//	virtual std::ostream& operator<<(unsigned int& value) = 0;
//	virtual std::ostream& operator<<(short& value) = 0;
//	virtual std::ostream& operator<<(unsigned short& value) = 0;
//	virtual std::ostream& operator<<(long& value) = 0;
//	virtual std::ostream& operator<<(unsigned long& value) = 0;
//	virtual std::ostream& operator<<(int_64_t& value) = 0;
//	virtual std::ostream& operator<<(u_int_64_t& value) = 0;
//	virtual std::ostream& operator<<(double& value) = 0;
//	virtual std::ostream& operator<<(float& value) = 0;
	virtual void operator<<(ibeo::IbeoLaserDataAbstract& dataObj) = 0;

protected:
	ProcessAndCaptureAbstract(const std::string& separator) :
		separator(separator), charSeparator(separator.at(0))
	{

	}

	virtual ~ProcessAndCaptureAbstract(){};
	const std::string separator;
	const unsigned char charSeparator;
	std::stringstream stream;
	boost::mutex shiftOperatorMutex;

};

} /* namespace capture */
#endif /* PROCESSANDCAPTUREABSTRACT_HPP_ */
