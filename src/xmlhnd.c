/******************************************************************************/
/*                                                                        */
/*   M Q   D U M P                                             */
/*                                                                */
/*   functions:                                            */
/*      - readXml                                  */
/*      - getXMLconfig                        */
/*      - createXmlNode                    */
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
tXmlNode _gXmlRoot ;
tXmlNode *_gpXmlRoot = &_gXmlRoot;

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
/*  initialize XML node                  */
/******************************************************************************/
tXmlNode* createXmlNode()
{
  logFuncCall() ;                       

  tXmlNode *node ;

  node = (tXmlNode*) malloc( sizeof(tXmlNode));
  if( node == NULL )                           //   of the ini-file
  {                                              //  & handle alloc error
    logger( LSTD_MEM_ALLOC_ERROR ) ;             //
    goto _door ;                                 //
  }                                              //
  
  node->id = NO_XML_ID ;
  memset(node->descr[16],(int)' ',sizeof(node->descr));
  node->type = NA ;
  node->vara.iVal = 0 ;
  node->parent = NULL;
  node->child = NULL;
  node->next  = NULL;

  _door:
  return node ;

  logFuncExit( ) ;
}
