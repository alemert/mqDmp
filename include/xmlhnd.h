/******************************************************************************/
/*   M Q   D U M P                   */
/******************************************************************************/

/******************************************************************************/
/*   I N C L U D E S                                                          */
/******************************************************************************/
// ---------------------------------------------------------
// system
// ---------------------------------------------------------

// ---------------------------------------------------------
// own 
// ---------------------------------------------------------

/******************************************************************************/
/*   D E F I N E S                                                            */
/******************************************************************************/
#define XML_NO_ID      -1 

#define XML_ROOT_ID     0
#define XML_ROOT_DSCR   "root"

#define XML_GEN_ID     10
#define XML_GEN_DSCR   "general"

#define XML_MQ_ALL_QMGR_ID   100
#define XML_MQ_ALL_QMGR_DSCR "allqmgr"

#define XML_MQ_QMGR_ID   101
#define XML_MQ_QMGR_DSCR "qmgr"

#define def2str( str) #str 

/******************************************************************************/
/*   T Y P E S                                                                */
/******************************************************************************/

// ---------------------------------------------------------
// XML Node
// ---------------------------------------------------------
typedef struct sXmlNode  tXmlNode ;
typedef enum   eXmlType  tXmlType ;
typedef union  uXmlVara  tXmlVara ;

// ---------------------------------------------------------
// XML Configuration Node
// ---------------------------------------------------------
typedef struct sXmlConfigNode  tXmlConfigNode ;
typedef enum   eXmlAppliance   tXmlAppliance ;

/******************************************************************************/
/*   S T R U C T S                                                            */
/******************************************************************************/

// ---------------------------------------------------------
// XML Node
// ---------------------------------------------------------
enum eXmlType 
{ 
  NA    ,       // not available
  EMPTY ,       // empty parent node
  FILTER,       // parent node with string filter 
  STR   ,       // child node with string value
  INT           // child node with integer value
};

union uXmlVara 
{
  int   iVal;
  char* sVal;
};

struct sXmlNode
{
  int      id;          // id of the node, should be a #define macro
                        //
  tXmlType type;        // type of the node
  tXmlVara vara;        //
  char     filter[16];  //
                        //
  tXmlNode *parent;     //
  tXmlNode *child ;     //
  tXmlNode *next  ;     //
};

// ---------------------------------------------------------
// XML Configuration Node
// ---------------------------------------------------------
enum eXmlAppliance { OPTIONAL, OBLIGATORY };

struct sXmlConfigNode
{
  int   id;                 //
  char *description;        //
                            //
  tXmlType       type;      //
  tXmlAppliance appliance;  //
                            //
  tXmlConfigNode *parent;   //
  tXmlConfigNode *child ;   //
  tXmlConfigNode *next  ;   //
};

/******************************************************************************/
/*   G L O B A L E S                                                          */
/******************************************************************************/

/******************************************************************************/
/*   M A C R O S                                                              */
/******************************************************************************/
#define findXmlCfgNode(     ...)                                   \
        findXmlCfgNodeFunc( getXmlCfgRoot()                      , \
                            ( sizeof( (int[]){0,##__VA_ARGS__} ) / \
			    sizeof(MQLONG)-1)                    , \
			    ##__VA_ARGS__                          ) 

/******************************************************************************/
/*   P R O T O T Y P E S                                                      */
/******************************************************************************/
int getXml( const char* file );

tXmlConfigNode* createConfigXmlNode( tXmlConfigNode *_parent, 
                                     int             _id    , 
                                     const char*     _dscr  ,
                                     tXmlType        _type  ,
                                     tXmlAppliance   _appl  );

tXmlConfigNode* getXmlCfgRoot();
tXmlConfigNode* findXmlCfgNodeFunc( tXmlConfigNode *start, int argc, ... );