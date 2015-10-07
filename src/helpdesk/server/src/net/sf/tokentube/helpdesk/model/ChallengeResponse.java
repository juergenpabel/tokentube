package net.sf.tokentube.helpdesk.stripes.model;


import java.util.Arrays;
import java.text.ParseException;
import java.security.*;


class CRC16 { 

	private static final int[] table = {
            0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
            0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
            0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
            0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
            0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
            0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
            0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
            0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
            0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
            0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
            0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
            0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
            0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
            0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
            0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
            0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
            0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
            0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
            0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
            0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
            0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
            0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
            0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
            0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
            0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
            0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
            0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
            0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
            0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
            0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
            0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
            0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040,
	};


	public static int calc(byte[] input, int offset, int length ) {
		int crc = 0x0000;

		for ( int i=0; i<length; i++ ) {
			crc = (crc >>> 8) ^ table[(crc ^ input[offset+i]) & 0xff];
		}
		return crc;
	}

}


public abstract class ChallengeResponse {

	private final static String charSetString = new String("abcdefghkmnopqrstuvwxyz023456789");
	private final static byte[] charSetArray = charSetString.getBytes();


public static void encodeData(byte[] inputBytes, byte[] outputCodes) {
        int inputPosition = 0;
        int outputPosition = 0;

        if( inputBytes.length % 2 != 0 ) {
            return;
        }
        if( inputBytes.length != outputCodes.length/2 ) {
            return;
        }
        while(inputPosition < inputBytes.length) {
            byte b[] = new byte[4];
            int bits = 0;
            int val = 0;

            bits = 0;
            b[0] = (byte)((inputBytes[inputPosition+0] & 0xf0) >> 4);
            b[1] = (byte)((inputBytes[inputPosition+0] & 0x0f) >> 0);
            b[2] = (byte)((inputBytes[inputPosition+1] & 0xf0) >> 4);
            b[3] = (byte)((inputBytes[inputPosition+1] & 0x0f) >> 0);

            bits = ((int)(inputBytes[inputPosition+0]<<8)&0xff00) | ((byte)(inputBytes[inputPosition+1]<<0)&0x00ff);
            bits <<= 4;
            bits |= (b[0] ^ b[1] ^ b[2] ^ b[3]) & 0x0f;
            
            for(int i=0; i<4; i++) {
                val = bits & 0x1f;
                bits >>= 5;
                outputCodes[outputPosition+3-i] = charSetArray[val];
            }
            outputPosition += 4;
            inputPosition += 2;
        }
    }


    public static void decodeData(byte[] inputCodes, byte[] outputBytes) throws ParseException {
        int inputPosition = 0;
        int outputPosition = 0;

        while(inputPosition < inputCodes.length) {
            byte b[] = new byte[5];
            int bits = 0;
            int val = 0;

            b[0] = (byte)charSetString.indexOf(inputCodes[inputPosition+0]);
            b[1] = (byte)charSetString.indexOf(inputCodes[inputPosition+1]);
            b[2] = (byte)charSetString.indexOf(inputCodes[inputPosition+2]);
            b[3] = (byte)charSetString.indexOf(inputCodes[inputPosition+3]);

            bits = 0;
            for(int i=0; i<4; i++) {
                bits <<= 5;
                bits |= b[i] & 0x1f;
            }
            val = (bits & 0xffff0) >> 4;

            for(int i=0; i<5; i++) {
                b[i] = (byte)(bits&0xf);
                bits >>= 4;
            }
            if((b[0] ^ b[1] ^ b[2] ^ b[3] ^ b[4]) != 0) {
                throw new ParseException("Code incorrect in character block "+(inputPosition/4+1)+"("+val+":"+b[0]+"/"+b[1]+"/"+b[2]+"/"+b[3]+"/"+b[4]+")", inputPosition/4+1);
            }
            
            outputBytes[outputPosition+0] = (byte)(((int)(val&0xff00))>>8);
            outputBytes[outputPosition+1] = (byte)(((int)(val&0x00ff))>>0);
            outputPosition += 2;
            inputPosition += 4;
        }
    }


	public static String calculateResponse(OTP otp, String challenge) throws ParseException {
		byte[] challengeCodes, challengeBytes;
		byte[] responseCodes, responseBytes;
		String keyCodes = null;
		byte[] keyBytes = new byte[32];
		byte[] hashBytes = null;
		int i, crc;
		byte crc0, crc1;

		keyCodes = otp.getLuksKey();
		keyBytes = new byte[ keyCodes.length() / 2 ];
		if( challenge.length() != 4+otp.getBits()/8*2 ) {
			throw new ParseException( "Invalid input, check input for typing errors", 0 );
		}

		challengeCodes = challenge.getBytes();
		challengeBytes = new byte[challengeCodes.length/2];
		decodeData( challengeCodes, challengeBytes );

		for( i=0; i<keyCodes.length(); i+=2) {
			keyBytes[i/2] = (byte)(Integer.parseInt( keyCodes.substring( i, i+2 ), 16) );
		}

		hashBytes = new byte[otp.getBits()/8];
		try {
			MessageDigest md = MessageDigest.getInstance( otp.getHash() );
			byte[] data = new byte[md.getDigestLength()];

			for( i=0; i<otp.getBits()/8; i++) {
				data[i] = keyBytes[i];
			}
			for( i=0; i<otp.getIterations(); i++ ) {
				md.reset();
				md.update( data, 0, otp.getBits()/8 );
				md.digest( data, 0, data.length );
			}
			for( i=0; i<otp.getBits()/8; i++) {
				hashBytes[i] = data[i];
			}
		} catch( Exception ex ) {
			throw new ParseException( "MessageDigest exception: " + ex.getMessage(), 0 );
		}

		crc = CRC16.calc( hashBytes, 0, otp.getBits()/8 );
		crc0 = (byte)((crc >>> 8 ) & 0xff);
		crc1 = (byte)((crc >>> 0 ) & 0xff);

		if( crc0 != challengeBytes[0] || crc1 != challengeBytes[1] ) {
			throw new ParseException( "Invalid, check input for typing errors", 0 );
		}

		responseBytes = new byte[2+otp.getBits()/8];
		responseCodes = new byte[responseBytes.length*2];
		for( i=0; i<otp.getBits()/8; i++) {
			responseBytes[2+i] = (byte)(challengeBytes[2+i] ^ keyBytes[i] ^ hashBytes[i]);
		}

		try {
			MessageDigest md = MessageDigest.getInstance( otp.getHash() );
			byte[] data = new byte[md.getDigestLength()];

			for( i=0; i<otp.getBits()/8; i++) {
				data[i] = hashBytes[i];
			}
			md.update( data, 0, otp.getBits()/8 );
			md.digest( data, 0, data.length );
			for( i=0; i<otp.getBits()/8; i++) {
				hashBytes[i] = data[i];
			}
		} catch( Exception ex ) {
			throw new ParseException( "MessageDigest exception: " + ex.getMessage(), 0 );
		}
		crc = CRC16.calc( hashBytes, 0, otp.getBits()/8 );
		crc0 = (byte)((crc >>> 8 ) & 0xff);
		crc1 = (byte)((crc >>> 0 ) & 0xff);
		responseBytes[0] = crc0;
		responseBytes[1] = crc1;
		encodeData( responseBytes, responseCodes );
		return new String( responseCodes );
	}

}

