/**
 * @file CaptureToCSV.cpp
 *
 * Beherbergt die Implementierungen der Funktionalität für das Loggen von Scandaten in eine CSV-Datei in einem Beispielformat.
 */

#include "../Additional/Defines/Defines.hpp"

#include <sstream>
#include <string>

#include "../Capture/CaptureToCSV.hpp"
#include "../IbeoLaser/IbeoLaserScanpoint.hpp"
#include "../Additional/Convert/Convert.hpp"
#include "../IbeoLaser/IbeoLaserDataAbstract.hpp"

namespace capture
{

CaptureToCSV::CaptureToCSV(const std::string& filename, const std::string& separator) :
		CaptureToFileAbstract(filename, separator)
{
//	fprintf(fp, "# Laserscanner: LSR: <Zeitstempel>|<Ebene>|<Echo>|<Winkel>|<Radius>|<Xwert>|<Ywert>|<EchoPulsBreite>|<Scanflags>\r\n");
	file << "Zeitstempel|Ebene|Echo|Winkel|Radius|Xwert|Ywert|EchoPulsBreite|Scanflags" << std::endl;
	buf = new char[1000000];
}

CaptureToCSV::~CaptureToCSV()
{

}

//void CaptureToCSV::operator<<(ibeo::ibeoLaserDataAbstractSmartPtr& dataObj)
void CaptureToCSV::operator<<(ibeo::IbeoLaserDataAbstract& dataObj)
{
	boost::lock_guard<boost::mutex> lock(CSVmutex);

	ss.str("");

	unsigned int nofSp = dataObj.getNumberOfScanpoints();
	u_int_64_t ts = dataObj.getTimeStamp();

//#pragma warning
//	for(unsigned int i = 0; i < nofSp - 1; i++)

	for(unsigned int i = 0; i < nofSp; i++)
	{
		ibeo::IbeoLaserScanpoint* sp = dataObj.getScanPointAt(i);

//		// wird genutzt für Performancesteigerungen, ohne dafür Daten ausgeben zu müssen --> kann bei Bedarf gelöscht werden ohne Beeinträchtigungen
//		---------------------------------------------------------------------------------------------------------------------------------------------
//		ibeo::IbeoLaserScanpoint* sp2 = dataObj.getScanPointAt(i+1);
//
//		ts += ts;
//		sp->echo += sp2->echo;
//		sp->echoPulseWidth += sp2->echoPulseWidth;
//		sp->horizontalAngle += sp2->horizontalAngle;
//		sp->layer += sp2->layer;
//		sp->radialDistance += sp2->radialDistance;
//		sp->scanFlag += sp2->scanFlag;
//		sp->xValue += sp2->xValue;
//		sp->yValue += sp2->yValue;
//		---------------------------------------------------------------------------------------------------------------------------------------------


// Zum Speichern der Laserscannerdaten kann man auch diese C++-Funktionen nehmen. Leider sind diese um ca. 30% langsamer als die C-Funktionen.
		ss << "LSR: ";
		ss << ts;
		ss << separator;
		ss << static_cast<unsigned short>(sp->layer);
		ss << separator;
		ss << static_cast<unsigned short>(sp->echo);
		ss << separator;
		ss << sp->horizontalAngle;
		ss << separator;
		ss << sp->radialDistance;
		ss << separator;
		ss << sp->xValue;
		ss << separator;
		ss << sp->yValue;
		ss << separator;
		ss << sp->echoPulseWidth;
		ss << separator;
		ss << static_cast<unsigned short>(sp->scanFlag);
		ss << std::endl;


//		fprintf(fp, "LSR: %"PRI64u"%c%d%c%d%c%lf%c%lf%c%lf%c%lf%c%d%c%d\r\n",
//						ts,
//						charSeparator,
//						sp->layer,
//						charSeparator,
//						sp->echo,
//						charSeparator,
//						sp->horizontalAngle,
//						charSeparator,
//						sp->radialDistance,
//						charSeparator,
//						sp->xValue,
//						charSeparator,
//						sp->yValue,
//						charSeparator,
//						sp->echoPulseWidth,
//						charSeparator,
//						sp->scanFlag
//						);
//		fprintf(fp, "LSR: %d%c%d%c%lf%c%lf%c%d%c%d\r\n",
//
//								sp->layer,
//								charSeparator,
//								sp->echo,
//								charSeparator,
//								sp->horizontalAngle,
//								charSeparator,
//								sp->radialDistance,
//								charSeparator,
//								sp->echoPulseWidth,
//								charSeparator,
//								sp->scanFlag
//								);




	}
	file << ss.str(); // für C++


}


} /* namespace capture */
