/**
 * @file IbeoLaserException.hpp
 *
 * Enthält Klasse IbeoLaserException, die die Daten des Laserscanners ibeoLUX verarbeitet.
 *
 * @author Christian Blumberg
 * @date 2012-06-05
 */

#ifndef IBEOLASEREXCEPTION_H_
#define IBEOLASEREXCEPTION_H_

#include <exception>

namespace ibeo
{

	/** @brief allgemeine Exception für Laserscanner.
	 *
	 * Die allgemeine Exception, die bei Laserscannern auftreten kann,
	 */
	class IbeoLaserException : public std::exception
	{
	public:
		IbeoLaserException(){};
		virtual ~IbeoLaserException() throw(){};
		virtual const char* what() const throw()
		{
			return("Beim IbeoLaser trat ein unbekannter Fehler auf.");
		}
	};

	/** @brief Exception für erfolgloses Lesen von Daten.
	 *
	 * Die Exception wird immer dann geworfen, wenn es umöglich ist Daten vom Laserscanner zu lesen.
	 */
	class IbeoLaserReadException : public IbeoLaserException
	{
	public:
		IbeoLaserReadException(){};
		virtual ~IbeoLaserReadException() throw(){};
		virtual const char* what() const throw()
		{
			return("Es ist nicht möglich Daten vom Laserscanner zu lesen.");
		}
	};

	/** @brief Exception für erfolglosen Verbindungsaufbau.
	 *
	 * Die Exception wird immer dann geworfen, wenn keine Verbindung zu dem entsprechenden Laserscanner aufgebaut werden konnte.
	 */
	class IbeoLaserConnectionOpenException : public IbeoLaserException
	{
	public:
		IbeoLaserConnectionOpenException(){};
		virtual ~IbeoLaserConnectionOpenException() throw(){};
		virtual const char* what() const throw()
		{
			return("Es konnte keine Verbindung zum IbeoLaser hergestellt werden.");
		}
	};

	/** @brief Exception für fehlerhaftes Schließen der Verbindung.
	 *
	 * Die Exception wird immer dann geworfen, wenn die hergestellte Verbindung zu dem entsprechenden Laserscanner nicht mehr geschlossen werden konnte.
	 */
	class IbeoLaserConnectionCloseException : public IbeoLaserException
	{
	public:
		IbeoLaserConnectionCloseException(){};
		virtual ~IbeoLaserConnectionCloseException() throw(){};
		virtual const char* what() const throw()
		{
			return("Die Verbindung zum IbeoLaser konnte nicht beendet werden.");
		}
	};

	/** @brief fehlerhafter Index angegeben.
	 *
	 * Die Exception wird dann geworfen, wenn der angegebene Index fehlerhaft war.
	 */
	class IbeoLaserScanpointIndexException : public IbeoLaserException
	{
	public:
		IbeoLaserScanpointIndexException(){};
		virtual ~IbeoLaserScanpointIndexException() throw(){};
		virtual const char* what() const throw()
		{
			return("Dieser Index eines Scanpunktes existiert nicht.");
		}
	};

	/** @brief fehlerhafter Index angegeben.
	 *
	 * Die Exception wird dann geworfen, wenn der angegebene Index fehlerhaft war.
	 */
	class IbeoLaserDataIndexException : public IbeoLaserException
	{
	public:
		IbeoLaserDataIndexException(){};
		virtual ~IbeoLaserDataIndexException() throw(){};
		virtual const char* what() const throw()
		{
			return("Dieser Index des angeforderten Datums existiert nicht.");
		}
	};


} /* namespace ibeo */
#endif /* IBEOLASEREXCEPTION_H_ */
