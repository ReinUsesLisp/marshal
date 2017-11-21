/*
 * This file is part of Marshal.
 *
 * Marshal is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Marshal is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Marshal.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _MARSHAL_H_
#define _MARSHAL_H_

/* from http://gcc.gnu.org/wiki/Visibility */
#if defined _WIN32 || defined __CYGWIN__
  #ifdef MARSHAL_BUILDING
    #ifdef __GNUC__
      #define MARSHAL_API __attribute__ ((dllexport))
    #else
      #define MARSHAL_API __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define MARSHAL_API __attribute__ ((dllimport))
    #else
      #define MARSHAL_API __declspec(dllimport)
    #endif
  #endif
#else
  #if __GNUC__ >= 4
    #define MARSHAL_API __attribute__ ((visibility ("default")))
  #else
    #define MARSHAL_API
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MARSHAL_NIL     0
#define MARSHAL_BOOLEAN 1
#define MARSHAL_INTEGER 2
#define MARSHAL_BIGNUM  3
#define MARSHAL_FLOAT   4
#define MARSHAL_SYMBOL  5
#define MARSHAL_ARRAY   6
#define MARSHAL_HASH    7
#define MARSHAL_STRING  8
#define MARSHAL_REGEX   9
#define MARSHAL_CLASS   10
#define MARSHAL_MODULE  11
#define MARSHAL_OBJECT  12
#define MARSHAL_USERDEF 13

#define MARSHAL_ENCODE_UTF8 1

typedef struct
{
	int type;
} marshal_nil_t;

typedef struct
{
	int type;
	int value;
} marshal_boolean_t;

typedef struct
{
	int type;
	int value;
} marshal_integer_t;

typedef struct
{
	int type;
	int sign;
	int length;
	unsigned char *bytes;
} marshal_bignum_t;

typedef struct
{
	int type;
	double value;
} marshal_float_t;

typedef struct
{
	int type;
	char *name;
} marshal_symbol_t;

typedef struct
{
	int type;
	int count;
	void **values;
} marshal_array_t;

typedef struct
{
	int type;
	int count;
	void **pairs; /* even key, odd value */
	void *def;
} marshal_hash_t;

typedef struct
{
	int type;
	int data_size;
	void *data;
	int count;
	void **pairs;
	int encoding;
} marshal_string_t;

typedef struct
{
	int type;
	int data_size;
	void *data;
	int count;
	void **pairs;
	/* TODO */
} marshal_regex_t;

typedef struct
{
	int type;
	char *name;
} marshal_class_t;

typedef struct
{
	int type;
	char *name;
} marshal_module_t;

typedef struct
{
	int type;
	int count;
	char *klass;
	void **vars;
	void *symbol_instance;
} marshal_object_t;

typedef struct
{
	int type;
	int size;
	char *klass;
	void *data;
	void *symbol_instance;
} marshal_userdef_t;

typedef union
{
	int type;
	marshal_nil_t nil;
	marshal_boolean_t boolean;
	marshal_integer_t integer;
	marshal_bignum_t bignum;
	marshal_float_t float_no;
	marshal_symbol_t symbol;
	marshal_array_t array;
	marshal_hash_t hash;
	marshal_string_t string;
	marshal_regex_t regex;
	marshal_class_t klass;
	marshal_module_t module;
	marshal_object_t object;
	marshal_userdef_t userdef;
} marshal_t;

MARSHAL_API marshal_t *
marshal_decode(const void *data);

MARSHAL_API marshal_t *
marshal_decode_file(const char *path);

MARSHAL_API marshal_t *
marshal_clone(marshal_t *dest, const marshal_t *src);

MARSHAL_API void
marshal_free(marshal_t *marshal);

/* stream NULL uses stdout
 * "void *" type is used here to avoid including stdio.h */
MARSHAL_API void
marshal_print(const marshal_t *marshal, void *stream);

#ifdef __cplusplus
}
#endif
#endif /* _MARSHAL_H_ */
