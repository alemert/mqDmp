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
#define EMPTY_Q_NAME "                                                 "

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
  MQCHAR qname[MQ_Q_NAME_LENGTH];
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
  // setup PCF witch following MQSC:
  // dis qr(*) xmitq rqmname where( rname ne '' )
  // -------------------------------------------------------
  mqrc = mqAddStr( cmdBag              ,       //
                   MQCA_Q_NAME         ,       //
                   MQBL_NULL_TERMINATED,       // displaying remote queues needs
                   "*"                );       //  two commands
  switch( mqrc )                               //
  {                                            // first: 
    case MQRC_NONE : break;                    //    display queue(*)
    default: goto _door;                       //
  }                                            //
                                               //
  mqrc = mqAddInt( cmdBag       ,              // second:
                   MQIA_Q_TYPE  ,              //    of type qremote
                   MQQT_REMOTE );              //
  switch( mqrc )                               //
  {                                            //
    case MQRC_NONE : break;                    //
    default: goto _door;                       //
  }                                            //
                                               //
  mqrc = mqSetInqAttr( cmdBag          ,       // additional attributes
                       MQCA_XMIT_Q_NAME,       //   XMITQ
                       MQCA_REMOTE_Q_MGR_NAME);//  RQMNAME
  switch( mqrc )                               //
  {                                            //
    case MQRC_NONE : break;                    //
    default: goto _door;                       //
  }                                            //
                                               //
  mqrc = mqAddStrFilter( cmdBag            ,   // where condition
                         MQCA_REMOTE_Q_NAME,   //  Remote Queue
                         MQ_Q_NAME_LENGTH    , //    equals
                         EMPTY_Q_NAME      ,   //  empty queue
                         MQCFOP_EQUAL     );   //
  switch( mqrc )                               //
  {                                            //
    case MQRC_NONE : break;                    //
    default: goto _door;                       //
  }                                            //
                                               //

#if(0)
  mqrc = mqAddStrFilter( cmdBag, 
                         MQCA_XMIT_Q_NAME, 
                         MQ_Q_NAME_LENGTH  , 
			 "                                                 ",
                         MQCFOP_NOT_EQUAL     );
#endif

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
    if( alias[i] == NULL )                // allocate memory for queue name
    {                                     //
      logger(LSTD_MEM_ALLOC_ERROR);       //
      mqrc = -1 ;                         //
      goto _door;                         // set the queue name on empty
    }                                     //
    memset(alias[i],(int)' ',MQ_Q_NAME_LENGTH); 
    alias[i][MQ_Q_NAME_LENGTH] = '\0';    //
                                          //
    mqrc = mqStrInq( qAttrBag        ,    // check for XMITQ 
                     MQCA_XMIT_Q_NAME,    // attribute in Remote Queue
                     0               ,    // 
                     MQ_Q_NAME_LENGTH,    // 
                     qname           ,    // 
                     &lng           );    // 
    switch( mqrc )                        //
    {                                     //
      case MQRC_NONE : break;             //
      default: goto _door;                //
    }                                     //
                                          //
    if( memcmp( qname, EMPTY_Q_NAME, MQ_Q_NAME_LENGTH) == 0 )
    {                                     // ignore all queues with 
      continue;                           //   empty transmission
    }                                     //
                                          //
    mqrc = mqStrInq( qAttrBag        ,    // check for RQMNAME
                     MQCA_REMOTE_Q_NAME,  //  attribute in Remote Queue
                     0               ,    // 
                     MQ_Q_NAME_LENGTH,    // 
                     qname           ,    // 
                     &lng           );    // 
    switch( mqrc )                        // 
    {                                     //
      case MQRC_NONE : break;             //
      default: goto _door;                //
    }                                     //
                                          //
    if( memcmp( qname, EMPTY_Q_NAME, MQ_Q_NAME_LENGTH) == 0 )
    {                                     // ignore all queues with 
      continue;                           //  empty remote queue manager name
    }                                     //
                                          //
    mqrc = mqStrInq( qAttrBag          ,  // final
                     MQCA_Q_NAME       ,  // get the remote queue name
                     0                 ,  // 
                     MQ_Q_NAME_LENGTH  ,  // 
                     (PMQCHAR) alias[i],  // 
                     &lng             );  // 
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
  }                                       //

  _door:

  *sysRc = mqrc ;

  logFuncExit( ) ;
  return (const char**) alias ;
}


