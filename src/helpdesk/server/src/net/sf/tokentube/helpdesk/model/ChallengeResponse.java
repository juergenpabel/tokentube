package net.sf.tokentube.helpdesk.stripes.model;

import java.text.ParseException;


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


    public static String calculateResponse(byte[] keyBytes, byte[] hashBytes, String challenge) throws ParseException {
        byte[] challengeCodes, challengeBytes;
        byte[] responseCodes, responseBytes;

        if(challenge.length() != 4+hashBytes.length*2) {
            throw new ParseException("Incorrect length="+challenge.length()+", expected "+(4+hashBytes.length*2)+" characters", 0);
        }
        challengeCodes = challenge.getBytes();
        challengeBytes = new byte[challengeCodes.length/2];

        decodeData(challengeCodes, challengeBytes);
	//TODO: crc verification on challengeBytes

        responseBytes = new byte[challengeBytes.length];
        responseCodes = new byte[challengeBytes.length*2];
        for(int i=0; i<hashBytes.length; i++) {
            responseBytes[2+i] = (byte)(challengeBytes[2+i] ^ keyBytes[i] ^ hashBytes[i]);
        }

	//TODO: crc computation on response instead of zero'ing
        for(int i=0; i<2; i++) {
            responseBytes[i] = 0;
	}
        encodeData(responseBytes, responseCodes);
        return new String( responseCodes );
    }
}
