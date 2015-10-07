package net.sf.tokentube.helpdesk.stripes.dao.mock;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import net.sf.tokentube.helpdesk.stripes.dao.DaoOTP;
import net.sf.tokentube.helpdesk.stripes.model.OTP;

public class MockDaoOTP implements DaoOTP {
    private List<OTP> list = new ArrayList<OTP>();
    private Map<Integer,OTP> map = new HashMap<Integer,OTP>();
    private int nextId = 1;

    public List<OTP> read() {
        List<OTP> result = new ArrayList<OTP>(list.size());
        for (OTP iter: list) {
            result.add(copyOTP(iter));
        }
        return result;
    }
    public OTP read(Integer id) {
        return copyOTP(map.get(id));
    }
    public void save(OTP otp) {
        if (otp != null) {
            if (otp.getId() == null) {
                int id = nextId++;
                otp.setId(id);
                list.add(otp);
            }
            else {
                Integer id = otp.getId();
                list.set(list.indexOf(map.get(id)), otp);
            }
            map.put(otp.getId(), otp);
        }
    }
    public void delete(Integer id) {
        OTP otp = map.get(id);
        list.remove(otp);
        map.remove(id);
    }

    private static final int FIELD_SYSID         =  0;
    private static final int FIELD_OTP_KEY       =  1;
    private static final int FIELD_OTP_HASH   =  2;
    private static final int FIELD_OTP_BITS   =  3;
    private static final int FIELD_OTP_ITER =  4;
    private static final int FIELD_DATE_REGISTRATION =  5;
    private static final int FIELD_DATE_LAST_USAGE   =  6;

    private static final String[] RAW_DATA = {
        "LAPTOP-4711,00000000000000000000000000000000,SHA-256,64,1,2015-05-01 00:00,2015-05-01 00:00"
    };

    private MockDaoOTP() {
        try {
            for (String string : RAW_DATA) {
                save(createOTPFromString(string));
            }
        }
        catch (Exception exc) {
            throw new RuntimeException(exc);
        }
    }

    private static MockDaoOTP instance = new MockDaoOTP();
    public static MockDaoOTP getInstance() { return instance; }

    private OTP createOTPFromString(String string)
        throws Exception
    {
        String[] fields = string.split(",");

        OTP otp = new OTP();
        otp.setSystemID(fields[FIELD_SYSID]);
        otp.setLuksKey(fields[FIELD_OTP_KEY]);
        otp.setHash(fields[FIELD_OTP_HASH]);
        otp.setIterations(Integer.parseInt(fields[FIELD_OTP_ITER]));
        otp.setDateRegistration(new SimpleDateFormat("yyyy-MM-dd HH:mm").parse(fields[FIELD_DATE_REGISTRATION]));
        otp.setDateLastUsage(new SimpleDateFormat("yyyy-MM-dd HH:mm").parse(fields[FIELD_DATE_LAST_USAGE]));
        otp.setBits(Integer.parseInt(fields[FIELD_OTP_BITS]));
        return otp;
    }

    public OTP findBySystemID(String systemID) {
        OTP result = null;
        for (OTP otp: read()) {
            if (systemID.equals(otp.getSystemID())) {
                result = otp;
                break;
            }
        }
        return result;
    }

    private OTP copyOTP(OTP otp) {
        OTP copy = new OTP();

        copy.setId(otp.getId());
        copy.setSystemID(otp.getSystemID());
        copy.setSystemName(otp.getSystemName());
        copy.setHash(otp.getHash());
        copy.setLuksKey(otp.getLuksKey());
        copy.setIterations(otp.getIterations());
        copy.setDateRegistration(otp.getDateRegistration());
        copy.setDateLastUsage(otp.getDateLastUsage());
        copy.setBits(otp.getBits());
        return copy;
    }
}
