package net.sf.tokentube.helpdesk.stripes.model;

import java.text.ParseException;


public abstract class ChallengeResponse {
    private final static String charSetString = new String("abcdefghkmnopqrstuvwxyz023456789");
    private final static byte[] charSetArray = charSetString.getBytes();


    private static void encodeData(byte[] inputBits, byte[] outputBytes) {
        int inputPosition = 0;
        int outputPosition = 0;

        while(inputPosition < inputBits.length) {
            byte b[] = new byte[4];
            int bits = 0;
            int val = 0;

            for(int i=0; i<4; i++) {
                for(int j=0; j<4; j++) {
                    bits <<= 1;
                    bits |= inputBits[inputPosition+4*i+j];
                }
            }

            for(int i=0; i<4; i++) {
                for(int j=0; j<4; j++) {
                    b[i] <<= 1;
                    b[i] |= inputBits[inputPosition+4*i+j];
                }
            }
            bits <<= 4;
            bits |= b[0] ^ b[1] ^ b[2] ^ b[3];
            
            for(int i=0; i<4; i++) {
                val = bits & 0x1f;
                bits >>= 5;
                outputBytes[outputPosition+(3-i)] = charSetArray[val];
                inputPosition += 4;
            }
            outputPosition += 4;
        }
    }


    public static void decodeData(byte[] inputBytes, byte[] outputBits) throws ParseException {
        int inputPosition = 0;
        int outputPosition = 0;

        while(inputPosition < inputBytes.length) {
            byte b[] = new byte[5];
            int bits = 0;
            int val = 0;

            for(int i=0; i<4; i++) {
                b[i] = (byte)charSetString.indexOf(inputBytes[inputPosition+i]);
            }

            for(int i=0; i<4; i++) {
                bits <<= 5;
                bits |= b[i];
            }
            val = (bits & 0xffff0) >> 4;

            for(int i=0; i<5; i++) {
                b[i] = (byte)(bits&0xf);
                bits >>= 4;
            }
            if((b[0] ^ b[1] ^ b[2] ^ b[3] ^ b[4]) != 0) {
                throw new ParseException("Code incorrect in character block "+(inputPosition/4+1), inputPosition/4+1);
            }
            
            for(int i=0; i<4; i++) {
                for(int j=0; j<4; j++) {
                    outputBits[outputPosition+(15-(4*i+j))] = (byte)(val&0x1);
                    val >>= 1;
                }
            }
            outputPosition += 16;
            inputPosition += 4;
        }
    }


    public static String calculateResponse(byte[] keyBits, byte[] hashBits, String challenge) throws ParseException {
        byte[] challengeBits, challengeBytes;
        byte[] responseBits, responseBytes;

        if(challenge.replaceAll("-", "").length()*4 != 16+hashBits.length) {
            throw new ParseException("Incorrect length, expected "+hashBits.length+" characters", challenge.length());
        }
        for(int i=5; i<challenge.length(); i+=5) {
            if(challenge.charAt(i-1) != '-') { 
                throw new ParseException("Invalid format, expected '-' at position "+i, i);
            }
        }
        challenge = challenge.replaceAll("-", "");

        challengeBytes = challenge.getBytes();
        challengeBits = new byte[16+hashBits.length];
        responseBits  = new byte[16+hashBits.length];
        responseBytes  = new byte[2+hashBits.length/8];

        decodeData(challengeBytes, challengeBits);
	//TODO: crc verification on challengeBits

        for(int i=0; i<hashBits.length; i++) {
            responseBits[16+i] = (byte)((challengeBits[16+i]&0x1) ^ (keyBits[i]&0x1) ^ (hashBits[i]&0x1));
        }

	//TODO: crc computation on response instead of zero'ing
        for(int i=0; i<16; i++) {
            responseBits[i] = 0;
	}
        encodeData(responseBits, responseBytes);

        StringBuffer result = new StringBuffer();
        StringBuilder sb = new StringBuilder(new String(responseBytes));
        for(int i=4; i<sb.length(); i+=5) {
            if(sb.charAt(i) != '\0') {
                sb.insert(i, "-");
            }
        }
        return new String(sb.toString());
    }
}
