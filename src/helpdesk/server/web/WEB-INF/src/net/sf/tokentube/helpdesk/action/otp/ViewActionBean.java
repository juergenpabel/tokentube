package net.sf.tokentube.helpdesk.stripes.action.otp;

import java.util.Date;

import net.sourceforge.stripes.action.DefaultHandler;
import net.sourceforge.stripes.action.DontValidate;
import net.sourceforge.stripes.action.ForwardResolution;
import net.sourceforge.stripes.action.RedirectResolution;
import net.sourceforge.stripes.action.Resolution;
import net.sourceforge.stripes.action.UrlBinding;
import net.sf.tokentube.helpdesk.stripes.model.OTP;

@UrlBinding("/otp/view/{id}")
public class ViewActionBean extends BaseActionBean {
    @DefaultHandler
    public Resolution view() {
        return new ForwardResolution(super.VIEW);
    }

    @DontValidate
    public Resolution cancel() {
        return new RedirectResolution(IndexActionBean.class);
    }
}
