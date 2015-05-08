package net.sf.tokentube.helpdesk.stripes.action.otp;

import java.util.Date;
import java.util.Arrays;
import java.text.ParseException;
import java.security.*;
import java.math.BigInteger;
import java.io.*;

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

            StringBuffer sb = new StringBuffer(otp.getBits()/2);
            for(int i=0; i<1+otp.getBits()/16; i++) {
                sb.append(super.actionBeanContext.getRequest().getParameter("cr.text"+(i+1)));
            }
            challenge = sb.toString();

            response = ChallengeResponse.calculateResponse(Arrays.copyOfRange(key, 0, otp.getBits()/8 ), Arrays.copyOfRange(hash, 0, otp.getBits()/8 ), challenge);
            sb = new StringBuffer( response );
            for(int i=4; i<sb.length(); i+=5) {
                sb.insert(i, "-");
            }
            response = sb.toString();

            otp.setChallengeResult( response );

            otp.incrementIterations();
            otp.setDateLastUsage(new Date());
            getDaoOTP().save(otp);
        } catch( Exception ex ) {
            StringWriter sw = new StringWriter();
            PrintWriter pw = new PrintWriter(sw);
            ex.printStackTrace(pw);
            getContext().getMessages().add(new SimpleMessage( sw.toString() ) );
        }
        return new ForwardResolution(super.GENERATE);
    }

    @DontValidate
    public Resolution done() {
        return new RedirectResolution(IndexActionBean.class);
    }

}
