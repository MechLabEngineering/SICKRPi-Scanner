/**
 * @file Convert.hpp
 *
 * Enthält die Klasse Convert im Namensraum convert, die alle notwendigen Funktionalitäten für das Konvertieren von Datentypen bereitstellt.
 *
 * @author Christian Blumberg
 * @date 2012-06-04
 */

#ifndef CONVERT_HPP_
#define CONVERT_HPP_

#include "ConvertException.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <limits>
#include "../Detect/Detect.hpp"
#include <typeinfo>

namespace convert
{
	/** @brief Konvertierung von verschiedenen Datentypen
	 *
	 * Die Klasse Convert konvertiert Daten von einem Typ in einen anderen.
	 */
	class Convert
	{
	private:
		Convert(){}
		virtual ~Convert(){}

		template<typename T> T
		static switchByteOrder(const T& value)
		{
			T newValue = 0;
			T mask = 0xFF;

			int sizeof_T = sizeof(T);

			for(int i = 0; i < sizeof_T; i++)
			{
				newValue <<= 8;
				T v = ( ((value & mask) >> (i * 8)) & 0xFF);
				newValue |= v;
				mask <<= 8;
			}
			return(newValue);
		}

	public:
		/**
		 * Die Methode wandelt den angegebenen Datentyp in einen String um.
		 *
		 * @param[in] t Der Wert, der in einen String konvertiert werden soll.
		 * @return Gibt den String zurück mit dem konvertierten Wert. Der Wert selbst wird nicht verändert.
		 */
		template<typename T> std::string
		inline static toString(const T& t)
		{
			std::ostringstream stream;

			if(typeid(T) == typeid(char))
			{
				short x = static_cast<short>(t) & 0xFF;
				stream << x;
			}
			else if(typeid(T) == typeid(unsigned char))
			{
				short x = static_cast<unsigned short>(t) & 0xFF;
				stream << x;
			}
			else
			{
				stream << t;
			}

			return stream.str();
		}

		/**
		 * Die Methode wandelt den angegebenen String in den angegebenen Datentyp um.
		 *
		 * @param[in] s Der String, der in den angegebenen Datentyp konvertiert werden soll.
		 * @return Gibt den Wert von angegebenen Typ zurück. Der übergebene String wird nicht verändert.
		 *
		 * @throw ConvertInvalidArgumentException Die Exception wird geworfen, wenn der übergebene String leer ist.
		 * @throw ConvertOutOfRangeException      Die Exception wird geworfen, wenn der mit dem String übergebene Wert nicht im Wertebereich des angegebenen Datentyps liegt.
		 */
		template<typename T> T
		inline static fromString(const std::string& s)
		{
			if(s.empty()) throw(ConvertInvalidArgumentException());

		    T t;
			std::stringstream stream(s);

			stream >> t;
			if(t == 0)
			{
		    	if(s.find_first_not_of('0', 0) == std::string::npos) return t;
		    	else throw(ConvertInvalidArgumentException());
			}
			if(std::numeric_limits<T>::max() == t) throw(ConvertOutOfRangeException());
			if(std::numeric_limits<T>::min() == t) throw(ConvertOutOfRangeException());
		    return t;
		}

		/**
		 * Wandelt einen Wert vom anzugebenen Datentyp in einen String um, der diesen Wert als hexadezimale Zahl enthält.
		 *
		 * @param[in] t Der Wert, der in einen String umgewandelt werden soll.
		 * @return Gibt den String zurück, der den hexadezimalen Wert enthält. Der übergebene Wert wird nicht verändert.
		 */
		template<typename T> std::string
		inline static toHexString(const T& t)
		{
			 std::ostringstream stream;
			 stream << std::hex << static_cast<int>(t);
			 std::string s = stream.str();
			 return s;
		}

		/**
		 * wandelt einen Hexadezimalen String in eine int oder unsigned int Zahl um, double, float u.a. sind nicht möglich und müssen nachträglich gecastet werden
		 *
		 * @param[in] s Gibt den Wert vom angegebenen Datentyp zurück, der in einem String als hexadezimaler Wert gespeichert wurde.
		 * @return Gibt den Wert vom angegebenen Typ zurück. Der übergebene String wird nicht verändert.
		 *
		 * @throw ConvertInvalidArgumentException Die Exception wird geworfen, wenn der übergebene String leer oder unlesbar, d.h. keinen hexadezimalen Wert darstellt.
		 * @throw ConvertOutOfRangeException      Die Exception wird geworfen, wenn der mit dem String übergebene Wert nicht im Wertebereich des angegebenen Datentyps liegt.
		 */
		template<typename T> T
		inline static fromHexString(const std::string& s)
		{
			if(s.empty()) throw(ConvertInvalidArgumentException());

			T t;
			unsigned int pos;
			std::stringstream stream;
			std::string tmpString(s);

			if((pos = tmpString.find("0x")) != std::string::npos) tmpString.erase(pos, 2);
			if(tmpString.size() % 2 == 1) stream << "0";
			stream << std::hex << tmpString;
			if(stream.str().size() / 2 > sizeof(T)) throw(ConvertOutOfRangeException());
			stream >> t;

			if(t == 0)
			{
				if(tmpString.find_first_not_of('0', 0) == std::string::npos) return(t);
				else throw(ConvertInvalidArgumentException());
			}
			return t;
		}

		/**
		 * Wandelt einen mehrbytigen littleEndian-Wert auf bigEndian um. Diese Methode bewirkt nichts auf bigEndian-Systemen.
		 * @param value Der Wert, der nach bigEndian gewandelt werden soll.
		 * @return Als Wert wird der nach bigEndian konvertierte Wert zurückgegeben. Der übergebene Wert wird nicht verändert.
		 */
		template<typename T> T
		inline static host_to_network_byteOrder(const T& value)
		{
			if(!detect::Detect::isThisBigEndian())
			{
				Convert::switchByteOrder<T>(value);
			}
			return(value);
		}

		/**
		 * Wandelt einen mehrbytigen bigEndia-Wert auf littleEndian um. Diese Methode bewirkt nichts auf bigEndian-Systemen.
		 * @param value Der Wert, der nach littleEndian gewandelt werden soll.
		 * @return Als Wert wird der nach littleEndian konvertierte Wert zurückgegeben. Der übergebene Wert wird nicht verändert.
		 */
		template<typename T> T
		inline static network_to_host_byteOrder(const T& value)
		{
			if(!detect::Detect::isThisBigEndian())
			{
				 return(Convert::switchByteOrder<T>(value));
			}
			return(value);
		}

		/**
		 * Diese Methode wandelt zwangsweise einen littleEndian-Wert nach bigEndian um. Diese Methode bewirkt nichts auf littleEndian-Systemen.
		 * @param value Der Wert, der nach bigEndian konvertiert werden soll.
		 * @return Der nach bigEndian gewandelte Wert. Der übergebene Wert wir nicht verändert.
		 */
		template<typename T> T
		inline static switch_from_littleEndian_to_bigEndian(const T& value)
		{
			if(detect::Detect::isThisBigEndian())
			{
				return(Convert::switchByteOrder(value));
			}
			return(value);
		}
	};
}

#endif /* CONVERT_HPP_ */
