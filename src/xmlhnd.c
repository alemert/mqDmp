/******************************************************************************/
/*                                                                            */
/*   M Q   D U M P                                                            */
/*                                                                            */
/*   functions:                                                        */
/*      - readXml                                              */
/*      - getXMLconfig                                    */
/*      - createConfigXmlNode                          */
/*      - createXmlNode                            */
/*      - getXmlCfgRoot                              */
/*      - findXmlCfgNodeFunc            */
/*                                    */
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

/******************************************************************************/
/*   M A C R O S                                                              */
/******************************************************************************/

/******************************************************************************/
/*   P R O T O T Y P E S                                                      */
/******************************************************************************/
int readXml( const char* file, char** _xmlMem );

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
int getXMLconfig( const char* file )
{
  logFuncCall() ;                       

  int sysRc = 0;

  char *mem = NULL ;

  sysRc = readXml( file, &mem );
  if( sysRc != 0 ) goto _door;

  _door:

  logFuncExit( ) ;
  return sysRc ;
}

/******************************************************************************/
/*  create XML node                        */
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
/*  get XML configuration root node                      */
/******************************************************************************/
tXmlConfigNode* getXmlCfgRoot()
{
  return _gXmlCfgRoot ;
}

/******************************************************************************/
/*  find XML configuration node                  */
/******************************************************************************/
tXmlConfigNode* findXmlCfgNodeFunc( tXmlConfigNode *start, int argc, ... )
{
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