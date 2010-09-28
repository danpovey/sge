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
#include <sys/types.h>
#include <string.h>

#include "sge_gdiP.h"
#include "sge_any_request.h"
#include "commlib.h"
#include "sge_prog.h"
#include "sgermon.h"
#include "sge_log.h"
#include "sge_profiling.h"
#include "qm_name.h"
#include "pack.h"
#include "sge_feature.h"
#include "sge_security.h"
#include "sge_unistd.h"
#include "sge_string.h"
#include "sge_hostname.h"
#include "sge_bootstrap.h"
#include "sge_mtutil.h"
#include "msg_gdilib.h"
#include "sgeobj/sge_answer.h"

static pthread_mutex_t check_alive_mutex = PTHREAD_MUTEX_INITIALIZER;

static int gdi_log_flush_func(cl_raw_list_t* list_p);

/* setup a communication error callback */
static void general_communication_error(int cl_err);
static int gdi_general_communication_error = CL_RETVAL_OK;

static int gdi_log_flush_func(cl_raw_list_t* list_p) {
   int ret_val;
   cl_log_list_elem_t* elem = NULL;
   DENTER(COMMD_LAYER, "gdi_log_flush_func");

   if (list_p == NULL) {
      DEXIT;
      return CL_RETVAL_LOG_NO_LOGLIST;
   }

   if (  ( ret_val = cl_raw_list_lock(list_p)) != CL_RETVAL_OK) {
      DEXIT;
      return ret_val;
   }

   while ( (elem = cl_log_list_get_first_elem(list_p) ) != NULL) {
      char* param;
      char* module;
      if (elem->log_parameter == NULL) {
         param = "";
      } else {
         param = elem->log_parameter;
      }
      if (elem->log_module_name == NULL) {
         module = "";
      } else {
         module = elem->log_module_name;
      }
      switch(elem->log_type) {
         case CL_LOG_ERROR: 
            if ( log_state_get_log_level() >= LOG_ERR) {
               ERROR((SGE_EVENT,  "%s %-20s=> %s %s\n", elem->log_module_name, elem->log_thread_name, elem->log_message, param ));
            } else {
               printf("%s %-20s=> %s %s\n", elem->log_module_name, elem->log_thread_name, elem->log_message, param);
            }
            break;
         case CL_LOG_WARNING:
            if ( log_state_get_log_level() >= LOG_WARNING) {
               WARNING((SGE_EVENT,"%s %-20s=> %s %s\n", elem->log_module_name, elem->log_thread_name, elem->log_message, param ));
            } else {
               printf("%s %-20s=> %s %s\n", elem->log_module_name, elem->log_thread_name, elem->log_message, param);
            }
            break;
         case CL_LOG_INFO:
            if ( log_state_get_log_level() >= LOG_INFO) {
               INFO((SGE_EVENT,   "%s %-20s=> %s %s\n", elem->log_module_name, elem->log_thread_name, elem->log_message, param ));
            } else {
               printf("%s %-20s=> %s %s\n", elem->log_module_name, elem->log_thread_name, elem->log_message, param);
            }
            break;
         case CL_LOG_DEBUG:
            if ( log_state_get_log_level() >= LOG_DEBUG) { 
               DEBUG((SGE_EVENT,  "%s %-20s=> %s %s\n", elem->log_module_name, elem->log_thread_name, elem->log_message, param ));
            } else {
               printf("%s %-20s=> %s %s\n", elem->log_module_name, elem->log_thread_name, elem->log_message, param);
            }
            break;
      }
      cl_log_list_del_log(list_p);
   }
   
   if (  ( ret_val = cl_raw_list_unlock(list_p)) != CL_RETVAL_OK) {
      DEXIT;
      return ret_val;
   } 
   DEXIT;
   return CL_RETVAL_OK;
}

