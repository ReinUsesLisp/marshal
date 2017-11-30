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

#include <stddef.h>

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

/* http://ruby_doc.org/core_2.4.2/Encoding.html#method_c_list */
#define MARSHAL_ENCODING_ASCII_8BIT                 0 /* aka OLD_STRING */
#define MARSHAL_ENCODING_UTF_8                      1 /* aka T */
#define MARSHAL_ENCODING_US_ASCII                   2 /* aka F */
#define MARSHAL_ENCODING_UTF_16BE                   3
#define MARSHAL_ENCODING_UTF_16LE                   4
#define MARSHAL_ENCODING_UTF_32BE                   5
#define MARSHAL_ENCODING_UTF_32LE                   6
#define MARSHAL_ENCODING_UTF_16                     7
#define MARSHAL_ENCODING_UTF_32                     8
#define MARSHAL_ENCODING_UTF8_MAC                   9
#define MARSHAL_ENCODING_EUC_JP                     10
#define MARSHAL_ENCODING_Windows_31J                11
#define MARSHAL_ENCODING_Big5                       12
#define MARSHAL_ENCODING_Big5_HKSCS                 13
#define MARSHAL_ENCODING_Big5_UAO                   14
#define MARSHAL_ENCODING_CP949                      15
#define MARSHAL_ENCODING_Emacs_Mule                 16
#define MARSHAL_ENCODING_EUC_KR                     17
#define MARSHAL_ENCODING_EUC_TW                     18
#define MARSHAL_ENCODING_GB2312                     19
#define MARSHAL_ENCODING_GB18030                    20
#define MARSHAL_ENCODING_GBK                        21
#define MARSHAL_ENCODING_ISO_8859_1                 22
#define MARSHAL_ENCODING_ISO_8859_2                 23
#define MARSHAL_ENCODING_ISO_8859_3                 24
#define MARSHAL_ENCODING_ISO_8859_4                 25
#define MARSHAL_ENCODING_ISO_8859_5                 26
#define MARSHAL_ENCODING_ISO_8859_6                 27
#define MARSHAL_ENCODING_ISO_8859_7                 28
#define MARSHAL_ENCODING_ISO_8859_8                 29
#define MARSHAL_ENCODING_ISO_8859_9                 30
#define MARSHAL_ENCODING_ISO_8859_10                31
#define MARSHAL_ENCODING_ISO_8859_11                32
#define MARSHAL_ENCODING_ISO_8859_13                33
#define MARSHAL_ENCODING_ISO_8859_14                34
#define MARSHAL_ENCODING_ISO_8859_15                35
#define MARSHAL_ENCODING_ISO_8859_16                36
#define MARSHAL_ENCODING_KOI8_R                     37
#define MARSHAL_ENCODING_KOI8_U                     38
#define MARSHAL_ENCODING_Shift_JIS                  39
#define MARSHAL_ENCODING_Windows_1250               40
#define MARSHAL_ENCODING_Windows_1251               41
#define MARSHAL_ENCODING_Windows_1252               42
#define MARSHAL_ENCODING_Windows_1253               43
#define MARSHAL_ENCODING_Windows_1254               44
#define MARSHAL_ENCODING_Windows_1257               45
#define MARSHAL_ENCODING_IBM437                     46
#define MARSHAL_ENCODING_IBM737                     47
#define MARSHAL_ENCODING_IBM775                     48
#define MARSHAL_ENCODING_CP850                      49
#define MARSHAL_ENCODING_IBM852                     50
#define MARSHAL_ENCODING_CP852                      51
#define MARSHAL_ENCODING_IBM855                     52
#define MARSHAL_ENCODING_CP855                      53
#define MARSHAL_ENCODING_IBM857                     54
#define MARSHAL_ENCODING_IBM860                     55
#define MARSHAL_ENCODING_IBM861                     56
#define MARSHAL_ENCODING_IBM862                     57
#define MARSHAL_ENCODING_IBM863                     58
#define MARSHAL_ENCODING_IBM864                     59
#define MARSHAL_ENCODING_IBM865                     60
#define MARSHAL_ENCODING_IBM866                     61
#define MARSHAL_ENCODING_IBM869                     62
#define MARSHAL_ENCODING_Windows_1258               63
#define MARSHAL_ENCODING_GB1988                     64
#define MARSHAL_ENCODING_macCentEuro                65
#define MARSHAL_ENCODING_macCroatian                66
#define MARSHAL_ENCODING_macCyrillic                67
#define MARSHAL_ENCODING_macGreek                   68
#define MARSHAL_ENCODING_macIceland                 69
#define MARSHAL_ENCODING_macRoman                   70
#define MARSHAL_ENCODING_macRomania                 71
#define MARSHAL_ENCODING_macThai                    72
#define MARSHAL_ENCODING_macTurkish                 73
#define MARSHAL_ENCODING_macUkraine                 74
#define MARSHAL_ENCODING_CP950                      75
#define MARSHAL_ENCODING_CP951                      76
#define MARSHAL_ENCODING_IBM037                     77
#define MARSHAL_ENCODING_stateless_ISO_2022_JP      78
#define MARSHAL_ENCODING_eucJP_ms                   79
#define MARSHAL_ENCODING_CP51932                    80
#define MARSHAL_ENCODING_EUC_JIS_2004               81
#define MARSHAL_ENCODING_GB12345                    82
#define MARSHAL_ENCODING_ISO_2022_JP                83
#define MARSHAL_ENCODING_ISO_2022_JP_2              84
#define MARSHAL_ENCODING_CP50220                    85
#define MARSHAL_ENCODING_CP50221                    86
#define MARSHAL_ENCODING_Windows_1256               87
#define MARSHAL_ENCODING_Windows_1255               88
#define MARSHAL_ENCODING_TIS_620                    89
#define MARSHAL_ENCODING_Windows_874                90
#define MARSHAL_ENCODING_MacJapanese                91
#define MARSHAL_ENCODING_UTF_7                      92
#define MARSHAL_ENCODING_UTF8_DoCoMo                93
#define MARSHAL_ENCODING_SJIS_DoCoMo                94
#define MARSHAL_ENCODING_UTF8_KDDI                  95
#define MARSHAL_ENCODING_SJIS_KDDI                  96
#define MARSHAL_ENCODING_ISO_2022_JP_KDDI           97
#define MARSHAL_ENCODING_stateless_ISO_2022_JP_KDDI 98
#define MARSHAL_ENCODING_UTF8_SoftBank              99
#define MARSHAL_ENCODING_SJIS_SoftBank              100

