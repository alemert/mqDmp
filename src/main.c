/******************************************************************************/
/* change title on for new project                                            */
/******************************************************************************/

/******************************************************************************/
/*   I N C L U D E S                                                          */
/******************************************************************************/

// ---------------------------------------------------------
// system
// ---------------------------------------------------------

// ---------------------------------------------------------
// mq
// ---------------------------------------------------------

// ---------------------------------------------------------
// own 
// ---------------------------------------------------------
#include "main.h"
#include <ctl.h>
#include <msgcat/lgstd.h>

// ---------------------------------------------------------
// local
// ---------------------------------------------------------

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
/*                                  M A I N                                   */
/*                                                                            */
/******************************************************************************/
#ifndef __TDD__

int main(int argc, const char* argv[] )
{
  int sysRc ;

  sysRc = handleCmdLn( argc, argv ) ;
  if( sysRc != 0 ) goto _door ;

_door :

  return sysRc ;
}

#endif

/******************************************************************************/
/*                                                                            */
/*   F U N C T I O N S                                                        */
/*                                                                            */
/******************************************************************************/

