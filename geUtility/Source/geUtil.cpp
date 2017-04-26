/********************************************************************/
/**
 * @file   geUtil.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/03/06
 * @brief  
 *
 * Long Description
 *
 * @bug	   No known bugs.
 */
/********************************************************************/

/************************************************************************************************************************/
/* Includes																												*/
/************************************************************************************************************************/
#include "gePrerequisitesUtil.h"
#include "Externals/md5.h"

namespace geEngineSDK
{
	String md5(const WString& source)
	{
		MD5 md5;
		md5.update( (uint8*)source.c_str(), (uint32)source.length() * sizeof(WString::value_type) );
		md5.finalize();

		uint8 digest[16];
		md5.decdigest( digest, sizeof(digest) );

		ANSICHAR buf[33];
		for( uint32 i=0; i<16; i++ )
		{
			sprintf( buf + i * 2, "%02x", digest[i] );
		}
		buf[32] = 0;	//Last character NULL

		return String(buf);
	}

	String md5(const String& source)
	{
		MD5 md5;
		md5.update( (uint8*)source.c_str(), (uint32)source.length() * sizeof(String::value_type) );
		md5.finalize();

		uint8 digest[16];
		md5.decdigest(digest, sizeof(digest));

		ANSICHAR buf[33];
		for( uint32 i=0; i<16; i++ )
		{
			sprintf(buf + i * 2, "%02x", digest[i]);
		}
		buf[32] = 0;	//Last character NULL

		return String(buf);
	}
}
