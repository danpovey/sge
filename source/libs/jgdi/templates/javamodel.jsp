/*___INFO__MARK_BEGIN__*/
/*************************************************************************
 *
 *  The Contents of this file are made available subject to the terms of
 *  the Sun Industry Standards Source License Version 1.2
 *
 *  Sun Microsystems Inc., March, 2001
 *
 *
 *  Sun Industry Standards Source License Version 1.2
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.2 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://gridengine.sunsource.net/Gridengine_SISSL_license.html
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *   The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *   Copyright: 2001 by Sun Microsystems, Inc.
 *
 *   All Rights Reserved.
 *
 ************************************************************************/
/*___INFO__MARK_END__*/
/**
 *  Generated from javamodel.jsp
 *  !!! DO NOT EDIT THIS FILE !!!
 */
<%
  final com.sun.grid.cull.CullDefinition cullDef = (com.sun.grid.cull.CullDefinition)params.get("cullDef");
  final com.sun.grid.cull.JavaHelper jh = (com.sun.grid.cull.JavaHelper)params.get("javaHelper");
  final com.sun.grid.cull.CullObject cullObj = (com.sun.grid.cull.CullObject)params.get("cullObj");
  com.sun.grid.cull.CullAttr   attr = null;
  
  final String classname = jh.getClassName(cullObj);
%>
package <%=jh.getPackageName()%>;

import com.sun.grid.jgdi.configuration.GEObject;
import java.util.List;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Map;
import java.util.Set;
import java.util.Iterator;
import java.util.HashMap;
import java.util.logging.Logger;
import java.util.logging.Level;

/**
 *  Java representation of the cull object <%=cullObj.getName()%>
 *  defined in <%=jh.getSource(cullObj).getName()%>
 *
 *  <p><strong>Warning:</strong>  The  <%=classname%> class will 
 *  not be compatible with future Sun&trade; Grid Engine releases.</p>
 *
 */
public interface <%=classname%> extends <%

  if(cullObj.getParentObject() != null) {
%>  <%=jh.getClassName(cullObj.getParentObject())%> <%
  } else {%>
    GEObject <% 
    } 
