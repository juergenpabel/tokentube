<%@include file="/WEB-INF/jsp/common/taglibs.jsp"%>

<s:layout-render name="/WEB-INF/jsp/common/layout_main.jsp"
  title="OTP Information">
  <s:layout-component name="contents">
    <s:messages/>
    <table class="view">
      <tr>
        <td class="label"><s:label for="otp.systemID"/>:</td>
        <td class="value">${actionBean.OTP.systemID}</td>
      </tr>
      <tr>
        <td class="label"><s:label for="otp.systemName"/>:</td>
        <td class="value">${actionBean.OTP.systemName}</td>
      </tr>
      <tr>
        <td class="label"><s:label for="otp.dateRegistration"/>:</td>
        <td class="value">
          <s:format value="${actionBean.OTP.dateRegistration}" formatPattern="yyyy-MM-dd HH:mm"/>
        </td>
      </tr>
      <tr>
        <td class="label"><s:label for="otp.hash"/>:</td>
        <td class="value">${actionBean.OTP.hash}</td>
      </tr>
      <tr>
        <td class="label"><s:label for="otp.bits"/>:</td>
        <td class="value">${actionBean.OTP.bits}</td>
      </tr>
      <tr>
        <td class="label">&nbsp;</td>
        <td class="value">&nbsp;</td>
      </tr>
      <tr>
        <td class="label"><s:label for="otp.iterations"/>:</td>
        <td class="value">${actionBean.OTP.iterations}</td>
      </tr>
      <tr>
        <td class="label"><s:label for="otp.dateLastUsage"/>:</td>
        <td class="value">
          <s:format value="${actionBean.OTP.dateLastUsage}" formatPattern="yyyy-MM-dd HH:mm"/>
        </td>
      </tr>
    </table>
    <p>
      <s:link beanclass="net.sf.tokentube.helpdesk.stripes.action.otp.GenerateActionBean">
        <s:param name="id" value="${actionBean.OTP}"/>
        Generate Response Code
      </s:link> |
      <s:link beanclass="net.sf.tokentube.helpdesk.stripes.action.otp.EditActionBean">
        <s:param name="id" value="${actionBean.OTP}"/>
        Edit
      </s:link> |
      <s:link beanclass="net.sf.tokentube.helpdesk.stripes.action.otp.DeleteActionBean">
        <s:param name="id" value="${actionBean.OTP}"/>
        Delete
      </s:link> |
      <s:link beanclass="net.sf.tokentube.helpdesk.stripes.action.otp.IndexActionBean">
        Back to List
      </s:link>
    </p>
  </s:layout-component>
</s:layout-render>
