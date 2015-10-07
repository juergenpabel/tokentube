package net.sf.tokentube.helpdesk.stripes.action.otp;

import java.util.Date;
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
		return new ForwardResolution( BaseActionBean.GENERATE );
	}

	public Resolution generate() {
		OTP otp;
		String challenge;
		String response;

		otp = getOTP();
		try {
			StringBuffer sb = new StringBuffer( otp.getBits()/2 );
			for(int i=0; i<1+otp.getBits()/16; i++) {
				sb.append( super.actionBeanContext.getRequest().getParameter( "cr.text"+(i+1) ) );
			}
			challenge = sb.toString();

			response = ChallengeResponse.calculateResponse( otp, challenge );

			sb = new StringBuffer( response );
			for(int i=4; i<sb.length(); i+=5) {
				sb.insert( i, "-" );
			}
			response = sb.toString();

			otp.setChallengeResult( response );

			otp.incrementIterations();
			otp.setDateLastUsage( new Date() );
			getDaoOTP().save( otp ) ;
		} catch( Exception ex ) {
			StringWriter sw = new StringWriter();
			PrintWriter pw = new PrintWriter(sw);
			ex.printStackTrace(pw);
			getContext().getMessages().add(new SimpleMessage( ex.getMessage() ) );
		}
		
		return new ForwardResolution( BaseActionBean.GENERATE );
	}

	@DontValidate
	public Resolution done() {
		return new RedirectResolution( IndexActionBean.class );
	}

}
