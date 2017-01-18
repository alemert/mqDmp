/******************************************************************************/
/*                                                                            */
/*   M Q   D U M P                                                            */
/*                                                                            */
/*   functions:                                                               */
/*      - readXml                                                             */
/*      - getXml                                                              */
/*      - createConfigXmlNode                                              */
/*      - createXmlNode                                                       */
/*      - getXmlCfgRoot                                                       */
/*      - getXmlRoot                                                    */
/*      - findXmlCfgNodeFunc                          */
/*      - openXmlTag                                      */
/*      - closeXmlTag                                        */
/*      - setXmlNode                                */
/*      - matchRegExp                    */
/*      - parseXmlMem                    */
/*                                          */
/******************************************************************************/

/******************************************************************************/
/*   I N C L U D E S                                                          */
/******************************************************************************/

// ---------------------------------------------------------
// system
// ---------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <regex.h>

// ---------------------------------------------------------
// own 
// ---------------------------------------------------------
#include <genlib.h>

#include <ctl.h>
#include <msgcat/lgstd.h>

// ---------------------------------------------------------
// local
// ---------------------------------------------------------
#include "xmlhnd.h"

/******************************************************************************/
/*   G L O B A L S                                                            */
/******************************************************************************/
tXmlNode       *_gXmlRoot;
tXmlConfigNode *_gXmlCfgRoot;

/******************************************************************************/
/*   D E F I N E S                                                            */
/******************************************************************************/
#define XML_TAG_LENGTH 32

/******************************************************************************/
/*   M A C R O S                                                              */
/******************************************************************************/

/******************************************************************************/
/*   P R O T O T Y P E S                                                      */
/******************************************************************************/
int readXml( const char* file, char** _xmlMem );
tXmlNode* setXmlNode( tXmlNode *xml, tXmlConfigNode *cfg, char *mem );
tXmlNode* getXmlRoot();
tXmlConfigNode* getXmlCfgRoot();
const char* openXmlTag( tXmlConfigNode *node);
const char* closeXmlTag( tXmlConfigNode *node);
const regmatch_t* matchRegExp( char* _rxNat, const char* _mem );
const regmatch_t* parseXmlMem( char* _mem,
                               tXmlConfigNode *_cfg,tXmlType *_pRcType);

/******************************************************************************/
/*                                                                            */
/*   F U N C T I O N S                                                        */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*  read XML file                                                             */
/*                                                                            */
/*  description:                                                    */
/*      read XML file in memory as one line                     */
/*                                                                  */
/*  return code:                                                      */
/*      ok:      0                                            */
/*      error:   i.g. errno                                        */
/*               if error occurs *_iniMem = NULL                */
/*                                                                        */
/******************************************************************************/
int readXml( const char* file, char** _xmlMem )
{
  logFuncCall() ;                       

  int sysRc = 0;
  long memSize ;
  long realSize ;
  char* xmlMem ;

  FILE* ini;

  memSize = fileSize( file );                    // get the size of the
  if( memSize < 0 )                              //   XML-file
  {                                              // handle error
    logger( LSTD_GET_FILE_SIZE, file ) ;         //
    sysRc = -memSize ;                           // if memSize < 0 then
    goto _door ;                                 //   errno = -memSize
  }                                              //
  memSize++ ;                                    // increase memSize for '\0'
                                                 //
  xmlMem = (char*) malloc(memSize*sizeof(char)); // alloc memory in size
  if( xmlMem == NULL )                           //   of the ini-file
  {                                              //  & handle alloc error
    logger( LSTD_MEM_ALLOC_ERROR ) ;             //
    sysRc = errno ;                              //
    goto _door ;                                 //
  }                                              //
                                                 //
  if( (ini = fopen( file, "r" ) ) == NULL )      // open ini file
  {                                              //  & handle error
    logger( LSTD_OPEN_FILE_FAILED, file );       //
    sysRc = errno ;                              //
    goto _door ;                                 //
  }                                              //
                                                 //
  realSize = fread( xmlMem, sizeof(char), memSize-1, ini ) ;
  if( memSize - realSize != 1 )                  // read whole file in one step
  {                                              //  & handle error
    logger( LSTD_ERR_READING_FILE, file );       //
    sysRc = 1 ;                                  //
    goto _door ;                                 //
  }                                              //
  *(xmlMem+realSize) = '\0' ;                    // set the end of ini file flag
                            //
  _door:

  if( ini != NULL ) fclose( ini ) ;
  *_xmlMem = xmlMem ;

  logFuncExit( ) ;
  return sysRc ;
}