/****** sge_any_request/general_communication_error() **************************
*  NAME
*     general_communication_error() -- callback for communication errors
*
*  SYNOPSIS
*     static void general_communication_error(int cl_error) 
*
*  FUNCTION
*     This function is used by cl_com_set_error_func() to set the default
*     application error function for communication errors. On important 
*     communication errors the communication lib will call this function
*     with a corresponding error number.
*     This function should never block. Treat it as a kind of signal handler.
*
*  INPUTS
*     int cl_error - commlib error number
*
*  NOTES
*     MT-NOTE: general_communication_error() is not MT safe 
*     (static variable "gdi_general_communication_error" is used)
*
*  SEE ALSO
*     sge_any_request/sge_get_communication_error()
*******************************************************************************/
static void general_communication_error(int cl_error) {
   DENTER(COMMD_LAYER, "general_communication_error");
   DPRINTF((MSG_GDI_GENERAL_COM_ERROR_S, cl_get_error_text(cl_error)));
   gdi_general_communication_error = cl_error;
   DEXIT;
}

/****** sge_any_request/sge_get_communication_error() **************************
*  NAME
*     sge_get_communication_error() -- get last communication error
*
*  SYNOPSIS
*     int sge_get_communication_error(void) 
*
*  FUNCTION
*     This function returns the last communication error. This procedure returns
*     the last set communication error by communication lib callback.
*
*  RESULT
*     int - last communication error
*
*  NOTES
*     MT-NOTE: sge_get_communication_error() is not MT safe ( returns just 
*     an static defined integer) but can be called by more threads without 
*     problem.
*
*  SEE ALSO
*     sge_any_request/general_communication_error()
*******************************************************************************/
int sge_get_communication_error(void) {
   int com_error = gdi_general_communication_error;
   DENTER(COMMD_LAYER, "sge_get_communication_error");
   if ( gdi_general_communication_error != CL_RETVAL_OK) {
      WARNING((SGE_EVENT, MSG_GDI_GENERAL_COM_ERROR_S, cl_get_error_text(com_error)));
      gdi_general_communication_error = CL_RETVAL_OK;
   }
   DEXIT;
   return com_error;
}

/*-----------------------------------------------------------------------
 * prepare_enroll
 * just store values for later enroll() of commlib
 *
 * NOTES
 *    MT-NOTE: prepare_enroll() is MT safe
 *-----------------------------------------------------------------------*/
