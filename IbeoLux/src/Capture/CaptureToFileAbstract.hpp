/**
 * @file CaptureToFileAbstract.hpp
 *
 * Beherbergt die abstrakte Funktionalität für das Loggen von Scandaten in eine Datei.
 *
 * @author Christian Blumberg
 * @date 2012-06-08
 * @version 0.0.1
 * erstes Release
 */

#ifndef CAPTURETOFILEABSTRACT_HPP_
#define CAPTURETOFILEABSTRACT_HPP_

#include "../Additional/Defines/Defines.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdio>

#include "ProcessAndCaptureAbstract.hpp"
#include <boost/thread/mutex.hpp>


namespace capture
{
	class ProcessAndCaptureAbstract;

	/** @brief abstracte Basisklasse für in Dateien schreibende Capturingklassen.
	 *
	 * Die abstracte Basisklasse, von der alle Capturingklassen erben sollen, die Daten in Dateien schreiben.
	 */
	class CaptureToFileAbstract : public ProcessAndCaptureAbstract
	{
	public:
		virtual ~CaptureToFileAbstract();

//		virtual std::ostream& operator<<(std::ostream& value);
//		virtual std::ostream& operator<<(const std::string& value);
//		virtual std::ostream& operator<<(const char* value);
//		virtual std::ostream& operator<<(const unsigned char* value);
//		virtual std::ostream& operator<<(char& value);
//		virtual std::ostream& operator<<(unsigned char& value);
//		virtual std::ostream& operator<<(int& value);
//		virtual std::ostream& operator<<(unsigned int& value);
//		virtual std::ostream& operator<<(short& value);
//		virtual std::ostream& operator<<(unsigned short& value);
//		virtual std::ostream& operator<<(long& value);
//		virtual std::ostream& operator<<(unsigned long& value);
//		virtual std::ostream& operator<<(int_64_t& value);
//		virtual std::ostream& operator<<(u_int_64_t& value);
//		virtual std::ostream& operator<<(double& value);
//		virtual std::ostream& operator<<(float& value);

	protected:
		CaptureToFileAbstract(const std::string& filename, const std::string& separator);

		FILE *fp;
		std::fstream file;

//		CaptureToFileAbstract& capture;
	};

} /* namespace capture */
#endif /* CAPTURETOFILEABSTRACT_HPP_ */
