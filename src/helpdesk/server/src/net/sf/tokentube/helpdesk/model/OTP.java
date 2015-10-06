package net.sf.tokentube.helpdesk.stripes.model;

import java.util.Date;

public class OTP {
    private Integer id;
    private String  systemID;
    private String  systemName;
    private String  luksKey;
    private String  otpHash;
    private Integer otpBits;
    private Integer otpIterations;
    private Date    dateRegistration;
    private Date    dateLastUsage;
    private String  challengeResult;

    public Integer getId() {
        return id;
    }
    public void setId(Integer id) {
        this.id = id;
    }
    public String getSystemID() {
        return systemID;
    }
    public void setSystemID(String systemID) {
        this.systemID = systemID;
    }
    public String getSystemName() {
        return systemName;
    }
    public void setSystemName(String systemName) {
        this.systemName = systemName;
    }
    public String getLuksKey() {
        return luksKey;
    }
    public void setLuksKey(String luksKey) {
        this.luksKey = luksKey;
    }
    public Integer getIterations() {
        return otpIterations;
    }
    public void incrementIterations() {
        this.otpIterations = new Integer(otpIterations.intValue()+1);
    }
    public void setIterations(Integer otpIterations) {
        this.otpIterations = otpIterations;
    }
    public Date getDateRegistration() {
        return this.dateRegistration;
    }
    public void setDateRegistration(Date dateRegistration) {
        this.dateRegistration = dateRegistration;
    }
    public Date getDateLastUsage() {
        return this.dateLastUsage;
    }
    public void setDateLastUsage(Date dateLastUsage) {
        this.dateLastUsage = dateLastUsage;
    }
    public String getChallengeResult() {
        String challengeResult = this.challengeResult;

        this.challengeResult = null;
        return challengeResult;
    }
    public void setChallengeResult(String challengeResult) {
        this.challengeResult = challengeResult;
    }
    public String getHash() {
        return this.otpHash;
    }
    public void setHash(String otpHash) {
        this.otpHash = otpHash;
    }
    public Integer getBits() {
        return this.otpBits;
    }
    public void setBits(Integer otpBits) {
        this.otpBits = otpBits;
    }

    @Override
    public boolean equals(Object obj) {
        try { return id.equals(((OTP) obj).getId()); }
        catch (Exception exc) { return false; }
    }
    @Override
    public int hashCode() {
        return 31 + ((id == null) ? 0 : id.hashCode());
    }
    @Override
    public String toString() {
        return String.format("%s %s", systemID, systemName);
    }
}
