package net.sf.tokentube.helpdesk.stripes.action.otp;

import net.sourceforge.stripes.action.ActionBean;
import net.sourceforge.stripes.action.ActionBeanContext;
import net.sf.tokentube.helpdesk.stripes.dao.DaoOTP;
import net.sf.tokentube.helpdesk.stripes.dao.mock.MockDaoOTP;
import net.sf.tokentube.helpdesk.stripes.model.OTP;


public abstract class BaseActionBean implements ActionBean {
    protected ActionBeanContext actionBeanContext;

    protected static final String INDEX="/WEB-INF/jsp/otp/index.jsp";
    protected static final String VIEW="/WEB-INF/jsp/otp/view.jsp";
    protected static final String EDIT="/WEB-INF/jsp/otp/edit.jsp";
    protected static final String DELETE="/WEB-INF/jsp/otp/delete.jsp";
    protected static final String GENERATE="/WEB-INF/jsp/otp/generate.jsp";

    protected OTP otp;

    public ActionBeanContext getContext() {
        return actionBeanContext;
    }
    public void setContext(ActionBeanContext actionBeanContext) {
        this.actionBeanContext = actionBeanContext;
    }

    public OTP getOTP() {
        return otp;
    }

    public void setId(Integer id) {
        this.otp = otpDao.read(id);
    }

    public void setOTP(OTP otp) {
        this.otp = otp;
    }

    private DaoOTP otpDao = MockDaoOTP.getInstance();
    protected DaoOTP getDaoOTP() {
        return otpDao;
    }
}