void prepare_enroll(const char *name)
{
   int ret_val;
   u_long32 me_who;
   cl_com_handle_t* handle = NULL;
   cl_host_resolve_method_t resolve_method = CL_SHORT;
   const char* default_domain = NULL;


   DENTER(TOP_LAYER, "prepare_enroll");

   if (sge_security_initialize(name)) {
      CRITICAL((SGE_EVENT, MSG_GDI_INITSECURITYDATAFAILED));
      SGE_EXIT(1);
   }
   
   /* TODO: activate mutlithreaded communication for SCHEDD and EXECD !!!
            This can only by done when the daemonize functions of SCHEDD and EXECD
            are thread save and reresolve qualified hostname for each thread */

   if ( uti_state_get_mewho() == QMASTER || uti_state_get_mewho() == QMON 
        /* || uti_state_get_mewho() == EXECD || uti_state_get_mewho() == SCHEDD */ ) {
      INFO((SGE_EVENT,MSG_GDI_MULTI_THREADED_STARTUP));
      ret_val = cl_com_setup_commlib(CL_ONE_THREAD,CL_LOG_OFF,gdi_log_flush_func);
   } else {
      INFO((SGE_EVENT,MSG_GDI_SINGLE_THREADED_STARTUP));
      ret_val = cl_com_setup_commlib(CL_NO_THREAD,CL_LOG_OFF,gdi_log_flush_func);
   }
   if (ret_val != CL_RETVAL_OK) {
      ERROR((SGE_EVENT, cl_get_error_text(ret_val)) );
   }
 
   /* set alias file */
   {
      char *alias_path = sge_get_alias_path();
      ret_val = cl_com_set_alias_file(alias_path);
      if (ret_val != CL_RETVAL_OK) {
         ERROR((SGE_EVENT, cl_get_error_text(ret_val)) );
      }
      FREE(alias_path);
   }

   /* set hostname resolve (compare) method */
   if (bootstrap_get_ignore_fqdn() == false) {
      resolve_method = CL_LONG;
   } 
   if ( bootstrap_get_default_domain() != NULL && SGE_STRCASECMP(bootstrap_get_default_domain(), "none") != 0) {
      default_domain = bootstrap_get_default_domain();
   }
   ret_val = cl_com_set_resolve_method(resolve_method, (char*)default_domain);

   if (ret_val != CL_RETVAL_OK) {
      ERROR((SGE_EVENT, cl_get_error_text(ret_val)) );
   }


   ret_val = cl_com_set_error_func(general_communication_error);
   if (ret_val != CL_RETVAL_OK) {
      ERROR((SGE_EVENT, cl_get_error_text(ret_val)) );
   }

   me_who = uti_state_get_mewho();

   handle = cl_com_get_handle((char*)uti_state_get_sge_formal_prog_name() ,0);
   if (handle == NULL) {
      int my_component_id = 0; /* 1 for daemons, 0=automatical for clients */
      int execd_port = 0;
      if ( me_who == QMASTER ||
           me_who == EXECD   ||
           me_who == SCHEDD  || 
           me_who == SHADOWD ) {
         my_component_id = 1;   
      }

      switch(me_who) {
         case EXECD:
            /* add qmaster as known endpoint */
            cl_com_append_known_endpoint_from_name((char*)sge_get_master(gdi_state_get_reread_qmaster_file()), (char*)prognames[QMASTER], 1 ,sge_get_qmaster_port(),CL_CM_AC_DISABLED ,1);
            execd_port = sge_get_execd_port(); 
            handle = cl_com_create_handle(CL_CT_TCP, CL_CM_CT_MESSAGE, 1,execd_port ,
                                          (char*)prognames[uti_state_get_mewho()], my_component_id , 1 , 0 );
            cl_com_set_auto_close_mode(handle, CL_CM_AC_ENABLED );
            if (handle == NULL) {
               CRITICAL((SGE_EVENT, MSG_GDI_CANT_GET_EXECD_HANDLE_SUU, 
                         (char*) prognames[uti_state_get_mewho()],
                          u32c(my_component_id), 
                          u32c(execd_port)));
            }
            break;
         case QMASTER:
            DPRINTF(("creating QMASTER handle\n"));
            handle = cl_com_create_handle(CL_CT_TCP, CL_CM_CT_MESSAGE,                              /* message based tcp communication                */
                                          1, sge_get_qmaster_port(),                                /* create service on qmaster port,                */
                                                                                                    /* use execd port to connect to endpoints         */
                                          (char*)prognames[uti_state_get_mewho()], my_component_id, /* this endpoint is called "qmaster" and has id 1 */
                                          1 , 0 );                                                  /* select timeout is set to 1 second 0 usec       */
            if (handle == NULL) {
               CRITICAL((SGE_EVENT,MSG_GDI_CANT_CREATE_COM_HANDLE));
            }
            break;
         case QMON:
            DPRINTF(("creating QMON GDI handle\n"));
            handle = cl_com_create_handle(CL_CT_TCP, CL_CM_CT_MESSAGE, 0, sge_get_qmaster_port(),
                                         (char*)prognames[uti_state_get_mewho()], my_component_id , 1 , 0 );
            cl_com_set_auto_close_mode(handle, CL_CM_AC_ENABLED );
            if (handle == NULL) {
               CRITICAL((SGE_EVENT,MSG_GDI_CANT_CREATE_COM_HANDLE));
            }
            break;

         default:
            /* this is for "normal" gdi clients of qmaster */
            DPRINTF(("creating GDI handle\n"));
            handle = cl_com_create_handle(CL_CT_TCP, CL_CM_CT_MESSAGE, 0, sge_get_qmaster_port(),
                                         (char*)prognames[uti_state_get_mewho()], my_component_id , 1 , 0 );
            if (handle == NULL) {
               CRITICAL((SGE_EVENT,MSG_GDI_CANT_CREATE_COM_HANDLE));
            }
            break;
      }

      if (handle != NULL) {
         INFO((SGE_EVENT, MSG_GDI_HANDLE_CREATED_FOR_S, uti_state_get_sge_formal_prog_name() ));
      }
   } 
   DEXIT;
}



