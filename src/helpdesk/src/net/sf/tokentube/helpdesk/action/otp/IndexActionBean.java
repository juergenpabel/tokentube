package net.sf.tokentube.helpdesk.stripes.action.otp;

import java.util.List;

import net.sourceforge.stripes.action.DefaultHandler;
import net.sourceforge.stripes.action.ForwardResolution;
import net.sourceforge.stripes.action.RedirectResolution;
import net.sourceforge.stripes.action.Resolution;
import net.sourceforge.stripes.action.SimpleMessage;
import net.sourceforge.stripes.action.UrlBinding;
import net.sf.tokentube.helpdesk.stripes.model.OTP;

@UrlBinding("/otp/")
public class IndexActionBean extends BaseActionBean {

    @DefaultHandler
    public Resolution list() {
        return new ForwardResolution(super.INDEX);
    }

    public List<OTP> getOTPs() {
        return getDaoOTP().read();
    }
}