/******************************************************************************/
/*  get XML configuration                 */
/******************************************************************************/
int getXml( const char* file )
{
  logFuncCall() ;                       

  int sysRc = 0;

  char *mem = NULL ;

  sysRc = readXml( file, &mem );
  if( sysRc != 0 ) goto _door;

  setXmlNode ( NULL, getXmlCfgRoot() ,mem );

  _door:

  logFuncExit( ) ;
  return sysRc ;
}

/******************************************************************************/
/*  create XML node                              */
/******************************************************************************/
tXmlConfigNode* createConfigXmlNode( tXmlConfigNode *_parent, 
                                     int             _id    , 
                                     const char*     _dscr  ,
                                     tXmlType        _type  ,
                                     tXmlAppliance   _appl  )
{
  logFuncCall() ;                       

  tXmlConfigNode *node ;
  tXmlConfigNode *p ;

  node = (tXmlConfigNode*)malloc( sizeof(tXmlConfigNode) ); 
  if( node == NULL )                  // allocate memory for new
  {                                   //   configuration node 
    logger( LSTD_MEM_ALLOC_ERROR );   // and
    goto _door ;                      // handle allocation error
  }                                   //
                                      //
  // -------------------------------------------------------
  // setup base configuration
  // -------------------------------------------------------
  node->id = _id;                     // id, which should be #define macro
  node->description = (char*) _dscr;  // description (#define macro)
  node->type = _type;                 // data type 
  node->appliance = _appl;            // (obligatory or optional)
  node->parent = _parent;             // parent node, if null, then this nod 
                                      //   is a root node
  // -------------------------------------------------------
  // setup relationship
  // -------------------------------------------------------
  node->child = NULL;                 //
  node->next  = NULL;                 //
  if( _parent == NULL )               // handle a root node
  {                                   //
    _gXmlCfgRoot = node;              // set up as global root
  }                                   //
  else                                //
  {                                   //
    node->parent = _parent;           //
    if( !node->parent->child )        // this node is the first child 
    {                                 //
      node->parent->child = node;     //
    }                                 //
    else                              //
    {                                 //
      p = node->parent->child;        // this node is not first child
      while(p->next)                  // find the last child 
      {                               //
	p=p->next;                    // and add this node as sister (next)
      }                               // to the last child
      p->next = node;                 //
    }                                 //
  }                                   //

  _door:

  logFuncExit( ) ;
  return node;
}

/******************************************************************************/
/*  create XML node                        */
/******************************************************************************/
tXmlNode* createXmlNode()
{
  logFuncCall() ;                       

  tXmlNode *node ;

  node = (tXmlNode*)malloc( sizeof(tXmlNode) ); //
  if( node == NULL )                            //   
  {                                             //   handle allocation error
    logger( LSTD_MEM_ALLOC_ERROR );             //
    goto _door ;                                //
  }                                             //
                                                //
  node->id        = XML_NO_ID;                  //
  node->type      = eNA;                         //
  node->vara.iVal = 0;                          //
  node->parent    = NULL;                       //
  node->child     = NULL;                       //
  node->next      = NULL;                       //
      
  _door:
  return node ;

  logFuncExit( ) ;
}

/******************************************************************************/
/*  get XML configuration root node                            */
/******************************************************************************/
tXmlConfigNode* getXmlCfgRoot()
{
  if( !_gXmlCfgRoot )
    logger( LSTD_XML_NO_ROOT_NODE, "Config");
  return _gXmlCfgRoot ;
}

tXmlNode* getXmlRoot()
{
  if( !_gXmlRoot )
    logger( LSTD_XML_NO_ROOT_NODE, "Data");

  return _gXmlRoot ;
}

/******************************************************************************/
/*  find XML configuration node                              */
/******************************************************************************/
tXmlConfigNode* findXmlCfgNodeFunc( tXmlConfigNode *start, int argc, ... )
{
  logFuncCall() ;                       

  tXmlConfigNode *node = NULL;
  tXmlConfigNode *p    = start;

  int i ;

  int id = XML_NO_ID ;

  va_list argp;

  va_start( argp, argc );

  for(i=0;i<argc;i++)
  {
    id = va_arg( argp, int );
    while( p )
    {
      if( p->id == id ) 
      {
        if( p->child == NULL ) break ;
        p = p->child ;
	break;
      }
      p=p->next;
    }
  }
  if( p ) node=p;
  return node; 
}

