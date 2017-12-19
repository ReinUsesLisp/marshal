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
#include <string.h>
#include "marshal.h"

static int
equal(const marshal_t *a, const marshal_t *b);

static int
equal_boolean(const marshal_t *a, const marshal_t *b)
{
	return a->boolean.value == b->boolean.value;
}

static int
equal_integer(const marshal_t *a, const marshal_t *b)
{
	return a->integer.value == b->integer.value;
}

static int
equal_bignum(const marshal_t *a, const marshal_t *b)
{
	return a->bignum.sign == b->bignum.sign
		&& a->bignum.length == b->bignum.length
		&& 0 == memcmp(a->bignum.bytes, b->bignum.bytes,
				a->bignum.length);
}

static int
equal_float(const marshal_t *a, const marshal_t *b)
{
	return a->float_no.value == b->float_no.value;
}

static int
equal_symbol(const marshal_t *a, const marshal_t *b)
{
	return 0 == strcmp(a->symbol.name, b->symbol.name);
}

static int
equal_array(const marshal_t *a, const marshal_t *b)
{
	int i;
	if (a->array.count != b->array.count)
		return 0;
	for (i = 0; i < a->array.count; i++)
	{
		if (!equal(a->array.values[i], b->array.values[i]))
			return 0;
	}
	return 1;
}

static int
equal_hash(const marshal_t *a, const marshal_t *b)
{
	int i;
	if (a->hash.count != b->hash.count || !equal(a->hash.def, b->hash.def))
		return 0;
	/* handle out-of-order hashes */
	for (i = 0; i < a->hash.count; i++)
	{
		marshal_t *a_key = a->hash.pairs[i*2];
		marshal_t *a_value = a->hash.pairs[i*2+1];
		marshal_t *b_value = marshal_hash_get(b, a_key);
		if (!b_value || !equal(a_value, b_value))
			return 0;
	}
	return 1;
}

static int
equal_string(const marshal_t *a, const marshal_t *b)
{
	int i;
	if (a->string.data_size != b->string.data_size
			|| a->string.count != b->string.count
			|| a->string.encoding != b->string.encoding)
		return 0;
	if (0 != memcmp(a->string.data, b->string.data, a->string.data_size))
		return 0;

	for (i = 0; i < a->string.count*2; i++)
	{
		if (!equal(a->string.pairs[i], b->string.pairs[i]))
			return 0;
	}
	return 1;
}

static int
equal_class(const marshal_t *a, const marshal_t *b)
{
	return 0 == strcmp(a->klass.name, b->klass.name);
}

static int
equal_module(const marshal_t *a, const marshal_t *b)
{
	return 0 == strcmp(a->module.name, b->module.name);
}

static int
equal_object(const marshal_t *a, const marshal_t *b)
{
	int i;
	if (a->object.count != b->object.count
			|| 0 != strcmp(a->object.klass, b->object.klass))
		return 0;
	for (i = 0; i < a->object.count; i++)
	{
		marshal_t *found;
		marshal_t *key = a->object.vars[i*2];

		if (MARSHAL_SYMBOL != key->type)
			return 0;
		found = marshal_object_get(b, key->symbol.name);
		if (!found || !equal(a->object.vars[i*2+1], found))
			return 0;
	}
	return 1;
}

static int
equal_userdef(const marshal_t *a, const marshal_t *b)
{
	int size = a->userdef.size;
	return a->userdef.size == b->userdef.size
		&& 0 == strcmp(a->userdef.klass, b->userdef.klass)
		&& 0 == memcmp(a->userdef.data, b->userdef.data, size);
}

static int
equal(const marshal_t *a, const marshal_t *b)
{
	/* if both are NULL or pointers are equal then a and b are equal */
	if ((!a && !b) || (a == b))
		return 1;
	if (!a || !b || a->type != b->type)
		return 0;
	switch (a->type)
	{
		case MARSHAL_NIL: return 1;
                case MARSHAL_BOOLEAN: return equal_boolean(a, b);
		case MARSHAL_INTEGER: return equal_integer(a, b);
		case MARSHAL_BIGNUM: return equal_bignum(a, b);
		case MARSHAL_FLOAT: return equal_float(a, b);
		case MARSHAL_SYMBOL: return equal_symbol(a, b);
		case MARSHAL_ARRAY: return equal_array(a, b);
		case MARSHAL_HASH: return equal_hash(a, b);
		case MARSHAL_STRING: return equal_string(a, b);
		case MARSHAL_CLASS: return equal_class(a, b);
		case MARSHAL_MODULE: return equal_module(a, b);
		case MARSHAL_OBJECT: return equal_object(a, b);
		case MARSHAL_USERDEF: return equal_userdef(a, b);
		default:
			/* fprintf(stderr, "not implemented %d\n", m->type); */
			return 1;
	}
}

int
marshal_equal(const marshal_t *marshal1, const marshal_t *marshal2)
{
	return equal(marshal1, marshal2);
}
