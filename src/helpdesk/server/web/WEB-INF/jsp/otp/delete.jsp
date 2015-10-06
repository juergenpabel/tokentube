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
          <td><s:text name="OTP.systemID" class="required"/></td>
          <td><s:errors field="otp.systemID"/></td>
        </tr>
        <tr>
          <td><s:label for="otp.systemName"/>:</td>
          <td><s:text name="OTP.systemName" class="required"/></td>
          <td><s:errors field="otp.systemName"/></td>
        </tr>
        <tr>
          <td><s:label for="otp.iterations"/>:</td>
          <td><s:text name="OTP.iterations"/></td>
          <td><s:errors field="otp.iterations"/></td>
        </tr>
        <tr>
          <td><s:label for="otp.dateRegistration"/>:</td>
          <td><s:text name="OTP.dateRegistration" formatPattern="yyyy-MM-dd HH:mm"/></td>
          <td><s:errors field="otp.dateRegistration"/></td>
        </tr>
        <tr>
          <td><s:label for="otp.dateLastUsage"/>:</td>
          <td><s:text name="OTP.dateLastUsage" formatPattern="yyyy-MM-dd HH:mm"/></td>
          <td><s:errors field="otp.dateLastUsage"/></td>
        </tr>
        <tr>
          <td>&nbsp;</td>
          <td>
            <s:submit name="delete" value="Delete"/>
            <s:submit name="cancel" value="Cancel"/>
          </td>
        </tr>
      </table>
    </s:form>
  </s:layout-component>
</s:layout-render>
