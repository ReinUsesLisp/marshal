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
#include <stdlib.h>
#include <string.h>
#include "marshal.h"

static marshal_t *
alloc(marshal_t *dest, int type)
{
	marshal_t *m = dest ? dest : malloc(sizeof(marshal_t));
	memset(m, 0, sizeof(marshal_t));
	m->type = type;
	return m;
}

static void *
memory_clone(int len, void *src)
{
	void *mem = malloc(len);
	if (!mem)
		return NULL;
	memcpy(mem, src, len);
	return mem;
}

static char *
string_clone(const char *src)
{
	size_t len = strlen(src);
	char *clone = malloc(len + 1);
	if (!clone)
		return NULL;
	memcpy(clone, src, len);
	clone[len] = 0;
	return clone;
}

static marshal_t *
clone_boolean(marshal_t *dest, const marshal_t *src)
{
	marshal_t *m = alloc(dest, MARSHAL_BOOLEAN);
	m->boolean.value = src->boolean.value;
	return m;
}

static marshal_t *
clone_integer(marshal_t *dest, const marshal_t *src)
{
	marshal_t *m = alloc(dest, MARSHAL_INTEGER);
	m->integer.value = src->integer.value;
	return m;
}

static marshal_t *
clone_bignum(marshal_t *dest, const marshal_t *src)
{
	marshal_t *m = alloc(dest, MARSHAL_BIGNUM);
	m->bignum.sign = src->bignum.sign;
	m->bignum.length = src->bignum.length;
	m->bignum.bytes = memory_clone(m->bignum.length, src->bignum.bytes);
	return m->bignum.bytes ? m : NULL;
}

static marshal_t *
clone_float(marshal_t *dest, const marshal_t *src)
{
	marshal_t *m = alloc(dest, MARSHAL_FLOAT);
	m->float_no.value = src->float_no.value;
	return m;
}

static marshal_t *
clone_symbol(marshal_t *dest, const marshal_t *src)
{
	marshal_t *m = alloc(dest, MARSHAL_SYMBOL);
	m->symbol.name = string_clone(src->symbol.name);
	return m->symbol.name ? m : NULL;
}

static void *
clone_values(int count, void **src)
{
	int i;
	void **values = malloc(count * sizeof(void *));
	if (!values)
		return NULL;
	for (i = 0; i < count; i++)
	{
		marshal_t *value = marshal_clone(NULL, src[i]);
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
	return values;
}

static marshal_t *
clone_array(marshal_t *dest, const marshal_t *src)
{
	marshal_t *m = alloc(dest, MARSHAL_ARRAY);
	m->array.count = src->array.count;
	m->array.values = clone_values(m->array.count, src->array.values);
	return m->array.values ? m : NULL;
}

static marshal_t *
clone_hash(marshal_t *dest, const marshal_t *src)
{
	marshal_t *m = alloc(dest, MARSHAL_HASH);
	m->hash.count = src->hash.count;
	m->hash.pairs = clone_values(m->hash.count * 2, src->hash.pairs);
	return m->hash.pairs ? m : NULL;
}

static marshal_t *
clone_string(marshal_t *dest, const marshal_t *src)
{
	marshal_t *m = alloc(dest, MARSHAL_STRING);
	m->string.data_size = src->string.data_size;
	m->string.data = malloc(m->string.data_size + 4);
	if (!m->string.data)
		return NULL;
	memcpy(m->string.data, src->string.data, m->string.data_size);
	memset(m->string.data + m->string.data_size, 0, 4);
	m->string.count = src->string.count;
	m->string.pairs = clone_values(m->string.count * 2, src->string.pairs);
	if (!m->string.pairs)
		return NULL;
	m->string.encoding = src->string.encoding;
	return m;
}

static marshal_t *
clone_class(marshal_t *dest, const marshal_t *src)
{
	marshal_t *m = alloc(dest, MARSHAL_CLASS);
	m->klass.name = string_clone(src->klass.name);
	return m->klass.name ? m : NULL;
}

static marshal_t *
clone_module(marshal_t *dest, const marshal_t *src)
{
	marshal_t *m = alloc(dest, MARSHAL_MODULE);
	m->module.name = string_clone(src->module.name);
	return m->module.name ? m : NULL;
}

static marshal_t *
clone_object(marshal_t *dest, const marshal_t *src)
{
	marshal_t *m = alloc(dest, MARSHAL_OBJECT);
	m->object.count = src->object.count;
	m->object.vars = clone_values(m->object.count*2, src->object.vars);
	m->object.symbol_instance =
		marshal_clone(NULL, src->object.symbol_instance);
	m->object.klass =
		((marshal_t *)m->object.symbol_instance)->symbol.name;
	return m;
}

static marshal_t *
clone_userdef(marshal_t *dest, const marshal_t *src)
{
	marshal_t *m = alloc(dest, MARSHAL_USERDEF);
	m->userdef.size = src->userdef.size;
	m->userdef.data = memory_clone(m->userdef.size, src->userdef.data);
	m->userdef.symbol_instance =
		marshal_clone(NULL, src->userdef.symbol_instance);
	m->userdef.klass =
		((marshal_t *)m->userdef.symbol_instance)->symbol.name;
	return m;
}

marshal_t *
marshal_clone(marshal_t *dest, const marshal_t *src)
{
	switch (src->type)
	{
		case MARSHAL_NIL: return alloc(dest, MARSHAL_NIL);
		case MARSHAL_BOOLEAN: return clone_boolean(dest, src);
		case MARSHAL_INTEGER: return clone_integer(dest, src);
		case MARSHAL_BIGNUM: return clone_bignum(dest, src);
		case MARSHAL_FLOAT: return clone_float(dest, src);
		case MARSHAL_SYMBOL: return clone_symbol(dest, src);
		case MARSHAL_ARRAY: return clone_array(dest, src);
		case MARSHAL_HASH: return clone_hash(dest, src);
		case MARSHAL_STRING: return clone_string(dest, src);
		case MARSHAL_CLASS: return clone_class(dest, src);
		case MARSHAL_MODULE: return clone_module(dest, src);
		case MARSHAL_OBJECT: return clone_object(dest, src);
		case MARSHAL_USERDEF: return clone_userdef(dest, src);
		default: return NULL;
	}
}
