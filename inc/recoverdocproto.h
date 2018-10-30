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
UINT1 u1MatchesSignatureValues(UINT1 *u1ActualValues, UINT1 *u1CorrectValues, size_t size);
INT4 RecoverDocFindSignatures(UINT1 u1SeachFlags);
// VOID DumpCompoundBinaryFileHeader(struct StructuredStorageHeader* header);
