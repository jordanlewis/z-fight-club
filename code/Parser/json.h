/*! \file json.h
 *
 * \author John Reppy
 *
 * The representation of JSON files produced by the JSON parser.
 */

/*
 * COPYRIGHT (c) 2010 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 *
 * This code it adapted from the Manticore Project sources.
 *
 *	http://manticore.cs.uchicago.edu
 */

#ifndef _JSON_H_
#define _JSON_H_

#include <stdint.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

//! \brief the types of JSON values
typedef enum {
    JSON_null,		//!< the null value
    JSON_bool,		//!< boolean values
    JSON_int,		//!< integer values
    JSON_float,		//!< floating-point values
    JSON_string,	//!< string values
    JSON_array,		//!< an array of values
    JSON_object		//!< a JSON object (i.e., name/value pairs)
} JSON_Type_t;

typedef struct struct_json_array JSON_Array_t;
typedef struct struct_json_object JSON_Object_t;
typedef struct struct_json_value JSON_Value_t;

struct struct_json_array {
    int			length;
    JSON_Value_t	**elems;
};

typedef struct {
    char	*label;
    JSON_Value_t *data;
} JSON_Field_t;

struct struct_json_object {
    int			length;
    JSON_Field_t	*elems;
};

struct struct_json_value {
    JSON_Type_t		tag;
    union {
	JSON_Array_t	array;
	JSON_Object_t	obj;
	bool		boolean;
	int64_t		integer;
	double		flt;
	char		*string;
    } u;
};

/*! \brief Parse a JSON file
 *  \parm file the name of the file to parse
 *  \return the JSON value in the file, or 0 if there was an error.
 */
JSON_Value_t *JSON_ParseFile (const char *file);

/*! \brief Free the storage allocated for a JSON value
 *  \param v the JSON value to be freed.
 */
void JSON_Free (JSON_Value_t *v);

/*! \brief Return the named field from a JSON object
 *  \param v the JSON object
 *  \param name the name of the object field to extract
 *  \return the value of the field, or 0 if the field is not present
 */
JSON_Value_t *JSON_GetField (JSON_Value_t *v, const char *name);

/*! \brief Return the given item from a JSON array
 *  \param v the JSON array
 *  \param i the index of the array element to return
 *  \return the element or 0 if the field is not present
 */
JSON_Value_t *JSON_GetElem (JSON_Value_t *v, int i);

/*! \brief Convert a JSON value to an integer.
 *  \param v the JSON value, which should be an integer
 *  \param n for returning the value of \a v
 *  \return true if the conversion was successful; false otherwise.
 */
bool JSON_GetInt (JSON_Value_t *v, int *n);

/*! \brief Convert a JSON value to a boolean.
 *  \param v the JSON value, which should be a boolean
 *  \param n for returning the value of \a v
 *  \return true if the conversion was successful; false otherwise.
 */
bool JSON_GetBool (JSON_Value_t *v, bool *b);

/*! \brief Convert a JSON value to a floating-point number
 *  \param v the JSON value, which should be an integer or floating-point number
 *  \param n for returning the value of \a v
 *  \return true if the conversion was successful; false otherwise.
 */
bool JSON_GetFloat (JSON_Value_t *v, float *f);

/*! \brief return the character pointer for a string value (or 0 if
 *  the JSON value is not a string).  Note that the storage for the
 *  string in allocated as part of the \a v object.
 *  \param v the JSON value that is the string
 *  \return the pointer to the string or 0 if \a v is not a string
 */
const char *JSON_GetString (JSON_Value_t *v);

/*! \brief convert a JSON value to a JSON array.
 *  \param v the JSON value, which should be an array
 *  \return the JSON array, or 0 if \a v is not an array
 */
JSON_Array_t *JSON_GetArray (JSON_Value_t *v);

/*! \brief convert a JSON value to a JSON object.
 *  \param v the JSON value, which should be an object
 *  \return the JSON array, or 0 if \a v is not an object
 */
JSON_Object_t *JSON_GetObj (JSON_Value_t *v);

#ifdef __cplusplus
}
#endif

#endif /* !_JSON_H_ */
