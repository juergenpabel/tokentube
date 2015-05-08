package net.sf.tokentube.helpdesk.stripes.action.otp;

import java.util.Date;

import net.sourceforge.stripes.action.DefaultHandler;
import net.sourceforge.stripes.action.DontValidate;
import net.sourceforge.stripes.action.ForwardResolution;
import net.sourceforge.stripes.action.RedirectResolution;
import net.sourceforge.stripes.action.Resolution;
import net.sourceforge.stripes.action.SimpleMessage;
import net.sourceforge.stripes.action.UrlBinding;
import net.sourceforge.stripes.validation.EmailTypeConverter;
import net.sourceforge.stripes.validation.SimpleError;
import net.sourceforge.stripes.validation.Validate;
import net.sourceforge.stripes.validation.ValidateNestedProperties;
import net.sourceforge.stripes.validation.ValidationErrors;
import net.sourceforge.stripes.validation.ValidationMethod;
import net.sf.tokentube.helpdesk.stripes.model.OTP;

@UrlBinding("/otp/edit/{id}")
public class EditActionBean extends BaseActionBean {

    @DefaultHandler
    public Resolution edit() {
        return new ForwardResolution(super.EDIT);
    }

    @ValidateNestedProperties({
        @Validate(field="systemID", minlength=5, maxlength=25),
        @Validate(field="systemName", minlength=2, maxlength=40),
        @Validate(field="hash", minlength=1, maxlength=40),
        @Validate(field="dateRegistration", expression="${this <= today}"),
        @Validate(field="dateLastUsage", expression="${this <= today}")
    })
    public Resolution save() {
        OTP otp = getOTP();
        getDaoOTP().save(otp);
        getContext().getMessages().add(
            new SimpleMessage("{0} has been saved.", otp)
        );
        return new RedirectResolution(IndexActionBean.class);
    }

    @DontValidate
    public Resolution cancel() {
        getContext().getMessages().add(
            new SimpleMessage("Action cancelled.")
        );
        return new RedirectResolution(IndexActionBean.class);
    }

    //Override
//    public void setOTP(OTP otp) {
//        super.setOTP(otp);
//    }

    public Date getToday() {
        return new Date();
    }
}
