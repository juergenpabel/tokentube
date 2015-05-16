<%@include file="/WEB-INF/jsp/common/taglibs.jsp"%>

<s:layout-render name="/WEB-INF/jsp/common/layout_main.jsp"
  title="OTP Information">
  <s:layout-component name="contents">
    <s:messages/>
    <s:form beanclass="${actionBean['class']}">
      <div><s:hidden name="id"/></div>
      <table class="form">
        <tr>
          <td><s:label for="otp.systemID"/>:</td>
          <td>${actionBean.OTP.systemID}</td>
        </tr>
        <tr>
          <td><s:label for="otp.systemName"/>:</td>
          <td>${actionBean.OTP.systemName}</td>
        </tr>
        <tr>
          <td><s:label for="otp.hash"/>:</td>
          <td>${actionBean.OTP.hash}</td>
        </tr>
        <tr>
          <td><s:label for="otp.bits"/>:</td>
          <td>${actionBean.OTP.bits}</td>
        </tr>
      <tr>
        <td class="label">&nbsp;</td>
        <td class="value">&nbsp;</td>
      </tr>
        <tr>
          <td><s:label for="otp.iterations"/>:</td>
          <td>${actionBean.OTP.iterations}</td>
        </tr>
        <tr>
          <td><s:label for="cr.challenge"/>:</td>
          <td>
            <c:set var="x" value="1"/>
            <c:set var="bits" value="${actionBean.OTP.bits}"/>

            <s:text name="cr.text1" size="4" maxlength="4"/>
            <%
                for(int i=0; i<Integer.valueOf(pageContext.getAttribute("bits").toString())/16; i++) {
            %>
                    - <s:text name="cr.text${x+1}" size="4" maxlength="4"/>
                    <c:set var="x" value="${x+1}"/>
            <%
                }
            %>
          </td>
          <td><s:errors field="cr.text1"/></td>
          <td><s:errors field="cr.text2"/></td>
          <td><s:errors field="cr.text3"/></td>
          <td><s:errors field="cr.text4"/></td>
          <td><s:errors field="cr.text5"/></td>
          <td><s:errors field="cr.text6"/></td>
          <td><s:errors field="cr.text7"/></td>
          <td><s:errors field="cr.text8"/></td>
        </tr>
        <tr>
          <td><s:label for="cr.response"/>:</td>
          <td>${actionBean.OTP.challengeResult}</td>
        </tr>
        <tr>
          <td>&nbsp;</td>
          <td>
            <s:submit name="generate" value="Generate"/>
            <s:submit name="done" value="Done"/>
          </td>
        </tr>
      </table>
    </s:form>
  </s:layout-component>
</s:layout-render>
