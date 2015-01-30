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
#define XML_NO_ID       0 
#define XML_NO_ID_DSCR  ""

#define XML_ROOT        (-1) 
#define XML_ROOT_DSCR   "root"

#define XML_GEN       (-10)
#define XML_GEN_DSCR   "general"

#define XML_NAME       (-11)
#define XML_NAME_DSCR   "name"

#define XML_ENABLED          (-21)
#define XML_ENABLED_DSCR     "enable"

#define XML_MQ_ALL_QMGR      (-100)
#define XML_MQ_ALL_QMGR_DSCR "allqmgr"

#define XML_MQ_QMGR      -101
#define XML_MQ_QMGR_DSCR "qmgr"

#define MQIA_TRIGGER_CONTROL_DSCR   ""

// #define def2str( str ) #str 

#if 1
#define newXmlRule( _parent, _id, _type, _app )                             \
        ({                                                                  \
	  tXmlConfigNode *__rcCN;                                           \
          if( _id < 1  )                                                    \
            __rcCN=createConfigXmlNode(_parent,_id,_id##_DSCR,_type,_app ); \
          else                                                              \
            __rcCN=createConfigXmlNode(_parent,_id,#_id      ,_type,_app ); \
          __rcCN;                                                           \
        })
#endif

#if 0
#define newXmlRule( _parent, _id, _type, _app )                            \
        ({                                                                 \
	  tXmlConfigNode *__rcCN;                                          \
          __rcCN=createConfigXmlNode(_parent,_id,_id##_DSCR,_type,_app );  \
          __rcCN;                                                          \
        })
#define newXmlRule( _parent, _id, _type, _app )                            \
        ({                                                                 \
	  tXmlConfigNode *__rcCN;                                          \
          __rcCN=createConfigXmlNode(_parent,_id,_id##_DSCR,_type,_app );  \
          __rcCN;                                                          \
        })
#endif


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
//FILTER,       // parent node with string filter 
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