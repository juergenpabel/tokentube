package net.sf.tokentube.helpdesk.stripes.dao;

import java.util.List;
import net.sf.tokentube.helpdesk.stripes.model.OTP;

public interface DaoOTP {
    public List<OTP> read();
    public OTP read(Integer id);
    public void save(OTP otp);
    public void delete(Integer id);
    public OTP findBySystemID(String systemID);
}
