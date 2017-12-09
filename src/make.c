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
alloc(int type)
{
	marshal_t *m = calloc(1, sizeof(marshal_t));
	m->type = type;
	return m;
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

marshal_t *
marshal_make_nil()
{
	return alloc(MARSHAL_NIL);
}

marshal_t *
marshal_make_boolean(int value)
{
	marshal_t *m = alloc(MARSHAL_BOOLEAN);
	if (m)
		m->boolean.value = value;
	return m;
}

marshal_t *
marshal_make_integer(int value)
{
	marshal_t *m = alloc(MARSHAL_INTEGER);
	if (m)
		m->integer.value = value;
	return m;
}

marshal_t *
marshal_make_bignum(int sign, int length, unsigned char *bytes)
{
	marshal_t *m = alloc(MARSHAL_BIGNUM);
	unsigned char *fresh = malloc(length);
	if (!m || !fresh)
	{
		if (m)
			free(m);
		if (fresh)
			free(fresh);
		return NULL;
	}
	memcpy(fresh, bytes, length);

	m->bignum.sign = sign;
	m->bignum.length = length;
	m->bignum.bytes = fresh;
	return m;
}

marshal_t *
marshal_make_float(double value)
{
	marshal_t *m = alloc(MARSHAL_FLOAT);
	if (m)
		m->float_no.value = value;
	return m;
}

marshal_t *
marshal_make_symbol(const char *name)
{
	marshal_t *m = alloc(MARSHAL_SYMBOL);
	if (m)
	{
		m->symbol.name = string_clone(name);
		if (!m->symbol.name)
		{
			free(m);
			return NULL;
		}
	}
	return m;
}

marshal_t *
marshal_make_array()
{
	return alloc(MARSHAL_ARRAY);
}

marshal_t *
marshal_make_hash(marshal_t *def)
{
	marshal_t *m = alloc(MARSHAL_HASH);
	if (m)
		m->hash.def = def;
	return m;
}

marshal_t *
marshal_make_ascii(const char *string)
{
	marshal_t *m = alloc(MARSHAL_STRING);
	if (m)
	{
		m->string.data_size = strlen(string);
		m->string.data = string_clone(string);
		m->string.encoding = MARSHAL_ENCODING_ASCII_8BIT;
		if (!m->string.data)
		{
			free(m);
			return NULL;
		}
	}
	return m;
}

marshal_t *
marshal_make_class(const char *name)
{
	marshal_t *m = alloc(MARSHAL_CLASS);
	if (m)
	{
		m->klass.name = string_clone(name);
		if (!m->klass.name)
		{
			free(m);
			return NULL;
		}
	}
	return m;
}

marshal_t *
marshal_make_module(const char *name)
{
	marshal_t *m = alloc(MARSHAL_MODULE);
	if (m)
	{
		m->module.name = string_clone(name);
		if (!m->module.name)
		{
			free(m);
			return NULL;
		}
	}
	return m;
}

marshal_t *
marshal_make_object(const char *klass)
{
	marshal_t *m = alloc(MARSHAL_OBJECT);
	if (m)
	{
		m->object.klass = string_clone(klass);
		if (!m->object.klass)
		{
			free(m);
			return NULL;
		}

	}
	return m;
}

marshal_t *
marshal_make_userdef(const char *klass, int size, const void *data)
{
	marshal_t *m = alloc(MARSHAL_USERDEF);
	if (m)
	{
		m->userdef.klass = string_clone(klass);
		if (!m->userdef.klass)
		{
			free(m);
			return NULL;
		}
		m->userdef.size = size;
		m->userdef.data = malloc(size);
		if (!m->userdef.data)
		{
			free(m->userdef.klass);
			free(m);
			return NULL;
		}
		memcpy(m->userdef.data, data, size);
	};
	return m;
}