/*---------------------------------------------------------
 *  sge_send_any_request
 *  returns 0 if ok
 *          -4 if peer is not alive or rhost == NULL
 *          return value of gdi_send_message() for other errors
 *
 *  NOTES
 *     MT-NOTE: sge_send_gdi_request() is MT safe (assumptions)
 *---------------------------------------------------------*/
int sge_send_any_request(int synchron, u_long32 *mid, const char *rhost, 
                         const char *commproc, int id, sge_pack_buffer *pb, 
                         int tag, u_long32  response_id, lList **alpp)
{
   int i;
   u_long32 me_who;
   cl_xml_ack_type_t ack_type;
   cl_com_handle_t* handle = NULL;
   unsigned long dummy_mid = 0;

   DENTER(GDI_LAYER, "sge_send_any_request");

   ack_type = CL_MIH_MAT_NAK;

   if (rhost == NULL) {
      answer_list_add(alpp, MSG_GDI_RHOSTISNULLFORSENDREQUEST, STATUS_ESYNTAX,
                      ANSWER_QUALITY_ERROR);
      DEXIT;
      return CL_RETVAL_PARAMS;
   }
   
   me_who = uti_state_get_mewho();
   handle = cl_com_get_handle((char*)uti_state_get_sge_formal_prog_name() ,0);
   if (handle == NULL) {
      answer_list_add(alpp, MSG_GDI_NOCOMMHANDLE, STATUS_NOCOMMD, ANSWER_QUALITY_ERROR);
      DEXIT;
      return CL_RETVAL_HANDLE_NOT_FOUND;
   }

   if (strcmp(commproc, (char*)prognames[QMASTER]) == 0 && id == 1) {
      cl_com_append_known_endpoint_from_name((char*)rhost, (char*)prognames[QMASTER], 1 , sge_get_qmaster_port(), CL_CM_AC_DISABLED ,1);
   }
   
   if (synchron) {
      ack_type = CL_MIH_MAT_ACK;
   }
  
#if 0
   SGE_ADD_MSG_ID(sprintf(SGE_EVENT, MSG_GDI_SENDINGMESSAGE_SIU, commproc,id,
                          (unsigned long) pb->bytes_used));
   answer_list_add(alpp, SGE_EVENT, STATUS_OK, ANSWER_QUALITY_INFO);
#endif
   i = gdi_send_sec_message( handle,
                                (char*) rhost,(char*) commproc , id, 
                                ack_type , 
                                (cl_byte_t*)pb->head_ptr ,(unsigned long) pb->bytes_used , 
                                &dummy_mid , response_id, tag, 1, synchron);
   if (i != CL_RETVAL_OK) {
      /* try again ( if connection timed out ) */
      i = gdi_send_sec_message( handle,
                                   (char*)rhost, (char*)commproc , id, 
                                   ack_type ,
                                   (cl_byte_t*)pb->head_ptr ,(unsigned long) pb->bytes_used , 
                                   &dummy_mid ,response_id,tag,1 , synchron);
   }
   
   if (mid) {
      *mid = dummy_mid;
   }

   if (i != CL_RETVAL_OK) {
      SGE_ADD_MSG_ID(sprintf(SGE_EVENT,
                             MSG_GDI_SENDMESSAGETOCOMMPROCFAILED_SSISS ,
                             (synchron ? "" : "a"),
                             commproc, 
                             id, 
                             rhost, 
                             cl_get_error_text(i)));
      answer_list_add(alpp, SGE_EVENT, STATUS_NOCOMMD, ANSWER_QUALITY_ERROR);
   }

   DEXIT;
   return i;
}


/*----------------------------------------------------------
 * sge_get_any_request
 *
 * returns 0               on success
 *         -1              rhost is NULL 
 *         commlib return values (always positive)
 *
 * NOTES
 *    MT-NOTE: sge_get_any_request() is MT safe (assumptions)
 *----------------------------------------------------------*/
