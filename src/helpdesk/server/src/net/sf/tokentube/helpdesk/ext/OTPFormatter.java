package net.sf.tokentube.helpdesk.stripes.ext;


import java.util.Locale;
import net.sourceforge.stripes.format.Formatter;
import net.sf.tokentube.helpdesk.stripes.model.OTP;


public class OTPFormatter implements Formatter<OTP> {

    public void init() { }
    public void setLocale(Locale locale) { }
    public void setFormatType(String type) { }
    public void setFormatPattern(String pattern) { }

    public String format(OTP otp) {
        return String.valueOf(otp.getId());
    }
}

