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
   RAPP_MSG_TYPE_MSG_A = 0x51u
  ,RAPP_MSG_TYPE_MSG_B = 0x52u
  ,RAPP_MSG_TYPE_MSG_C = 0x54u
  ,RAPP_MSG_TYPE_CONFIG_START = 0x61u
  ,RAPP_MSG_TYPE_CONFIG_STOP  = 0x62u
  ,RAPP_MSG_TYPE_CONFIG_READY = 0x63u
  ,RAPP_MSG_TYPE_CONFIG_PARAM = 0x64u
  ,RAPP_MSG_TYPE_CONFIG_PID   = 0x65u
  ,RAPP_MSG_TYPE_CONFIG_BCKST = 0x66u
  ,RAPP_MSG_TYPE_CONFIG_TRANS = 0x67u
  ,RAPP_MSG_TYPE_MSG_ERROR = 0xFF
} RAPP_MSG_Type;


#endif /* __RNET_APP_CONFIG__ */