int sge_get_any_request(char *rhost, char *commproc, u_short *id, sge_pack_buffer *pb, int *tag, int synchron, u_long32 for_request_mid, u_long32* mid) 
{
   int i;
   ushort usid=0;
   char host[MAXHOSTLEN+1];
   cl_com_message_t* message = NULL;
   cl_com_endpoint_t* sender = NULL;
   cl_com_handle_t* handle = NULL;


   DENTER(GDI_LAYER, "sge_get_any_request");

   PROF_START_MEASUREMENT(SGE_PROF_GDI);

   if (id)
      usid = (ushort)*id;

   if (!rhost) {
      ERROR((SGE_EVENT, MSG_GDI_RHOSTISNULLFORGETANYREQUEST ));
      PROF_STOP_MEASUREMENT(SGE_PROF_GDI);
      DEXIT;
      return -1;
   }
   
   strcpy(host, rhost);

   handle = cl_com_get_handle((char*)uti_state_get_sge_formal_prog_name() ,0);
   cl_commlib_trigger(handle);
   i = gdi_receive_sec_message( handle, rhost, commproc, usid, synchron, for_request_mid, &message, &sender);

   if ( i == CL_RETVAL_CONNECTION_NOT_FOUND ) {
      if ( commproc[0] != '\0' && rhost[0] != '\0' ) {
         /* The connection was closed, reopen it */
         i = cl_commlib_open_connection(handle,rhost,commproc,usid);
         INFO((SGE_EVENT,"reopen connection to %s,%s,"U32CFormat" (2)\n", rhost, commproc, u32c(usid)));
         if (i == CL_RETVAL_OK) {
            INFO((SGE_EVENT,"reconnected successfully\n"));
            i = gdi_receive_sec_message( handle, rhost, commproc, usid, synchron, for_request_mid, &message, &sender);
         }
      } else {
         DEBUG((SGE_EVENT,"can't reopen a connection to unspecified host or commproc (2)\n"));
      }
   }

   if (i != CL_RETVAL_OK) {
      if (i != CL_RETVAL_NO_MESSAGE) {
         /* This if for errors */
         DEBUG((SGE_EVENT, MSG_GDI_RECEIVEMESSAGEFROMCOMMPROCFAILED_SISS , 
               (commproc[0] ? commproc : "any"), 
               (int) usid, 
               (host[0] ? host : "any"),
                cl_get_error_text(i)));
      }
      cl_com_free_message(&message);
      cl_com_free_endpoint(&sender);
      /* This is if no message is there */
      PROF_STOP_MEASUREMENT(SGE_PROF_GDI);
      DEXIT;
      return i;
   }    

   /* ok, we received a message */
   if (message != NULL ) {
      if (sender != NULL && id) {
         *id = sender->comp_id;
      }
      if (tag) 
        *tag = message->message_tag;
      if (mid)
        *mid = message->message_id;


      /* fill it in the packing buffer */
      i = init_packbuffer_from_buffer(pb, (char*)message->message, message->message_length , 0);


      /* TODO: the packbuffer must be hold, not deleted !!! */
      message->message = NULL;

      if(i != PACK_SUCCESS) {
         ERROR((SGE_EVENT, MSG_GDI_ERRORUNPACKINGGDIREQUEST_S, cull_pack_strerror(i)));
         PROF_STOP_MEASUREMENT(SGE_PROF_GDI);
         DEXIT;
         return CL_RETVAL_READ_ERROR;
      } 

      if (sender != NULL ) {
         DEBUG((SGE_EVENT,"received from: %s,"U32CFormat"\n",sender->comp_host, u32c(sender->comp_id) ));
         if (rhost[0] == '\0') {
            strcpy(rhost, sender->comp_host); /* If we receive from anybody return the sender */
         }
         if (commproc[0] == '\0') {
            strcpy(commproc , sender->comp_name); /* If we receive from anybody return the sender */
         }
      }

      cl_com_free_endpoint(&sender);
      cl_com_free_message(&message);
   }
   PROF_STOP_MEASUREMENT(SGE_PROF_GDI);
   DEXIT;
   return CL_RETVAL_OK;
}


