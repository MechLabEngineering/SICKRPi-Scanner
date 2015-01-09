/**
 * @file CaptureToScreenAbstract.hpp
 *
 * Beherbergt die implementierte abstrakte Funktionalität für das Loggen von Daten die auf dem Bildschirm ausgegeben werden sollen.
 *
 * @author Christian Blumberg
 * @date 2012-06-08
 * @version 0.0.1
 * erstes Release
 */

#include "../Additional/Defines/Defines.hpp"

#include "CaptureToScreenAbstract.hpp"

#include <sstream>
#include <iostream>

namespace capture
{

CaptureToScreenAbstract::CaptureToScreenAbstract(const std::string& separator, const unsigned int bufferSize) :
		ProcessAndCaptureAbstract(separator), bufferSize(bufferSize)
{


}

CaptureToScreenAbstract::~CaptureToScreenAbstract()
{
	this->flush();
}

void CaptureToScreenAbstract::flush()
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	std::cout << stream.str();
	stream.str("");
}

void CaptureToScreenAbstract::checkForFlushStream()
{
	if(stream.tellp() >= bufferSize)
	{
		std::cout << stream.str();
		stream.str("");
	}
}

std::ostream& CaptureToScreenAbstract::operator<<(std::ostream& value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(const std::string& value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(const char* value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(const unsigned char* value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(char& value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(unsigned char& value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(int& value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(unsigned int& value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(short& value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(unsigned short& value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(long& value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(unsigned long& value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(int_64_t& value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(u_int_64_t& value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(double& value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

std::ostream& CaptureToScreenAbstract::operator<<(float& value)
{
	boost::lock_guard<boost::mutex> lock(shiftOperatorMutex);
	stream << value;
	checkForFlushStream();
	return(stream);
}

} /* namespace capture */
