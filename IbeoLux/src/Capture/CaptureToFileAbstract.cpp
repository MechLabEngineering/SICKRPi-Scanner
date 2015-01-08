/**
 * @file CaptureToFileAbstract.cpp
 *
 * Beherbergt die implementierte abstrakte Funktionalität für das Loggen von Scandaten in eine Datei.
 *
 * @author Christian Blumberg
 * @date 2012-06-08
 * @version 0.0.1
 * erstes Release
 */

#include "../Additional/Defines/Defines.hpp"

#include "CaptureToFileAbstract.hpp"

#include <fstream>

#include <boost/thread/mutex.hpp>

#include "../Additional/Convert/Convert.hpp"

namespace capture
{

	CaptureToFileAbstract::CaptureToFileAbstract(const std::string& filename, const std::string& separator) :
		ProcessAndCaptureAbstract(separator)//, capture(*this)
	{
//		fp = fopen(filename.c_str(), "w");
		file.open(filename.c_str(), std::ios::out);
	}

	CaptureToFileAbstract::~CaptureToFileAbstract()
	{
//		fclose(fp);
		file.close();
//		delete[] captureBuffer;
	}

//	std::ostream& CaptureToFileAbstract::operator<<(std::ostream& value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(const std::string& value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(const char* value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(const unsigned char* value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(char& value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(unsigned char& value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(int& value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(unsigned int& value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(short& value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(unsigned short& value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(long& value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(unsigned long& value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(int_64_t& value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(u_int_64_t& value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(double& value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}
//
//	std::ostream& CaptureToFileAbstract::operator<<(float& value)
//	{
//		boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
//		file << value;
//
//		return(file);
//	}

} /* namespace capture */
