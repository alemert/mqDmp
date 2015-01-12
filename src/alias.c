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
#include <stdlib.h>

// ---------------------------------------------------------
// MQ
// ---------------------------------------------------------

// ---------------------------------------------------------
// own 
// ---------------------------------------------------------
#include <ctl.h>
#include <msgcat/lgstd.h>
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

  MQLONG lng ;
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
  if( itemCnt > 0 )                       // item count is MQ reason code if > 0
  {                                       // therefore error
    mqrc = itemCnt;                       //
    goto _door;                           // anything else is real item counter
  }                                       //
  itemCnt =- itemCnt ;                    //
                                          // 
  // -------------------------------------------------------
  // allocate memory for queue names
  // -------------------------------------------------------
  alias = (char**) malloc( sizeof(char*)*(itemCnt+1));
  if( alias == NULL )                     //
  {                                       //
    logger(LSTD_MEM_ALLOC_ERROR);         //
    mqrc = -1 ;                           //
    goto _door;                           //
  }                                       //
  alias[itemCnt] = NULL ;                 //
                                          //
  // -------------------------------------------------------
  //
  // -------------------------------------------------------
  for( i=0; i<itemCnt; i++ )              // go through all bags (queues)
  {                                       //
    mqrc = mqBagInq( respBag   ,          // inquire each bag (each queue)
                     i         ,          //
                    &qAttrBag );          //
                                          //
    switch( mqrc )                        //  handle error
    {                                     //
      case MQRC_NONE : break;             //
      default: goto _door;                //
    }                                     //
                                          //
    alias[i] = (char*)malloc(sizeof(char)*(itemCnt)*(MQ_Q_NAME_LENGTH+1));
    if( alias[i] == NULL )                //
    {                                     //
      logger(LSTD_MEM_ALLOC_ERROR);       //
      mqrc = -1 ;                         //
      goto _door;                         //
    }                                     //
    memset(alias[i],(int)' ',MQ_Q_NAME_LENGTH); 
    alias[i][MQ_Q_NAME_LENGTH] = '\0';    //
                                          //
    mqrc = mqStrInq( qAttrBag          ,  // attribute bag
                     MQCA_Q_NAME       ,  // selector (queue name)
                     0                 ,  // item index
                     MQ_Q_NAME_LENGTH  ,  // available buffer length
                     (PMQCHAR) alias[i],  // buffer
                     &lng             );  // real buffer length
                                          //
    switch( mqrc )                        //  handle error
    {                                     //
      case MQRC_NONE : break;             //
      default: goto _door;                //
    }                                     //
                                          //
    if( lng < MQ_Q_NAME_LENGTH)           //
    {                                     //
      goto _door;                         //
    }                                     //
                                          //
    
  }

  _door:

  *sysRc = mqrc ;

  logFuncExit( ) ;
  return (const char**) alias ;
}


