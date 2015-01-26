/******************************************************************************/
/*                       D U M P   M Q   O B J E C T S                        */
/*                                                                            */
/*      module: worker                                                        */
/*                                                                            */
/*      functions:                                  */
/*        - worker                                          */
/*        - setupXmlConfig              */
/*                        */
/******************************************************************************/

/******************************************************************************/
/*   I N C L U D E S                                                          */
/******************************************************************************/

// ---------------------------------------------------------
// system
// ---------------------------------------------------------
//#include <stdio.h>

// ---------------------------------------------------------
// MQ
// ---------------------------------------------------------
#include <cmqc.h>

// ---------------------------------------------------------
// own 
// ---------------------------------------------------------
#include <ctl.h>
#include <mqbase.h>

// ---------------------------------------------------------
// local
// ---------------------------------------------------------
#include "cmdln.h"

#include <worker.h>
#include <xmlhnd.h>

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
int setupXmlConfig();

/******************************************************************************/
/*                                                                            */
/*   F U N C T I O N S                                                        */
/*                                                                            */
/******************************************************************************/
int worker()
{
  logFuncCall() ;                       

  int sysRc = 0;

  MQHCONN  Hcon   ;                 // connection handle   
  char qmgrName[MQ_Q_MGR_NAME_LENGTH+1] ;
  const char **qmgrAlias ;
  char **p;

  if( getStrAttr( "proxy" ) )
  {
    snprintf( qmgrName, MQ_Q_MGR_NAME_LENGTH, "%s", getStrAttr("proxy"));
  }
  else
  {
    memset(qmgrName,' ', MQ_Q_MGR_NAME_LENGTH );
  }

  // -------------------------------------------------------
  // connect to queue manager
  // -------------------------------------------------------
  sysRc =  mqConn( (char*) qmgrName,      // queue manager          
                           &Hcon  );      // connection handle            
                                          //
  switch( sysRc )                         //
  {                                       //
    case MQRC_NONE :     break ;          // OK
    case MQRC_Q_MGR_NAME_ERROR :          // queue manager does not exists
    {                                     //
      logger(LMQM_UNKNOWN_QMGR,qmgrName); //
      goto _conn;                         //
    }                                     //
    default : goto _conn;                 // error will be logged in mqConn
  }                                       //
                                          //
  // -------------------------------------------------------
  // get remote queue manager aliases
  // -------------------------------------------------------
  qmgrAlias = getQmgrAliases( Hcon, &sysRc ); 

  // -------------------------------------------------------
  // check for command line (show / list queue manager aliases)
  // -------------------------------------------------------
  if( getFlagAttr("show") == 0 )          //
  {                                       //
    p=(char**)qmgrAlias;                  //
    while( *p )                           //
    {                                     //
      printf( " %s\n", *p );              //
      p++;                                //
    }                                     //
    sysRc = 0;                            //
    goto _door;                           //
  }                                       //
                                          //
  // -------------------------------------------------------
  // for all other command line attributes an initialization file is necessary
  // -------------------------------------------------------
  if( !getStrAttr("ini")  )               //
  {                                       //
    fprintf(stderr,"--ini attribute missing\n") ;
    sysRc = 1;                            //
    goto _door;                           //
  }                                       //
                                          //
  setupXmlConfig();                       // setup XML rules
                                          //
  if( getXml( getStrAttr("ini") ) != 0 )  //
  {                                       //
    sysRc = 2;                            //
    goto _door;                           //
  }                                       //
                                          //
  // -------------------------------------------------------
  // error handling exit point
  // -------------------------------------------------------
  _door:                                  //
                                          //
  // -------------------------------------------------------
  // connect to queue manager
  // -------------------------------------------------------
  sysRc = mqDisc( &Hcon ) ;               //
                                          //
  switch( sysRc )                         //
  {                                       //
    case MQRC_NONE: break;                //
    default : goto _door ;                //
  }                                       //
                                          //
  logFuncExit( ) ;

  _conn:
  return sysRc ;
}

/******************************************************************************/
/* setup XML Configuration                        */
/******************************************************************************/
int setupXmlConfig()
{
  int sysRc = 0;

  logFuncCall() ;                       

//  tXmlConfigNode *node; 
  
  if( !createConfigXmlNode( NULL         , 
                            XML_ROOT_ID  , XML_ROOT_DSCR, 
                            EMPTY        , 
                            OBLIGATORY ) ) { sysRc=1; goto _door;}

  if( !createConfigXmlNode( getXmlCfgRoot(), 
                            XML_GEN_ID, XML_GEN_DSCR,
                            EMPTY        , 
                            OBLIGATORY  ) ) { sysRc=1; goto _door;}

  if( !createConfigXmlNode( findXmlCfgNode( XML_ROOT_ID,
                                            XML_GEN_ID ),
                            MQIA_TRIGGER_CONTROL, def2str(MQIA_TRIGGER_CONTROL),
                            INT        , 
                            OBLIGATORY  ) ) { sysRc=1; goto _door;}

  if( !createConfigXmlNode( getXmlCfgRoot(), 
                            XML_MQ_ALL_QMGR_ID , XML_MQ_ALL_QMGR_DSCR,
                            EMPTY          , 
                            OBLIGATORY   ) ) { sysRc=1; goto _door;}

  if( !createConfigXmlNode( findXmlCfgNode( XML_ROOT_ID  ,
                                            XML_MQ_ALL_QMGR_ID),
                            XML_MQ_QMGR_ID , XML_MQ_QMGR_DSCR,
                            FILTER         , 
                            OBLIGATORY   ) ) { sysRc=1; goto _door;}

#if(0)
  if( !createConfigXmlNode( findXmlCfgNode( XML_ROOT_ID    , 
                                            XML_MQ_QMGR_ID ,
                                            XML_MQ_QMGR_ID),
                            XML_NAME_ID, XML_NAME_DSCR,
                            STR        , 
                            OBLIGATORY   ) ) { sysRc=1; goto _door;}
#endif
  _door:

  logFuncExit( ) ;
  return sysRc;
}
