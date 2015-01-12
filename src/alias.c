/******************************************************************************/
/*                       D U M P   M Q   O B J E C T S                        */
/*                                                                            */
/*      module: worker                                                        */
/*                                                                            */
/*      functions:                        */
/*        - getQmgrAliases                   */
/*                    */
/******************************************************************************/

/******************************************************************************/
/*   I N C L U D E S                                                          */
/******************************************************************************/

// ---------------------------------------------------------
// system
// ---------------------------------------------------------

// ---------------------------------------------------------
// MQ
// ---------------------------------------------------------

// ---------------------------------------------------------
// own 
// ---------------------------------------------------------
#include <ctl.h>
#include <mqbase.h>

// ---------------------------------------------------------
// local
// ---------------------------------------------------------
#include <worker.h>

/******************************************************************************/
/*   G L O B A L S                                                            */
/******************************************************************************/

/******************************************************************************/
/*   D E F I N E S                                                            */
/******************************************************************************/

/******************************************************************************/
/*   M A C R O S                                                              */
/******************************************************************************/

/******************************************************************************/
/*   P R O T O T Y P E S                                                      */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*   F U N C T I O N S                                                        */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*    get queue manager aliases            */
/******************************************************************************/
const char** getQmgrAliases( MQHCONN hCon, int *sysRc ) 
{
  logFuncCall() ;                       

  MQLONG mqrc = MQRC_NONE ;
  MQLONG itemCnt ;
  int    i;
//MQLONG compCode ;

  MQHBAG cmdBag  = MQHB_UNUSABLE_HBAG;
  MQHBAG respBag = MQHB_UNUSABLE_HBAG;
  MQHBAG qAttrBag ;


  char** alias = NULL;

  // -------------------------------------------------------
  // open bags
  // -------------------------------------------------------
  mqrc = mqOpenAdminBag( &cmdBag );
  switch( mqrc )
  {
    case MQRC_NONE : break;
    default: goto _door;
  }

  mqrc = mqOpenAdminBag( &respBag );
  switch( mqrc )
  {
    case MQRC_NONE : break;
    default: goto _door;
  }

  // -------------------------------------------------------
  // dis qr(*) queue
  // -------------------------------------------------------
  mqrc = mqAddStr( cmdBag, MQCA_Q_NAME, MQBL_NULL_TERMINATED, "*");
  switch( mqrc )
  {
    case MQRC_NONE : break;
    default: goto _door;
  }

  mqrc = mqAddInt( cmdBag, MQIA_Q_TYPE, MQQT_REMOTE );
  switch( mqrc )
  {
    case MQRC_NONE : break;
    default: goto _door;
  }
  
  mqrc = mqExecPcf( hCon           ,   // connection handle
                    MQCMD_INQUIRE_Q,   // PCF command
                    cmdBag         ,   // command bag
                    respBag       );   // response bag 

  switch( mqrc )
  {
    case MQRC_NONE : break;
    default: goto _door;
  }

  // -------------------------------------------------------
  // Count the number of system bags embedded in the 
  // response bag from the mqExecute call. The attributes 
  // for each queue are in a separate bag.  
  // -------------------------------------------------------
  itemCnt = mqBagCountItem( respBag, MQHA_BAG_HANDLE );
  if( itemCnt > 0 )
  {
    mqrc = itemCnt ;
    goto _door ;
  }

  for( i=0; i<itemCnt; i++ )
  {
    
  }

  _door:

  *sysRc = mqrc ;

  logFuncExit( ) ;
  return (const char**) alias ;
}