typedef struct marshal_nil_t
{
	int type;
} marshal_nil_t;

typedef struct marshal_boolean_t
{
	int type;
	int value;
} marshal_boolean_t;

typedef struct marshal_integer_t
{
	int type;
	int value;
} marshal_integer_t;

typedef struct marshal_bignum_t
{
	int type;
	int sign; /* 1: positive, _1: negative */
	int length;
	unsigned char *bytes;
} marshal_bignum_t;

typedef struct marshal_float_t
{
	int type;
	double value;
} marshal_float_t;

typedef struct marshal_symbol_t
{
	int type;
	char *name;
} marshal_symbol_t;

typedef struct marshal_array_t
{
	int type;
	int count;
	void **values;
} marshal_array_t;

typedef struct marshal_hash_t
{
	int type;
	int count;
	void **pairs; /* even key, odd value */
	void *def;
} marshal_hash_t;

typedef struct marshal_string_t
{
	int type;
	int data_size;
	void *data;
	int count;
	void **pairs;
	int encoding;
} marshal_string_t;

typedef struct marshal_regex_t
{
	int type;
	int data_size;
	void *data;
	int count;
	void **pairs;
	/* TODO */
} marshal_regex_t;

typedef struct marshal_class_t
{
	int type;
	char *name;
} marshal_class_t;

typedef struct marshal_module_t
{
	int type;
	char *name;
} marshal_module_t;

typedef struct marshal_object_t
{
	int type;
	int count;
	char *klass;
	void **vars;
	void *symbol_instance;
} marshal_object_t;

typedef struct marshal_userdef_t
{
	int type;
	int size;
	char *klass;
	void *data;
	void *symbol_instance;
} marshal_userdef_t;

typedef union marshal_t
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

/*
 * decodes a marshal byte stream
 * returns NULL on failure
 */
MARSHAL_API marshal_t *
marshal_decode(const void *data);

/*
 * decodes a marshal file
 * returns NULL on failure
 */
MARSHAL_API marshal_t *
marshal_decode_file(const char *path);

/*
 * encodes a marshal C structure into a malloc_allocated buffer
 * buffer's size is returned in size argument (it can be NULL)
 * returns NULL on failure
 */
MARSHAL_API void *
marshal_encode(const marshal_t *marshal, size_t *size);

/*
 * writes a marshal C structure into a file
 * returns 0 on success
 */
MARSHAL_API int
marshal_encode_file(const char *path, const marshal_t *marshal);

/*
 * deallocates memory used by a marshal C struct, include the pointer itself
 * it should never fail with a cloned or decoded structure
 */
MARSHAL_API void
marshal_free(marshal_t *marshal);

/*
 * makes a deep copy (hosted in fresh memory) of a marshal C structure
 * returns NULL on failure
 */
MARSHAL_API marshal_t *
marshal_clone(marshal_t *dest, const marshal_t *src);

/*
 * prints a marshal C struct like Ruby's "p" function would do
 * stream NULL uses stdout
 * "void *" type is used here to avoid including stdio.h
 */
MARSHAL_API void
marshal_print(const marshal_t *marshal, void *stream);

/*
 * translates a Ruby encoding name into an integer id
 */
MARSHAL_API int
marshal_encoding_name_to_id(const char *name);

/*
 * inverse of marshal_encoding_name_to_id
 */
MARSHAL_API const char *
marshal_encoding_id_to_name(int id);


#ifdef __cplusplus
}
#endif
#endif /* _MARSHAL_H_ */