/******************************************************************************/
/*  open XML tag                            */
/******************************************************************************/
const char* openXmlTag( tXmlConfigNode *node)
{
  logFuncCall() ;                       
  static char buffer[XML_TAG_LENGTH] ;

  switch( node->type )
  {
    case eEMPTY:
    {
      snprintf( buffer, XML_TAG_LENGTH, "<%s>",node->description);
      break;
    }
#if(0)
    case FILTER :
    {
      snprintf( buffer,
                XML_TAG_LENGTH,
                "<%s\\s*=\"(\\w+)\"\\s*>",
                node->description);
      break;
    }
#endif
    case eSTRING :
    {
      snprintf( buffer,
                XML_TAG_LENGTH,
                "<%s\\s*=\"(\\d+)\"\\s*>",
                node->description);
      break;
    }
    case eINT :
    {
      snprintf( buffer,
                XML_TAG_LENGTH,
                "%s\\s*=\\s*\\(d+)",
                node->description);
      break;
    }
    case eNA: break;
  }

  logFuncExit( ) ;
  return buffer ; 
}

/******************************************************************************/
/*  close XML tag                            */
/******************************************************************************/
const char* closeXmlTag( tXmlConfigNode *node)
{
  logFuncCall() ;                       
  static char buffer[XML_TAG_LENGTH] ;

  snprintf(buffer,XML_TAG_LENGTH,"</%s>",node->description);

  logFuncExit( ) ;
  return buffer ; 
}

/******************************************************************************/
/*  set XML node                                           */
/******************************************************************************/
tXmlNode* setXmlNode( tXmlNode *_parent, tXmlConfigNode *_cfg, char *_mem )
{
  logFuncCall() ;                       

  tXmlNode *node=NULL;
  tXmlNode *pXml = NULL;
  tXmlConfigNode *pCfg = _cfg;
  int iVal;
  char iStr[32];
  char *sStr ;

  char *submem ;

  const regmatch_t *pRxMatch;               // regular expression match array
                        //
  tXmlType nodeType = eNA;      //
                  //
                                  //
  while(pCfg)
  {
    pRxMatch = parseXmlMem( _mem, pCfg, &nodeType);   //
    if( nodeType == eNA )      //
    {                  //
      node = NULL;      //
      goto _door;      //
    }            //
                    //
    node = (tXmlNode*)malloc(sizeof(tXmlNode));
                                          //
    node->id = pCfg->id;                     //
    node->next=NULL;                        //
    node->child=NULL;                       //
    node->parent=_parent;                    //
    if( _parent == NULL )                    //
    {                                       //
      _gXmlRoot = node;                     //
    }                                       //
    else                                    //
    {                                       //
      if( !_parent->child )                  //
      {                                     //
        _parent->child = node;               //
      }                                     //
      else                                  //
      {                                     //
        pXml=_parent->child;                    //
        while( pXml->next )                    //
        {                                   //
          pXml=pXml->next;                        //
        }                                   //
        pXml->next = node;                     //
      }                                     //
    }                                       //
                                          //
    switch( pCfg->type)                      //
    {                                       //
      case eEMPTY:                           //
      {                                     //
        submem=(char*)malloc(sizeof(char)*(pRxMatch[1].rm_eo-pRxMatch[1].rm_so));
        memcpy(submem,(_mem+pRxMatch[1].rm_so),pRxMatch[1].rm_eo-pRxMatch[1].rm_so);
        *(submem+pRxMatch[1].rm_eo-pRxMatch[1].rm_so)='\0';
        setXmlNode(node,pCfg->child,submem);
        break;
      }
      case eSTRING: 
      {
	sStr = (char*) malloc( 200 );
        memcpy(sStr,(_mem+pRxMatch[1].rm_so),pRxMatch[1].rm_eo-pRxMatch[1].rm_so);
        break;
      }
      case eINT: 
      {
        memcpy(iStr,(_mem+pRxMatch[1].rm_so),pRxMatch[1].rm_eo-pRxMatch[1].rm_so);
        *(iStr+pRxMatch[1].rm_eo-pRxMatch[1].rm_so)='\0';
        iVal=(int)strtol(iStr,NULL,10);
	switch( errno )
        {
          case EINVAL:
          case ERANGE:
	  {
            free(node);
            node=NULL;
	 //   logger();
            goto _door;
	  }
          default :break;
        }
        node->vara.iVal = iVal;
        break;
      }
      case eNA: break;
    }
    pCfg = pCfg->next;
  }


  _door:

  logFuncExit( ) ;
  return node ;
}

