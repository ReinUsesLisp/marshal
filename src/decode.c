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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "marshal.h"
#include "format.h"

#define GROW_RATE 8

#define OK 0
#define FAILED 1

#define CHECK(call) do { int _err = call ; if (_err) return _err; } while (0)
#define CHECK_NULL(call) do { if (! call ) return FAILED; } while (0)

typedef struct
{
	int sym_size;
	int obj_size;
	int sym_count;
	int obj_count;
	marshal_t **syms;
	marshal_t **objs;
} cache_t;

typedef const void *buf_t;

static marshal_t *
decode(buf_t *buf, cache_t *cache);

static void
read(void *ptr, size_t size, buf_t *buf)
{
	memcpy(ptr, *buf, size);
	*buf += size;
}

static int
add_object(cache_t *cache, marshal_t *new_obj)
{
	/* expand cache if it's required */
	if (cache->obj_size <= cache->obj_count+1)
	{
		cache->obj_size = cache->obj_size ?
			cache->obj_size * GROW_RATE : GROW_RATE;
		cache->objs = realloc(cache->objs, cache->obj_size * sizeof(void *));
		if (!cache->objs)
			return FAILED;
	}
	cache->objs[cache->obj_count] = new_obj;
	cache->obj_count++;
	return OK;
}

static int
read_integer(buf_t *buf)
{
	long n = 0;
	int raw = 0;
	read(&raw, 1, buf);

	if (0 == raw)
		return 0;
	else if (raw <= 5)
	{
		read(&n, raw, buf);
		return n;
	}
	else if (raw <= 0x7F)
		return raw - 5;
	else if (raw <= 0xFB)
		return raw - 0xFB;
	else if (raw <= 0xFF)
	{
		int bytes = (0xFF+1) - raw;
		read(&n, bytes, buf);
		return n - (1 << bytes * 8);
	}
	/* it should never reach here,
	 * returning 0 something (an allocation) should fail */
	return 0;
}

static char *
read_chars(buf_t *buf)
{
	char *raw;
	int len = read_integer(buf);

	if (len < 0)
		return NULL;
	raw = malloc(len+1);
	if (!raw)
		return NULL;
	read(raw, len, buf);
	raw[len] = 0;
	return raw;
}

static void **
decode_values(int count, buf_t *buf, cache_t *cache)
{
	int i;
	marshal_t **values = malloc(count * sizeof(marshal_t *));
	if (!values)
		return NULL;
	for (i = 0; i < count; i++)
	{
		marshal_t *value = decode(buf, cache);
		if (!value)
		{
			int j;
			for (j = 0; j < i; j++)
				marshal_free(values[j]);
			free(values);
			return NULL;
		}
		values[i] = value;
	}
	return (void **)values;
}

static int
decode_nil(marshal_t *m)
{
	m->type = MARSHAL_NIL;
	return OK;
}

static int
decode_true(marshal_t *m)
{
	m->type = MARSHAL_BOOLEAN;
	m->boolean.value = 1;
	return OK;
}

static int
decode_false(marshal_t *m)
{
	m->type = MARSHAL_BOOLEAN;
	m->boolean.value = 0;
	return OK;
}

static int
decode_integer(marshal_t *m, buf_t *buf)
{
	m->type = MARSHAL_INTEGER;
	m->integer.value = read_integer(buf);
	return OK;
}

static int
decode_bignum(marshal_t *m, buf_t *buf, cache_t *cache)
{
	char sign;
	m->type = MARSHAL_BIGNUM;

	read(&sign, 1, buf);
	if ('+' == sign)
		m->bignum.sign = 1;
	else if ('-' == sign)
		m->bignum.sign = -1;
	else
		return FAILED;
	
	m->bignum.length = read_integer(buf) * 2;

	m->bignum.bytes = malloc(m->bignum.length);
	CHECK_NULL(m->bignum.bytes);

	read(m->bignum.bytes, m->bignum.length, buf);

	return add_object(cache, m);
}

static int
decode_float(marshal_t *m, buf_t *buf)
{
	char *raw = read_chars(buf);
	CHECK_NULL(raw);

	m->type = MARSHAL_FLOAT;
	m->float_no.value = atof(raw);
	free(raw);
	return OK;
}