/**********************************************************************
  send a message giving a packbuffer

  same as gdi_send_message, but this is delivered a sge_pack_buffer.
  this function flushes the z_stream_buffer if compression is turned on
  and passes the result on to send_message
  Always use this function instead of gdi_send_message directly, even
  if compression is turned off.
  
    NOTES
       MT-NOTE: gdi_send_message_pb() is MT safe (assumptions)
**********************************************************************/
int gdi_send_message_pb(int synchron, const char *tocomproc, int toid, 
                        const char *tohost, int tag, sge_pack_buffer *pb, 
                        u_long32 *mid) 
{
   long ret = 0;

   DENTER(GDI_LAYER, "gdi_send_message_pb");

   if ( !pb ) {
       DPRINTF(("no pointer for sge_pack_buffer\n"));
       ret = gdi_send_message(synchron, tocomproc, toid, tohost, tag, NULL, 0, mid, 0);
       DEXIT;
       return ret;
   }

#ifdef COMMCOMPRESS
   if(pb->mode == 0) {
      if(flush_packbuffer(pb) == PACK_SUCCESS)
         ret = gdi_send_message(synchron, tocomproc, toid, tohost, tag, (char*)pb->head_ptr, pb->cpr.total_out, mid, 1);
      else
         ret = CL_MALLOC;
   }
   else
#endif
      ret = gdi_send_message(synchron, tocomproc, toid, tohost, tag, pb->head_ptr, pb->bytes_used, mid, 0);

   DEXIT;
   return ret;
}

/*-------------------------------------------------------------------------*
 * check_isalive
 *    check if master is registered and alive
 *    calls is_commd_alive() and ask_commproc()
 * returns
 *    0                    if qmaster is enrolled at sge_commd
 *    > 0                  commlib error number (always positve)
 *    CL_FIRST_FREE_EC+1   can't get masterhost
 *    CL_FIRST_FREE_EC+2   can't connect to commd
 *
 *  NOTES
 *     MT-NOTE: check_isalive() is MT safe
 *-------------------------------------------------------------------------*/
int check_isalive(const char *masterhost) 
{
   int alive = CL_RETVAL_OK;
   cl_com_handle_t* handle = NULL;
   cl_com_SIRM_t* status = NULL;
   int ret;
   static int error_locked = 0;

 
   DENTER(TOP_LAYER, "check_isalive");

   if (!masterhost) {
      DPRINTF(("can't get masterhost\n"));
      DEXIT;
      return CL_RETVAL_UNKNOWN_ENDPOINT;
   }


   
   handle=cl_com_get_handle((char*)uti_state_get_sge_formal_prog_name() ,0);
   if (handle == NULL) {

      sge_mutex_lock("check_alive_mutex", SGE_FUNC, __LINE__, &check_alive_mutex);  

      if ( error_locked == 0 ) {
         WARNING((SGE_EVENT,MSG_GDI_COULD_NOT_GET_COM_HANDLE_S, (char*) uti_state_get_sge_formal_prog_name() ));
      }
      error_locked = 1;

      sge_mutex_unlock("check_alive_mutex", SGE_FUNC, __LINE__, &check_alive_mutex);  
      return CL_RETVAL_UNKNOWN;
   }

   sge_mutex_lock("check_alive_mutex", SGE_FUNC, __LINE__, &check_alive_mutex);  
   error_locked = 0;
   sge_mutex_unlock("check_alive_mutex", SGE_FUNC, __LINE__, &check_alive_mutex);  



   ret = cl_commlib_get_endpoint_status(handle,(char*)masterhost,(char*)prognames[QMASTER] , 1, &status);
   if (ret != CL_RETVAL_OK) {
      DPRINTF(("cl_commlib_get_endpoint_status() returned "SFQ"\n", cl_get_error_text(ret)));
      alive = ret;
   } else {
      DEBUG((SGE_EVENT,MSG_GDI_QMASTER_STILL_RUNNING));   
      alive = CL_RETVAL_OK;
   }

   if (status != NULL) {
      DEBUG((SGE_EVENT,MSG_GDI_ENDPOINT_UPTIME_UU, u32c( status->runtime) , u32c( status->application_status) ));
      cl_com_free_sirm_message(&status);
   }
 
   DEXIT;
   return alive;
}