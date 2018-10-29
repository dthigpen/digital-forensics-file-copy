/********************************************************************
 *
 * File         : recoverdocproto.h
 * Author       : Team 7
 * Date created : October 1, 2018
 * Description  : This file contains all function prototypes 
 *
 *******************************************************************/

/*****************************************************************************/
/*                          recoverdoc.c                                  */
/*****************************************************************************/
INT4 HasHexSignatureAtOffset(UINT1 *buffer, const CHAR *hexSigInASCII, UINT4 u4DecOffset);
INT4 RecoverDocFindSignatures();