static int
decode_symbol(marshal_t *m, buf_t *buf, cache_t *cache)
{
	m->type = MARSHAL_SYMBOL;
	m->symbol.name = read_chars(buf);
	CHECK_NULL(m->symbol.name);

	/* expand symbol cache if it's required */
	if (cache->sym_size <= cache->sym_count+1)
	{
		cache->sym_size = cache->sym_size ?
			cache->sym_size * GROW_RATE : GROW_RATE;
		cache->syms =
			realloc(cache->syms, cache->sym_size * sizeof(void *));
		if (!cache->syms)
		{
			free(m->symbol.name);
			return FAILED;
		}
	}
	cache->syms[cache->sym_count] = m;
	cache->sym_count++;
	return OK;
}

static int
decode_symlink(marshal_t *m, buf_t *buf, cache_t *cache)
{
	int index = read_integer(buf);
	if (index >= cache->sym_count)
		return FAILED;
	CHECK_NULL(marshal_clone(m, cache->syms[index]));
	return OK;
}

static int
decode_array(marshal_t *m, buf_t *buf, cache_t *cache)
{
	void **values;
	int len;

	CHECK(add_object(cache, m));

	len = read_integer(buf);
	values = decode_values(len, buf, cache);
	CHECK_NULL(values);

	m->type = MARSHAL_ARRAY;
	m->array.count = len;
	m->array.values = values;
	return OK;
}

static int
decode_hash(marshal_t *m, buf_t *buf, cache_t *cache, int has_def)
{
	void **pairs;
	void *def = NULL;
	int len;

	CHECK(add_object(cache, m));

	len = read_integer(buf);
	pairs = decode_values(len*2, buf, cache);
	CHECK_NULL(pairs);

	if (has_def)
	{
		def = decode(buf, cache);
		CHECK_NULL(def);
	}

	m->type = MARSHAL_HASH;
	m->hash.count = len;
	m->hash.pairs = pairs;
	m->hash.def = def;
	return OK;
}

static int
decode_old_string(marshal_t *m, buf_t *buf, cache_t *cache)
{
	m->type = MARSHAL_STRING;
	m->string.data = read_chars(buf);
	m->string.data_size = strlen(m->string.data);
	CHECK_NULL(m->string.data);
	m->string.count = 0;
	m->string.pairs = NULL;
	m->string.encoding = MARSHAL_ENCODE_UTF8;
	return add_object(cache, m);
}

static int
decode_ivar(marshal_t *m, buf_t *buf, cache_t *cache)
{
	char type;
	int data_len;
	void *data;
	int count;
	void **pairs;

	CHECK(add_object(cache, m));

	/* read head data (string) */
	read(&type, 1, buf);
	data_len = read_integer(buf);
	data = malloc(data_len+4);
	CHECK_NULL(data);
	read(data, data_len, buf);
	memset(data+data_len, 0, 4);

	/* get instances */
	count = read_integer(buf);
	pairs = decode_values(count*2, buf, cache);
	CHECK_NULL(pairs);

	switch (type)
	{
		case M_STRING:
			m->type = MARSHAL_STRING;
			m->string.data_size = data_len;
			m->string.data = data;
			m->string.count = count;
			m->string.pairs = pairs;
			/* TODO read encoding */
			m->string.encoding = MARSHAL_ENCODE_UTF8;
			break;

		case M_REGEX:
			return FAILED;
	}
	return OK;
}

static int
decode_class(marshal_t *m, buf_t *buf, cache_t *cache)
{
	m->type = MARSHAL_CLASS;
	m->klass.name = read_chars(buf);
	CHECK_NULL(m->klass.name);
	return add_object(cache, m);
}

static int
decode_module(marshal_t *m, buf_t *buf, cache_t *cache)
{
	m->type = MARSHAL_MODULE;
	m->module.name = read_chars(buf);
	CHECK_NULL(m->module.name);
	return add_object(cache, m);
}

