<%@page contentType="text/html;charset=ISO-8859-1" language="java"%>
<%@include file="/WEB-INF/jsp/common/taglibs.jsp"%>

<s:layout-definition>
  <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
    "http://www.w3.org/TR/html4/strict.dtd">
  <html>
    <head>
      <title>${title}</title>
      <link rel="stylesheet" type="text/css"
        href="${contextPath}/css/style.css">
    </head>
    <body>
      <div id="header">
        <span class="title">${title}</span>
      </div>
      <div id="body">
        <s:layout-component name="contents"/>
      </div>
    </body>
  </html>
</s:layout-definition>
