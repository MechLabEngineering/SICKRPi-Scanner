/**
 * @file CaptureToScreenAbstract.hpp
 *
 * Beherbergt die abstrakte Funktionalität für das Loggen von Daten die auf dem Bildschirm ausgegeben werden sollen.
 *
 * @author Christian Blumberg
 * @date 2012-06-08
 * @version 0.0.1
 * erstes Release
 */

#ifndef CAPTURETOSCREENABSTRACT_HPP_
#define CAPTURETOSCREENABSTRACT_HPP_

#include "ProcessAndCaptureAbstract.hpp"
#include "../Additional/Defines/Defines.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <boost/thread/locks.hpp>

namespace capture
{

class ProcessAndCaptureAbstract;

/** @brief abstrakte Logik für das Loggen und Ausgeben von Daten auf dem Bildschirm.
 *
 * Die Klasse stellt die abstrakte Logik für das Loggen von Daten und dem Ausgeben dieser auf dem Bildschirm bereit.
 */
class CaptureToScreenAbstract : public ProcessAndCaptureAbstract
{
public:
	virtual ~CaptureToScreenAbstract();
	void flush();

	virtual std::ostream& operator<<(std::ostream& value);
	virtual std::ostream& operator<<(const std::string& value);
	virtual std::ostream& operator<<(const char* value);
	virtual std::ostream& operator<<(const unsigned char* value);
	virtual std::ostream& operator<<(char& value);
	virtual std::ostream& operator<<(unsigned char& value);
	virtual std::ostream& operator<<(int& value);
	virtual std::ostream& operator<<(unsigned int& value);
	virtual std::ostream& operator<<(short& value);
	virtual std::ostream& operator<<(unsigned short& value);
	virtual std::ostream& operator<<(long& value);
	virtual std::ostream& operator<<(unsigned long& value);
	virtual std::ostream& operator<<(int_64_t& value);
	virtual std::ostream& operator<<(u_int_64_t& value);
	virtual std::ostream& operator<<(double& value);
	virtual std::ostream& operator<<(float& value);

protected:
	CaptureToScreenAbstract(const std::string& separator, const unsigned int bufferSize = 100000);
	void checkForFlushStream();
	const unsigned int bufferSize;
	std::stringstream stream;
};

} /* namespace capture */
#endif /* CAPTURETOSCREENABSTRACT_HPP_ */
