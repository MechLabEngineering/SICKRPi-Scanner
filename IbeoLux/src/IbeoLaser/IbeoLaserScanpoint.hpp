/**
 * @file IbeoLaserScanpoint.hpp
 *
 * Enthält die Klasse IbeoLaserScanpoint, die einen Scanpunkt eines Laserscanners repräsentiert.
 *
 * @author Christian Blumberg
 * @date 2012-06-05
 */

#ifndef IBEOLASERSCANPOINT_HPP_
#define IBEOLASERSCANPOINT_HPP_

namespace capture{class CaptureToCSV;}

namespace ibeo
{

	/** @brief Ein Scanpunkt eines Laserscanners.
	 *
	 * Diese Klasse repräsentiert einen Scanpunkt eines Laserscanners, speichert die entsprechenden Werte und macht sie zugänglich.
	 */
	class IbeoLaserScanpoint
	{
	public:

		friend class capture::CaptureToCSV;

		IbeoLaserScanpoint();

		IbeoLaserScanpoint(	const unsigned char& layer,
							const unsigned char& echo,
							const unsigned char& scanFlag,
							const float& horizontalAngle,
							const float& radialDistance,
							const float& xValue,
							const float& yValue,
							const float& zValue,
							const unsigned short& echoPulseWidth,
							unsigned char* datapointer
						  );

		virtual ~IbeoLaserScanpoint() {};

		void setData(	const unsigned char& layer,
						const unsigned char& echo,
						const unsigned char& scanFlag,
						const float& horizontalAngle,
						const float& radialDistance,
						const float& xValue,
						const float& yValue,
						const float& zValue,
						const unsigned short& echoPulseWidth,
						unsigned char* datapointer
					);

		const unsigned char& getLayer() const;
		const unsigned char& getEcho() const;
		const unsigned char& getScanFlag() const;
		const float& getHorizontalAngle() const;
		const float& getRadialDistance() const;
		const float& getXValue() const;
		const float& getYValue() const;
		const float& getZValue() const;
		const unsigned short& getEchoPulseWidth() const;

	private:
		unsigned char layer;				// Ebene des Scanpunktes (char für 1 Byte, Auswertung einfach als INT machen)
		unsigned char echo;				// Echonummer von diesem Punkt
		unsigned char scanFlag;				// gibt an um was für ein Punkt es sich handelt. Wird direkt vom ibeoLaser übermittelt
		float horizontalAngle;				// der eigentliche Horizonzalwinkel
		float radialDistance;				// Entfernung des Punktes bezogen auf das Scannerkoordinatensystem
		unsigned short echoPulseWidth;			// erkannte Breite des Echopulses
		float xValue;					// der X-Wert des Scanpunktes
		float yValue;
		float zValue;
		unsigned char* datapointer;
	};

} /* namespace ibeo */
#endif /* IBEOLASERSCANPOINT_HPP_ */