/******************************************************************************/
/*  match regular expression                                                  */
/******************************************************************************/
const regmatch_t* matchRegExp( char* _rxNat, const char* _mem )
{
  logFuncCall() ;                       

  int sysRc ;

  #define RX_ERR_BUFF_LNG 100
  #define RX_MATCH_LNG     16
  char rxErrBuff[RX_ERR_BUFF_LNG];
  regex_t    rxComp;                        // regular expression compiled
  static regmatch_t rxMatch[RX_MATCH_LNG];  // regular expression match array
                                            //
  sysRc = regcomp( &rxComp, _rxNat, REG_NOTBOL );
  if( sysRc != 0 )                          // compile regular expression
  {                                         //
    logger(LSTD_XML_REGEX_CC_ERR, _rxNat ); //
    goto _door;                             //
  }                                         //
                                            //
  sysRc = regexec( &rxComp      ,           // regular expression
                    _mem        ,           // string to analyze
                    RX_MATCH_LNG,           // number of substrings to match
                    rxMatch     ,           // matched strings
                    0          );           // flags
  if( sysRc != 0 )                          //
  {                                         //
    regerror(sysRc,&rxComp,rxErrBuff,RX_ERR_BUFF_LNG);  
    goto _door;                             //
  }                                         //
                                            //
  _door:

  regfree(&rxComp);      //
  logFuncExit( ) ;
  if( sysRc == 0 ) return rxMatch ;
  return NULL;
}

/******************************************************************************/
/*  parse XML memory                                                          */
/*                                                                            */
/*  description:                                                              */
/*    This function tries to match some regular expressions.               */
/*    A node type depends on which expression matches.                  */
/*    Finally function checks if this type is allowed on this node-tree depth */
/*    and returns the type                                                    */
/*    Type NA will be returned if function fails to match the regular         */
/*    expression                             */
/*                                                                            */
/******************************************************************************/
const regmatch_t* parseXmlMem( char* _mem          , 
                               tXmlConfigNode *_cfg, 
                               tXmlType *_pRcType  )
{
  logFuncCall() ;                       

  char rxNat[4*XML_TAG_LENGTH];    // regular expression native
  const regmatch_t* pRxMatch;      //
                                   //
  *_pRcType = eNA;                  //
  tXmlConfigNode *pCfg = _cfg;     //
                                   //
  while( pCfg )                    //
  {                                //
    switch( pCfg->type )           //
    {                              //
      // ---------------------------------------------------
      // try to match empty tag: <tag> ... </tag>
      // ---------------------------------------------------
      case eEMPTY:                                //
      {                                          //
        snprintf( rxNat             ,            //
                  4*XML_TAG_LENGTH  ,            //
                  "<%s>[[:space:]]*(.+)[[:space:]]*</%s>",
                  pCfg->description ,            //
                  pCfg->description);            //
                                                 //
        pRxMatch = matchRegExp( rxNat, _mem );   // try to match regular expression
                                                 //
        if( pRxMatch != NULL )                   //
        {                                        //
          *_pRcType = pCfg->type;                //
          goto _door;                            //
        }                                        //
        break;                                   //
      }                                          //
                                                 //
      // ---------------------------------------------------
      // try to match key / value: key=value
      // ---------------------------------------------------
      case eSTRING:                                  //
      {                                          //
        snprintf( rxNat, 4*XML_TAG_LENGTH,       //
                "%s\\s*=\\s*(\\S+)",             //
		 pCfg->description);             //
                                                 //
        pRxMatch = matchRegExp( rxNat, _mem );   // try to match regular expression
                                                 //
        if( pRxMatch != NULL )                   //
        {                                        //
          *_pRcType = pCfg->type;                //
          goto _door;                            //
        }                                        //
        break;                                   //
      }                                          //
                                                 //
      // ---------------------------------------------------
      // try to match key / value: key=value
      // ---------------------------------------------------
      case eINT:                                  //
      {                                          //
        snprintf( rxNat, 4*XML_TAG_LENGTH,       //
                "%s[[:space:]]*=[[:space:]]*([[:digit:]]+)",   
		 pCfg->description);             //
                                                 //
        pRxMatch = matchRegExp( rxNat, _mem );   // try to match regular expression
                                                 //
        if( pRxMatch != NULL )                   //
        {                                        //
          *_pRcType = pCfg->type;                //
          goto _door;                      //
        }                                        //
        break;                                   //
      }                                    //
      // ---------------------------------------------------
      // should never come up 
      // ---------------------------------------------------
      case eNA:        //
      {                //
	break;              //
      }                      //
    }                                            //
                                                 //
    pCfg = pCfg->next;                           //
  }                                              //
                                                 //
  _door:               
  logFuncExit( ) ;    
  return pRxMatch;   
}