static int
decode_object(marshal_t *m, buf_t *buf, cache_t *cache)
{
	int count;
	void **vars;

	marshal_t *klass_name = decode(buf, cache);
	CHECK_NULL(klass_name);

	count = read_integer(buf);
	vars = decode_values(count*2, buf, cache);

	if (!vars || MARSHAL_SYMBOL != klass_name->type)
	{
		marshal_free(klass_name);
		return FAILED;
	}

	m->type = MARSHAL_OBJECT;
	m->object.count = count;
	m->object.klass = klass_name->symbol.name;
	m->object.vars = vars;
	m->object.symbol_instance = klass_name;
	return OK;
}

static int
decode_userdef(marshal_t *m, buf_t *buf, cache_t *cache)
{
	int size;
	void *data;

	marshal_t *klass_name = decode(buf, cache);
	CHECK_NULL(klass_name);

	size = read_integer(buf);
	data = malloc(size);
	CHECK_NULL(data);
	read(data, size, buf);

	m->type = MARSHAL_USERDEF;
	m->userdef.size = size;
	m->userdef.klass = klass_name->symbol.name;
	m->userdef.data = data;
	m->userdef.symbol_instance = klass_name;
	return OK;
}

static int
decode_object_ref(marshal_t *m, buf_t *buf, cache_t *cache)
{
	int index = read_integer(buf);
	if (index >= cache->obj_count)
		return FAILED;
	CHECK_NULL(marshal_clone(m, cache->objs[index]));
	return OK;
}

static int
decode_type_case(marshal_t *m, buf_t *buf, cache_t *cache)
{
	int type = 0;
	read(&type, 1, buf);

	switch (type)
	{
		case M_NIL: return decode_nil(m);
		case M_TRUE: return decode_true(m);
		case M_FALSE: return decode_false(m);
		case M_INTEGER: return decode_integer(m, buf);
		case M_BIGNUM: return decode_bignum(m, buf, cache);
		case M_FLOAT: return decode_float(m, buf);
		case M_SYMBOL: return decode_symbol(m, buf, cache);
		case M_SYMLINK: return decode_symlink(m, buf, cache);
		case M_ARRAY: return decode_array(m, buf, cache);
		case M_HASH: return decode_hash(m, buf, cache, 0);
		case M_HASH_DEFAULT: return decode_hash(m, buf, cache, 1);
		case M_OLD_STRING: return decode_old_string(m, buf, cache);
		case M_IVAR: return decode_ivar(m, buf, cache);
		case M_CLASS: return decode_class(m, buf, cache);
		case M_MODULE: return decode_module(m, buf, cache);
		case M_OBJECT: return decode_object(m, buf, cache);
		case M_USERDEF: return decode_userdef(m, buf, cache);
		case M_OBJECT_REF: return decode_object_ref(m, buf, cache);
		default:
			/* printf("%d (%c)\n", type, type); */
			return FAILED;
	}
}

static marshal_t *
decode(buf_t *buf, cache_t *cache)
{
	marshal_t *marshal = malloc(sizeof(marshal_t));
	if (!marshal)
		return NULL;
	if (FAILED == decode_type_case(marshal, buf, cache))
	{
		free(marshal);
		return NULL;
	}
	return marshal;
}

marshal_t *
marshal_decode(const void *data)
{
	marshal_t *marshal;
	cache_t cache = {};

	buf_t *buf = &data;
	int major = 0, minor = 0;

	read(&major, 1, buf);
	read(&minor, 1, buf);

	if (4 != major || 8 != minor)
		return NULL;

	marshal = decode(buf, &cache);

	/* free stuff */
	if (cache.syms)
		free(cache.syms);
	if (cache.objs)
		free(cache.objs);
	return marshal;
}

marshal_t *
marshal_decode_file(const char *path)
{
	marshal_t *marshal = NULL;
	long len;

	FILE *file = fopen(path, "rb");
	if (!file)
		return NULL;
	fseek(file, 0, SEEK_END);
	len = ftell(file);
	fseek(file, 0, SEEK_SET);

	void *data = malloc(len);
	if (!data)
		return NULL;
	/* check if data is successfully readed */
	if ((size_t)len == fread(data, 1, len, file))
		marshal = marshal_decode(data);
	free(data);
	fclose(file);
	return marshal;
}
