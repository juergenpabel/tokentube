package net.sf.tokentube.helpdesk.stripes.ext;

import java.util.Collection;
import java.util.Locale;

import net.sourceforge.stripes.validation.SimpleError;
import net.sourceforge.stripes.validation.TypeConverter;
import net.sourceforge.stripes.validation.ValidationError;

import net.sf.tokentube.helpdesk.stripes.dao.DaoOTP;
import net.sf.tokentube.helpdesk.stripes.dao.mock.MockDaoOTP;
import net.sf.tokentube.helpdesk.stripes.model.OTP;


public class OTPTypeConverter implements TypeConverter<OTP> {

	private DaoOTP DaoOTP = MockDaoOTP.getInstance();


	public OTP convert(String string, Class<? extends OTP> type, Collection<ValidationError> errors) {
		try {
			return DaoOTP.read( new Integer( string ) );
		}
		catch(Exception ex) {
			errors.add( new SimpleError("The OTP ID {1} is not valid.") );
			return null;
		}
	}


	public void setLocale(Locale locale) {
	}
}
