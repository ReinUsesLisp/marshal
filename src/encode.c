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

#define BUFFER_GROW_RATE 4096 /* heap */
#define FLOAT_BUFFER_SIZE 128 /* stack */

#define OK 0
#define FAILED 1

#define CHECK(call) do { int _err = call ; if (_err) return _err; } while (0)
#define CHECK_NULL(call) do { if (! call ) return FAILED; } while (0)

typedef struct
{
	size_t size;
	size_t cur;
	void *mem;
} buf_t;

static int
encode(const marshal_t *m, buf_t *buf);

static int
write(const void *ptr, size_t size, buf_t *buf)
{
	if (buf->cur + size >= buf->size)
	{
		buf->size += BUFFER_GROW_RATE;
		buf->mem = realloc(buf->mem, buf->size);
		CHECK_NULL(buf->mem);
	}
	memcpy(buf->mem + buf->cur, ptr, size);
	buf->cur += size;
	return OK;
}

static int
write_integer(buf_t *buf, int integer)
{
	if (0 == integer)
	{
		int zero = 0;
		CHECK(write(&zero, 1, buf));
	}
	else if (integer > 0 && integer <= 122)
	{
		int number = integer + 5;
		CHECK(write(&number, 1, buf));
	}
	else if (integer > 122)
	{
		int bytes;
		if (integer < 0xFF)
			bytes = 1;
		else if (integer < 0xFFFF)
			bytes = 2;
		else if (integer < 0xFFFFFF)
			bytes = 3;
		else
			bytes = 4;
		CHECK(write(&bytes, 1, buf));
		CHECK(write(&integer, bytes, buf));
	}
	else if (integer >= -123)
	{
		int number = integer + 0xFB;
		CHECK(write(&number, 1, buf));
	}
	else if (integer < -123)
	{
		int bytes;
		int encoded_bytes;
		int number;
		int positive = -integer;
		if (positive < 0xFF)
			bytes = 1;
		else if (positive < 0xFFFF)
			bytes = 2;
		else if (positive < 0xFFFFFF)
			bytes = 3;
		else
			bytes = 4;
		encoded_bytes = 0x100 - bytes;
		number = integer + (1 << bytes * 8);
		CHECK(write(&encoded_bytes, 1, buf));
		CHECK(write(&number, bytes, buf));
	}
	else
		return FAILED;
	return OK;
}

static int
write_values(buf_t *buf, const void **values, int count)
{
	int i;
	for (i = 0; i < count; i++)
		CHECK(encode(values[i], buf));
	return OK;
}

static int
encode_nil(const marshal_t *m, buf_t *buf)
{
	int type = M_NIL;
	m = m;
	return write(&type, 1, buf);
}

static int
encode_boolean(const marshal_t *m, buf_t *buf)
{
	int type = m->boolean.value ? M_TRUE : M_FALSE;
	return write(&type, 1, buf);
}

static int
encode_integer(const marshal_t *m, buf_t *buf)
{
	int type = M_INTEGER;
	CHECK(write(&type, 1, buf));
	return write_integer(buf, m->integer.value);
}

static int
encode_bignum(const marshal_t *m, buf_t *buf)
{
	int type = M_BIGNUM;
	char sign = m->bignum.sign > 0 ? '+' : '-';

	CHECK(write(&type, 1, buf));
	CHECK(write(&sign, 1, buf));
	CHECK(write_integer(buf, m->bignum.length));
	CHECK(write(m->bignum.bytes, m->bignum.length, buf));
	return OK;
}

static int
encode_float(const marshal_t *m, buf_t *buf)
{
	int type = M_FLOAT;
	char str[FLOAT_BUFFER_SIZE];
	int len;

	snprintf(str, sizeof(str), "%f", m->float_no.value);
	len = (int)strlen(str); /* trust snprintf */

	CHECK(write(&type, 1, buf));
	CHECK(write_integer(buf, len));
	CHECK(write(str, len, buf));
	return OK;
}

static int
encode_symbol(const marshal_t *m, buf_t *buf)
{
	int type = M_SYMBOL;
	int len = (int)strlen(m->symbol.name);
	CHECK(write(&type, 1, buf));
	CHECK(write_integer(buf, len));
	CHECK(write(m->symbol.name, len, buf));
	return OK;
}

static int
encode_array(const marshal_t *m, buf_t *buf)
{
	int type = M_ARRAY;
	CHECK(write(&type, 1, buf));
	CHECK(write_integer(buf, m->array.count));
	CHECK(write_values(buf, (const void **)m->array.values,
		m->array.count));
	return OK;
}

static int
encode_hash(const marshal_t *m, buf_t *buf)
{
	int type = m->hash.def ? M_HASH_DEFAULT : M_HASH;
	CHECK(write(&type, 1, buf));
	CHECK(write_integer(buf, m->hash.count));
	CHECK(write_values(buf, (const void **)m->hash.pairs,
		m->hash.count*2));
	if (m->hash.def)
		CHECK(encode(m->hash.def, buf));
	return OK;
}

static int
encode_string(const marshal_t *m, buf_t *buf)
{
	int type = M_IVAR;
	int subtype = M_STRING;

	CHECK(write(&type, 1, buf));
	CHECK(write(&subtype, 1, buf));
	CHECK(write_integer(buf, m->string.data_size));
	CHECK(write(m->string.data, m->string.data_size, buf));
	/* encoding must be in pairs */
	CHECK(write_integer(buf, m->string.count));
	CHECK(write_values(buf, (const void **)m->string.pairs,
		m->string.count*2));
	return OK;
}

static int
encode(const marshal_t *m, buf_t *buf)
{
	switch (m->type)
	{
		case MARSHAL_NIL: return encode_nil(m, buf);
		case MARSHAL_BOOLEAN: return encode_boolean(m, buf);
		case MARSHAL_INTEGER: return encode_integer(m, buf);
		case MARSHAL_BIGNUM: return encode_bignum(m, buf);
		case MARSHAL_FLOAT: return encode_float(m, buf);
		case MARSHAL_SYMBOL: return encode_symbol(m, buf);
		case MARSHAL_ARRAY: return encode_array(m, buf);
		case MARSHAL_HASH: return encode_hash(m, buf);
		case MARSHAL_STRING: return encode_string(m, buf);
		default:
			fprintf(stderr, "not implemented %d\n", m->type);
			return FAILED;
	}
}

static int
begin_encode(const marshal_t *m, buf_t *buf)
{
	int major = 4;
	int minor = 8;
	CHECK(write(&major, 1, buf));
	CHECK(write(&minor, 1, buf));
	return encode(m, buf);
}

void *
marshal_encode(const marshal_t *marshal, size_t *size)
{
	buf_t buf = {};
	if (FAILED == begin_encode(marshal, &buf))
	{
		if (buf.mem)
			free(buf.mem);
		if (size)
			*size = 0;
		return NULL;
	}
	else
	{
		/* last position (current cursor) is its size */
		if (size)
			*size = buf.cur;
		return buf.mem;
	}
}

int
marshal_encode_file(const char *path, const marshal_t *marshal)
{
	size_t size, written;
	void *mem = marshal_encode(marshal, &size);
	FILE *file;
	if (!mem)
		return FAILED;
	file = fopen(path, "wb");
	if (!file)
		return FAILED;
	written = fwrite(mem, 1, size, file);
	fclose(file);
	return size == written ? OK : FAILED;
}
