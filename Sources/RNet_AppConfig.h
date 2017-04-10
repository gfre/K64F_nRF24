/**
 * \file
 * \brief This is a configuration file for the RNet stack
 * \author (c) 2014 Erich Styger, http://mcuoneclipse.com/
 * \note MIT License (http://opensource.org/licenses/mit-license.html)
 *
 * Here the stack can be configured using macros.
 */

#ifndef __RNET_APP_CONFIG__
#define __RNET_APP_CONFIG__

/*! type ID's for application messages */
typedef enum {
   RAPP_MSG_TYPE_MSG_A = 0x51
  ,RAPP_MSG_TYPE_MSG_B = 0x52
  ,RAPP_MSG_TYPE_MSG_C = 0x54
  ,RAPP_MSG_TYPE_CONFIG_START = 0x61
  ,RAPP_MSG_TYPE_CONFIG_STOP  = 0x62
  ,RAPP_MSG_TYPE_CONFIG_PARAM = 0x63
  ,RAPP_MSG_TYPE_CONFIG_PID   = 0x64
  ,RAPP_MSG_TYPE_CONFIG_BCKST = 0x65
  ,RAPP_MSG_TYPE_MSG_ERROR = 0xFF
} RAPP_MSG_Type;



#endif /* __RNET_APP_CONFIG__ */