%> { 
<%
  // Iterator over all attributes and create member variable, 
    // getter and setter
  for(int i = 0; i < cullObj.getAttrCount(); i++ ) {

   attr = cullObj.getAttr(i);
   String attrType = jh.getFullClassName(attr.getType());
   String attrName = jh.getAttrName(attr);

   String gsname =  Character.toUpperCase( attrName.charAt(0) ) +
                   attrName.substring(1);

%>
   // Attribute <%=attrName%> ------------------------------------------------
<%
   if( attr instanceof com.sun.grid.cull.CullMapListAttr ) {
   /* ------------------------------------------------------------------------
    * ---------------------- MapList Attribute -----------------------------------
    * ------------------------------------------------------------------------
    */ 
       if( attrName.endsWith("List") ) {          
          attrName  = attrName.substring(0, attrName.length() - 4 );
          gsname =  Character.toUpperCase( attrName.charAt(0) ) +
                   attrName.substring(1);
          
       }
       com.sun.grid.cull.CullMapAttr mapAttr = (com.sun.grid.cull.CullMapAttr)attr;
       com.sun.grid.cull.CullObject subobj = cullDef.getCullObject(mapAttr.getType());


       com.sun.grid.cull.CullAttr keyAttr = mapAttr.getKeyAttr();
       com.sun.grid.cull.CullAttr valueAttr = mapAttr.getValueAttr();

       String keyClassName = jh.getClassName(keyAttr.getType());
       String valueClassName = jh.getClassName(valueAttr.getType());
       String fullValueClassName = jh.getClassName(valueAttr.getType());
       
       String defaultKey = mapAttr.getDefaultKey();
       if( defaultKey == null ) {
           defaultKey = "default";
       }
%>      

   /**
    *  Get the value of <code><%=mapAttr.getKeyName()%></code> at <code>index</code>.
    *  @param <%=mapAttr.getKeyName()%> the <%=mapAttr.getKeyName()%>
    *  @param index the index
    */
   public <%=valueClassName%> get<%=gsname%>(<%=keyClassName%> <%=mapAttr.getKeyName()%>, int index);
   
   /**
    *  Get the number of values for <code><%=mapAttr.getKeyName()%></code>
    *  @param <%=mapAttr.getKeyName()%> the <%=mapAttr.getKeyName()%>
    */
   public int get<%=gsname%>Count(<%=keyClassName%> <%=mapAttr.getKeyName()%>);
   
   /**
    *   Get a unmodifiable set of stored <%=attrName%> attribute keys.
    *   @return unmodifiable set of stored <%=attrName%> attribute keys
    */
   public Set get<%=gsname%>Keys();
   
   /**
    *  <p>Get the default value of the attribute <%=attrName%>.
    *  (<%=mapAttr.getDefaultKey()%>)</p>
    *  @return the default value of the attribute <%=attrName%>
    *  @throws  java.lang.IllegalStateException if the default value is not set
    */
   public <%=valueClassName%> getDefault<%=gsname%>(int index);
   
   /**
    *  Get the number of values for  the default <code><%=mapAttr.getKeyName()%></code>
    *  (<%=mapAttr.getDefaultKey()%>).
    */
   public int getDefault<%=gsname%>Count();
   
   
   
   /**
    *  Get an unmodifiable list of <%=attrName%> attributes for 
    *  <code><%=mapAttr.getKeyName()%></code>.
    *
    *  @param <%=mapAttr.getKeyName()%> the <%=mapAttr.getKeyName()%>
    *  @return unmodifiable list of <%=attrName%> attributes
    */
   public List get<%=gsname%>List(<%=keyClassName%> <%=mapAttr.getKeyName()%>);
   
<%                                                                              
   if (!attr.isReadOnly()) {                                                       
%> 
   /**
    *  Add <code><%=mapAttr.getValueName()%></code> attribute to <code><%=mapAttr.getKeyName()%></code>.
    *
    *  @param <%=mapAttr.getKeyName()%>  the <%=mapAttr.getKeyName()%>
    *  @param <%=mapAttr.getValueName()%> the <%=mapAttr.getValueName()%> attribute
    */
   public void add<%=gsname%>(<%=keyClassName%> <%=mapAttr.getKeyName()%>, <%=valueClassName%> <%=mapAttr.getValueName()%>);

   /**
    *  Initialize <code><%=mapAttr.getValueName()%></code> attribute for a <code><%=mapAttr.getKeyName()%></code> 
    *  with an empty list.
    *
    *  @param <%=mapAttr.getKeyName()%>  the <%=mapAttr.getKeyName()%>
    */
   public void addEmpty<%=gsname%>(<%=keyClassName%> <%=mapAttr.getKeyName()%>);
   
   
   /**
    *  Set <code><%=mapAttr.getValueName()%></code> attribute for <code><%=mapAttr.getKeyName()%></code>.
    *
    *  @param <%=mapAttr.getKeyName()%>  the <%=mapAttr.getKeyName()%>
    *  @param index  index of the <%=mapAttr.getValueName()%> attribute
    *  @param <%=mapAttr.getValueName()%>  the <%=mapAttr.getValueName()%> attribute
    */
   public void set<%=gsname%>(<%=keyClassName%> <%=mapAttr.getKeyName()%>, int index, <%=valueClassName%> <%=mapAttr.getValueName()%>);
   
   /**
    *  Remove <code><%=mapAttr.getValueName()%></code> attribute for <code><%=mapAttr.getKeyName()%></code>.
    *
    *  @param <%=mapAttr.getKeyName()%>  the <%=mapAttr.getKeyName()%>
    *  @param index the index of the <%=mapAttr.getValueName()%> attribute
    *  @return the remove <%=mapAttr.getValueName()%> attribute of <code>null</code>
    *          if the attribute has not been found
    */
   public Object remove<%=gsname%>At(<%=keyClassName%> <%=mapAttr.getKeyName()%>, int index);
   
   /**
    *  Remove <code><%=mapAttr.getValueName()%></code> attribute for <code><%=mapAttr.getKeyName()%></code>.
    *
    *  @param <%=mapAttr.getKeyName()%>  the <%=mapAttr.getKeyName()%>
    *  @param <%=mapAttr.getValueName()%> the <%=mapAttr.getValueName()%>) attribute
    *  @return <code>true</code> if the attribute has been removed
    */
   public boolean remove<%=gsname%>(<%=keyClassName%> <%=mapAttr.getKeyName()%>, <%=valueClassName%> <%=mapAttr.getValueName()%>);
   
   /**
    *  Remove all <code><%=mapAttr.getValueName()%></code> attributes for <code><%=mapAttr.getKeyName()%></code>.
    *
    *  @param <%=mapAttr.getKeyName()%> the <%=mapAttr.getKeyName()%>
    */   
   public void removeAll<%=gsname%>(<%=keyClassName%> <%=mapAttr.getKeyName()%>);
   
   // default methods
   
   /**
    *  Add <code><%=mapAttr.getValueName()%></code> attribute to 
    *  the default <code><%=mapAttr.getKeyName()%></code>  (<%=mapAttr.getDefaultKey()%>).
    *
    *  @param <%=mapAttr.getValueName()%> the <%=mapAttr.getValueName()%> attribute
    */
   public void addDefault<%=gsname%>(<%=valueClassName%> <%=mapAttr.getValueName()%>);
   
   /**
    *  Set <code><%=mapAttr.getValueName()%></code> attribute for the default
    *  <code><%=mapAttr.getKeyName()%></code> (<%=mapAttr.getDefaultKey()%>).
    *
    *  @param index  index of the <%=mapAttr.getValueName()%> attribute
    *  @param <%=mapAttr.getValueName()%>  the <%=mapAttr.getValueName()%> attribute
    */
   public void setDefault<%=gsname%>(int index, <%=valueClassName%> <%=mapAttr.getValueName()%>);
   
   /**
    *  Remove <code><%=mapAttr.getValueName()%></code> attribute for the
    *  default <code><%=mapAttr.getKeyName()%></code> (<%=mapAttr.getDefaultKey()%>).
    *
    *  @param <%=mapAttr.getKeyName()%>  the <%=mapAttr.getKeyName()%>
    *  @param index the index of the <%=mapAttr.getValueName()%> attribute
    *  @return the remove <%=mapAttr.getValueName()%> attribute of <code>null</code>
    *          if the attribute has not been found
    */
   public Object removeDefault<%=gsname%>At(<%=keyClassName%> <%=mapAttr.getKeyName()%>, int index);
   
   /**
    *  Remove <code><%=mapAttr.getValueName()%></code> attribute from the
    *  default <code><%=mapAttr.getKeyName()%></code> (<%=mapAttr.getDefaultKey()%>).
    *
    *  @param <%=mapAttr.getValueName()%> the <%=mapAttr.getValueName()%>) attribute
    *  @return <code>true</code> if the attribute has been removed
    */
   public boolean removeDefault<%=gsname%>(<%=keyClassName%> <%=mapAttr.getKeyName()%>, <%=valueClassName%> <%=mapAttr.getValueName()%>);

   /**
    *  Remove all <%=mapAttr.getValueName()%>) attributes from 
    *  the default <code><%=mapAttr.getKeyName()%></code> (<%=mapAttr.getDefaultKey()%>).
    */
   public void removeAllDefault<%=gsname%>();
   
   /**
    *  Remove all <%=mapAttr.getValueName()%>) attributes.
    */
   public void removeAll<%=gsname%>();
   
   
<%
   }
%>
    
<%   } else if( attr instanceof com.sun.grid.cull.CullMapAttr) { 
   /* ------------------------------------------------------------------------
    * ---------------------- Map Attribute -----------------------------------
    * ------------------------------------------------------------------------
    */ 
      
       if( attrName.endsWith("List") ) {          
          attrName  = attrName.substring(0, attrName.length() - 4 );
          gsname =  Character.toUpperCase( attrName.charAt(0) ) +
                   attrName.substring(1);
          
       }
      
       com.sun.grid.cull.CullMapAttr mapAttr = (com.sun.grid.cull.CullMapAttr)attr;
       com.sun.grid.cull.CullObject subobj = cullDef.getCullObject(mapAttr.getType());


       com.sun.grid.cull.CullAttr keyAttr = mapAttr.getKeyAttr();
       com.sun.grid.cull.CullAttr valueAttr = mapAttr.getValueAttr();

       String keyClassName = jh.getClassName(keyAttr.getType());
%>
      // keyClassName for <%=keyAttr.getType()%> = <%=keyClassName%>
<%
       String valueClassName = jh.getClassName(valueAttr.getType());
       String fullValueClassName = jh.getFullClassNameOrWrapper(valueAttr.getType());
       
       String defaultKey = mapAttr.getDefaultKey();
       if( defaultKey == null ) {
           defaultKey = "default";
       }
%>           
   
   /**
    *  <p>Get the default value of the attribute <%=attrName%>.</p>
    *  @return the default value of the attribute <%=attrName%>
    *  @throws  java.lang.IllegalStateException if the default value is not set
    */
   public <%=valueClassName%> getDefault<%=gsname%>();

   /**
    *  <p>Get the <%=attrName%> attribute for <code><%=mapAttr.getKeyName()%></code>.</p>
    *  @param   <%=mapAttr.getKeyName()%>   the <%=mapAttr.getKeyName()%>
    *  @return  the <%=attrName%> attribute for <code><%=mapAttr.getKeyName()%></code> or the
    *           default value of  <%=attrName%>
    */
   public <%=valueClassName%> get<%=gsname%>(String <%=mapAttr.getKeyName()%>);
   
   /**
    *  Get the set of <code><%=mapAttr.getKeyName()%></code>s which have the attribute <%=attrName%> set.
    *  @return set of <code><%=mapAttr.getKeyName()%></code>s
    */
   public Set get<%=gsname%>Keys();
   
   /**
    *  Get the number of <%=mapAttr.getKeyName()%>s which have the attribute <%=attrName%> set.
    *  @return the number of <%=mapAttr.getKeyName()%>s which have the attribute <%=attrName%> set.
    */
   public int get<%=gsname%>Count();
   
   /**
    *  Determine if any <%=attrName%> attribute is for <code><%=mapAttr.getKeyName()%></code> is set
    *  @param  <%=mapAttr.getKeyName()%> the <%=mapAttr.getKeyName()%>
    *  @return <code>true</code> if the <%=attrName%> attribute is set
    */
   public boolean isSet<%=gsname%>(String <%=mapAttr.getKeyName()%>);
   
   
<%                                                                              
   if (!attr.isReadOnly()) {                                                       
%>          
   /**
    *  <p>Put the attribute <%=attrName%> for <code><%=mapAttr.getKeyName()%></code>.</p>
    *
    *  @param <%=mapAttr.getKeyName()%>   the <%=mapAttr.getKeyName()%>
    *  @param <%=mapAttr.getValueName()%> the <%=mapAttr.getValueName()%>
    */
   public void put<%=gsname%>(<%=keyClassName%> <%=mapAttr.getKeyName()%>, <%=valueClassName%> <%=mapAttr.getValueName()%>);

   /**
    *  <p>remove the attribute <%=attrName%> for <code><%=mapAttr.getKeyName()%></code>.</p>
    *
    *  @param <%=mapAttr.getKeyName()%>  The <%=mapAttr.getKeyName()%>
    *  @return the value of the attribute for <code><%=mapAttr.getKeyName()%></code> or <code>null</code>
    *          if the attribute was not defined for <code><%=mapAttr.getKeyName()%></code>.
    */
   public <%=fullValueClassName%> remove<%=gsname%>(String <%=mapAttr.getKeyName()%>);
   
   /**
    *  <p>Remove the attribute <%=attrName%> for all <%=mapAttr.getKeyName()%>s.</p>
    */
   public void removeAll<%=gsname%>();
   
<%
   }
%>  

<% // end of Map Attribute  
   } else if( attr instanceof com.sun.grid.cull.CullListAttr ) {
   /* ------------------------------------------------------------------------
    * ---------------------- List Attribute ----------------------------------
    * ------------------------------------------------------------------------
    */
    if( attrName.endsWith("List") ) {          
       attrName  = attrName.substring(0, attrName.length() - 4 );
       gsname =  Character.toUpperCase( attrName.charAt(0) ) +
                attrName.substring(1);

    }
      
%>

   /**
    *   Get a unmodifiable list of all <%=attrType%> attributes.
    *
    *   @return Unmodifiable list with all <%=attrType%> attributes
    */
   public List get<%=gsname%>List();

   /**
    *  Get the number of <%=attrType%> attributes.
    *
    *  @return The number of <%=attrType%> attributes.
    */
   public int get<%=gsname%>Count();
 
   /**
    *  Get a <%=attrType%> attribute at an index.
    *  @param index  the index of the <%=attrType%> attribute which should be
    *                removed
    *  @return the <%=attrType%> attribute
    */
   public <%=attrType%> get<%=gsname%>(int index);
   
<% 
    if (!attr.isReadOnly()) {
%>       
   /**
    *   Add a <%=attrType%> attribute.
    *   @param a<%=attrName%>  the new <%=attrType%> attribute
    */
   public void add<%=gsname%>(<%=attrType%> a<%=attrName%>);
   
   /**
    *   Set a <%=attrType%> attribute.
    *   @param index   index of the <%=attrType%> attribute
    *   @param a<%=attrName%>  the new <%=attrType%> attribute
    */
   public void set<%=gsname%>(int index, <%=attrType%> a<%=attrName%>);

   /**
    *  Remove all <%=attrType%> attributes.
    */
   public void removeAll<%=gsname%>();
   
   /**
    *  Remote a <%=attrType%> attribute at an index.
    *  @param index  the index of the <%=attrType%> attribute which should be
    *                removed
    *  @return the removed <%=attrType%> attribute or <code>null</code> if
    *          no attribute at this index is stored
    */
   public <%=attrType%> remove<%=gsname%>(int index);

   /**
    *  Remote a specific <%=attrType%> attribute.
    *  @param a<%=attrName%>  the <%=attrType%> attribute which should be
    *                         removed
    *  @return <code>true</code> if the <%=attrType%> attribute has been removed
    */
   public boolean remove<%=gsname%>(<%=attrType%> a<%=attrName%>);
   
<% 
    }
%>       
   
   
<% // end of List Attribute
   } else {
   /* ------------------------------------------------------------------------
    * ---------------------- Simple Attribute ----------------------------------
    * ------------------------------------------------------------------------
    */
   %>
   
<% 
    if (!attr.isReadOnly()) {
%>       
   /**
    *   Set the <%=attrName%> attribute.
    *
    *   @param a<%=gsname%>  the new value for the  <%=attrName%> attribute
    */
   public void set<%=gsname%>(<%=attrType%> a<%=gsname%>);
<%
    }
%>  
   /**
    *  Get the value of the <%=attrName%> attribute.
    *  @return the value of the <%=attrName%> attribute
    */
   public <%=attrType%> <%=attrType.endsWith("oolean")?"is":"get"%><%=gsname%>();
   

<% } // end of else normal attributes
   
   // Methods which are common to all attribute types
%>   
   /**
    *  Determine if any <%=attrName%> attribute is set
    *  @return <code>true</code> if the <%=attrName%> attribute is set
    */
   public boolean isSet<%=gsname%>();

<%  } // end of for of attribute %>

  <%//SPECIAL CODE for COMPLEX ENTRY
  if (classname.equals("ComplexEntry")) {
     %>
    public String typeToString(int type);
    public int typeToInt(String type);
    public String opToString(int op);
    public int opToInt(String type);
    public String reqToString(int op);
    public int reqToInt(String type);
    <%
  }%>

}
