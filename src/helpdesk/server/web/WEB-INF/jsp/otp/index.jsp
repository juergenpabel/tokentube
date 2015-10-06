<%@include file="/WEB-INF/jsp/common/taglibs.jsp"%>

<s:layout-render name="/WEB-INF/jsp/common/layout_main.jsp"
  title="OTP List">
  <s:layout-component name="contents">
    <s:messages/>
    <s:link beanclass="net.sf.tokentube.helpdesk.stripes.action.otp.EditActionBean">
      Create a New OTP
    </s:link>
    <d:table name="${actionBean.OTPs}" id="id" requestURI="" defaultsort="1">
      <d:column title="Asset-ID" property="systemID" sortable="true"/>
      <d:column title="Hostname" property="systemName" sortable="true"/>
      <d:column title="OTP Iterations" property="iterations" sortable="true"/>
      <d:column title="Action">
        <s:link beanclass="net.sf.tokentube.helpdesk.stripes.action.otp.GenerateActionBean">
          <s:param name="id" value="${id}"/>
          Generate Response Code
        </s:link>
        <br/>
        <s:link beanclass="net.sf.tokentube.helpdesk.stripes.action.otp.ViewActionBean">
          <s:param name="id" value="${id}"/>
          View
        </s:link> |
        <s:link beanclass="net.sf.tokentube.helpdesk.stripes.action.otp.EditActionBean">
          <s:param name="id" value="${id}"/>
          Edit
        </s:link> |
        <s:link beanclass="net.sf.tokentube.helpdesk.stripes.action.otp.DeleteActionBean">
          <s:param name="id" value="${id}"/>
          Delete
        </s:link>
      </d:column>
    </d:table>
  </s:layout-component>
</s:layout-render>
