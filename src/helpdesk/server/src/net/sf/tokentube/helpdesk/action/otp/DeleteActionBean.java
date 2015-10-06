package net.sf.tokentube.helpdesk.stripes.action.otp;

import java.util.List;

import net.sourceforge.stripes.action.DefaultHandler;
import net.sourceforge.stripes.validation.Validate;
import net.sourceforge.stripes.action.DontValidate;
import net.sourceforge.stripes.action.ForwardResolution;
import net.sourceforge.stripes.action.RedirectResolution;
import net.sourceforge.stripes.action.Resolution;
import net.sourceforge.stripes.action.SimpleMessage;
import net.sourceforge.stripes.action.UrlBinding;
import net.sf.tokentube.helpdesk.stripes.model.OTP;

@UrlBinding("/otp/delete/{id}")
public class DeleteActionBean extends BaseActionBean {

    @DefaultHandler
    public Resolution show() {
        return new ForwardResolution(super.DELETE);
    }

    @DontValidate
    public Resolution cancel() {
        return new RedirectResolution(IndexActionBean.class);
    }

    @Validate(field="id", minlength=1)
    public Resolution delete() {
        OTP deleted = getOTP();
        getDaoOTP().delete(deleted.getId());
        getContext().getMessages().add(
            new SimpleMessage("Deleted {0}.", deleted)
        );
        return new RedirectResolution(IndexActionBean.class);
    }
}
