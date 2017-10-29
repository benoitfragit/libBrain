/**
 * \file brain_core_types.h
 * \brief Define all types
 * \author Benoit F.
 * \date 11 decembre 2016
 *
 * This file contains all predefines types used in this library
 */
#ifndef BRAIN_CORE_TYPES_H
#define BRAIN_CORE_TYPES_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlwriter.h>

/**
 * \def BRAIN_TRUE
 */
#define BRAIN_TRUE  1
/**
 * \def BRAIN_FALSE
 */
#define BRAIN_FALSE 0

typedef unsigned char BrainBool;
typedef int           BrainInt;
typedef unsigned int  BrainUint;
typedef float         BrainFloat;
typedef double        BrainDouble;
typedef const char*   BrainString;
typedef char          BrainChar;
typedef BrainDouble*  BrainSignal;

/**
 * \brief define a XML Context
 */
typedef xmlNodePtr Context;
/**
 * \brief define an XML Document
 */
typedef xmlDocPtr Document;
/**
 * \brief Define an XML buffer
 */
typedef xmlChar*  Buffer;
/**
 * \brief define an XML writer
 */
typedef xmlTextWriterPtr Writer;
/**
 * \brief Define a BrainNetwork
 */
typedef struct Network* BrainNetwork;
#endif /* BRAIN_CORE_TYPES_H */