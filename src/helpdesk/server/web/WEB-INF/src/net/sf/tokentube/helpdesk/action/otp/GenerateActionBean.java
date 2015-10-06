package net.sf.tokentube.helpdesk.stripes.action.otp;

import java.util.Date;
import java.util.Arrays;
import java.text.ParseException;
import java.security.*;
import java.math.BigInteger;

import net.sourceforge.stripes.action.DefaultHandler;
import net.sourceforge.stripes.action.DontValidate;
import net.sourceforge.stripes.action.ForwardResolution;
import net.sourceforge.stripes.action.RedirectResolution;
import net.sourceforge.stripes.action.Resolution;
import net.sourceforge.stripes.action.SimpleMessage;
import net.sourceforge.stripes.action.UrlBinding;
import net.sf.tokentube.helpdesk.stripes.model.OTP;
import net.sf.tokentube.helpdesk.stripes.model.ChallengeResponse;

@UrlBinding("/otp/generate/{id}")
public class GenerateActionBean extends BaseActionBean {

    @DefaultHandler
    public Resolution show() {
        return new ForwardResolution(super.GENERATE);
    }

    public Resolution generate() {
        OTP otp;
        String luksKey;
        byte[] keyValue;
        byte[] hashValue;
        byte[] responseValue;
        String challenge;
        String response;

        otp = getOTP();
        luksKey = otp.getLuksKey();

        try {
            byte[] key = new byte[32];
            byte[] hash = new byte[32];
            for(int i=0; i<luksKey.length(); i+=2) {
                key[i/2] = (byte)(Integer.parseInt(luksKey.substring(i, i+2), 16));
                hash[i/2] = key[i/2];
            }
            MessageDigest md = MessageDigest.getInstance(otp.getHash());
            for(int i=0; i<otp.getIterations(); i++) {
                md.reset();
                md.update(hash, 0, otp.getBits()/8);
		md.digest(hash, 0, hash.length);
            }
            keyValue = new byte[luksKey.length()/2*8];
            for(int i=0; i<keyValue.length/8; i++) {
                int mask = 0x1;
                for(int j=0; j<8; j++) {
                    if( (((int)key[i]) & mask) != 0) {
                        keyValue[i*8+(7-j)] = 1;
                    } else {
                        keyValue[i*8+(7-j)] = 0;
                    }
                    mask *= 2;
                }
            }
            hashValue = new byte[luksKey.length()/2*8];
            for(int i=0; i<hashValue.length/8; i++) {
                int mask = 0x1;
                for(int j=0; j<8; j++) {
                    if( (((int)hash[i]) & mask) != 0) {
                        hashValue[i*8+(7-j)] = 1;
                    } else {
                        hashValue[i*8+(7-j)] = 0;
                    }
                    mask *= 2;
                }
            }

            StringBuffer sb = new StringBuffer(otp.getBits()/16*5 - 1);
            for(int i=0; i<otp.getBits()/16; i++) {
                sb.append(super.actionBeanContext.getRequest().getParameter("cr.text"+(i+1)));
            }
            for(int i=4; i<sb.length(); i+=5) {
                if(sb.charAt(i) != '\0') {
                    sb.insert(i, "-");
                }
            }
            challenge = sb.toString();

            otp.setChallengeResult(ChallengeResponse.calculateResponse(Arrays.copyOf(keyValue, otp.getBits() ), Arrays.copyOf(hashValue, otp.getBits() ), challenge));

            otp.incrementIterations();
            otp.setDateLastUsage(new Date());
            getDaoOTP().save(otp);
        } catch( Exception ex ) {
            getContext().getMessages().add(new SimpleMessage(ex.getMessage()));
        }
        return new ForwardResolution(super.GENERATE);
    }

    @DontValidate
    public Resolution done() {
        return new RedirectResolution(IndexActionBean.class);
    }

}
