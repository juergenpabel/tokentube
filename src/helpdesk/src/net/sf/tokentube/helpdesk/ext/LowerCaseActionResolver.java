package net.sf.tokentube.helpdesk.stripes.ext;

import net.sourceforge.stripes.controller.NameBasedActionResolver;

public class LowerCaseActionResolver extends NameBasedActionResolver {
  //Override
  protected String getBindingSuffix() {
    return "";
  }

  protected String getUrlBinding(String actionBeanName) {
    return super.getUrlBinding(actionBeanName).toLowerCase();
  }

}

