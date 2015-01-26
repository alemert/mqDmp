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
#include <xmlhnd.h>

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
  node->type      = NA;                         //
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
  return _gXmlCfgRoot ;
}

tXmlNode* getXmlRoot()
{
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
    case EMPTY:
    {
      snprintf( buffer, XML_TAG_LENGTH, "<%s>",node->description);
      break;
    }
    case FILTER :
    {
      snprintf( buffer,
                XML_TAG_LENGTH,
                "<%s\\s*=\"(\\w+)\"\\s*>",
                node->description);
      break;
    }
    case STR :
    {
      snprintf( buffer,
                XML_TAG_LENGTH,
                "<%s\\s*=\"(\\d+)\"\\s*>",
                node->description);
      break;
    }
    case INT :
    {
      snprintf( buffer,
                XML_TAG_LENGTH,
                "%s\\s*=\\s*\\(d+)",
                node->description);
      break;
    }
    case NA: break;
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
tXmlNode* setXmlNode( tXmlNode *parent, tXmlConfigNode *cfg, char *mem )
{
  logFuncCall() ;                       
  int sysRc ;  

  tXmlNode *node=NULL;
  tXmlNode *p = NULL;

  char *submem ;

  char       rxNat[4*XML_TAG_LENGTH];       // regular expression native
  regex_t    rxComp;                        // regular expression compiled
  regmatch_t rxMatch[3];                    // regular expression match array
  char       rxErrBuf[100];                 // regular expression error buffer
                                            //
  const char* openTag  = openXmlTag(cfg);   //
  const char* closeTag = closeXmlTag(cfg);  //
                                            //
  // -------------------------------------------------------
  // build up, compile and execute regular expression
  // -------------------------------------------------------
  // alles falsch muss in schleife
  switch( cfg->type )                      //
  {                                         //
    case EMPTY :                            //
    {                                       //
      snprintf( rxNat           ,           //
                4*XML_TAG_LENGTH,           //
                "%s[[:space:]]*(.+)[[:space:]]*%s",
                openTag         ,           //
                closeTag       );           //
      break;                        //
    }                                       //
    case STR :                  //
    {                          //
      snprintf( rxNat, 4*XML_TAG_LENGTH,      //
	       "%s"  , openTag        );      //
      break;                    //
    }                    //
                                            //
  }
  sysRc = regcomp( &rxComp, rxNat, REG_NOTBOL );
  if( sysRc != 0 )                          //
  {                                         //
    // logger
    goto _door;                             //
  }                                         //
                                            //
#if(1)
  char* q = rxNat;    
#endif
  sysRc = regexec( &rxComp ,            // regular expression
                    mem    ,        // string to analyze
                    3      ,           // number of substrings to match
                    rxMatch,       // matched strings
                    0     );          // flags
//  regfree(&rxComp);
  if( sysRc != 0 )                      //
  {                            //
    regerror(sysRc,&rxComp,rxErrBuf,100);  //
    goto _door;              //
  }                      //
                      //
  node = (tXmlNode*)malloc(sizeof(tXmlNode));  //
                            //
  node->id = cfg->id ;        //
  node->next=NULL;        //
  node->child=NULL;        //
  node->parent=parent;        //
  if( parent == NULL )        //
  {                //
    _gXmlRoot = node;      //
  }          //
  else        //
  {          //
    if( !parent->child )       //
    {        //
      parent->child = node;      //
    }        //
    else        //
    {              //
      p=parent->child ;      //
      while( p->next )      //
      {        //
	p=p->next;      //
      }        //
      p->next = node;      //
    }        //
  }                //
              //
  switch( cfg->type)       //
  {      //
    case EMPTY:       //
    {      //
      submem=(char*)malloc( sizeof(char)*(rxMatch[1].rm_eo-rxMatch[1].rm_so));
      memcpy(submem,(mem+rxMatch[1].rm_so),rxMatch[1].rm_eo-rxMatch[1].rm_so);
      *(submem+rxMatch[1].rm_eo-rxMatch[1].rm_so)='\0';
      setXmlNode(node,cfg->child,submem);
      break;
    }
  }


  _door:

  logFuncExit( ) ;
  return node ;
}

/******************************************************************************/
/*  match regular expression      */
/******************************************************************************/
int matchRegExp( char* _rxNat )
{
  logFuncCall() ;                       

  int sysRc ;

  regex_t    rxComp;                        // regular expression compiled

  sysRc = regcomp( &rxComp, rxNat, REG_NOTBOL );
  if( sysRc != 0 )                          //
  {                                         //
    // logger
    goto _door;                             //
  }                                         //

  _door:

  logFuncExit( ) ;
  return sysRc ;
}

/******************************************************************************/
/*  parse XML memory      */
/******************************************************************************/
tXmlType parseXmlMem( char* mem, tXmlConfigNode cfg)
{
  logFuncCall() ;                       

  logFuncExit( ) ;
  return NA ;
}
