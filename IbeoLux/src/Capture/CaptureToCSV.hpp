/**
 * @file CaptureToCSV.hpp
 *
 * Beherbergt die Funktionalität für das Loggen von Scandaten in eine CSV-Datei in einem Beispielformat.
 *
 * @author Christian Blumberg
 * @date 2012-06-08
 * @version 0.0.1
 * erstes Release
 */

#ifndef CAPTURETOCSV_HPP_
#define CAPTURETOCSV_HPP_

#include "CaptureToFileAbstract.hpp"
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

namespace ibeo{ class IbeoLaserDataAbstract; }

namespace capture
{

/** @brief ermögtlich das Loggen in einem CSV-Format.
 *
 * Die Klasse ermöglicht das Loggen von Scandaten in einem CSV-Format.
 *
 * a@author Christian Blumberg
 * @date 2012-06-07
 * @version 0.0.1
 * erstes Release
 */
class CaptureToCSV : public CaptureToFileAbstract
{
public:
	CaptureToCSV(const std::string& filename, const std::string& separator);
	virtual ~CaptureToCSV();

	virtual void operator<<(ibeo::IbeoLaserDataAbstract& dataObj);
	//virtual void operator<<(ibeo::ibeoLaserDataAbstractSmartPtr& dataObj);
	


private:
	boost::mutex CSVmutex;
	std::string s;
	std::stringstream ss;
	char *buf;
};

} /* namespace capture */
#endif /* CAPTURETOCSV_HPP_ */
