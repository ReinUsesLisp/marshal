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

marshal_t *
marshal_array_get(const marshal_t *array, int index)
{
	marshal_array_t *a = (marshal_array_t *)array;
	if (!a || MARSHAL_ARRAY != a->type || index >= a->count || !a->values)
		return NULL;
	return a->values[index];
}

marshal_t *
marshal_array_add(marshal_t *array, marshal_t *value)
{
	marshal_array_t *a = (marshal_array_t *)array;
	if (MARSHAL_ARRAY != a->type)
		return NULL;
	a->values = realloc(a->values, (a->count+1) * sizeof(void *));
	/* if realloc fails then a->values has no reference
	   a memory leak may occur */
	if (!a->values)
		return NULL;
	a->values[a->count] = value;
	a->count++;
	return array;
}

marshal_t *
marshal_array_del(marshal_t *array, int index)
{
	int i;
	marshal_array_t *a = (marshal_array_t *)array;
	if (MARSHAL_ARRAY != a->type || index >= a->count || index < 0)
		return NULL;

	marshal_free(a->values[index]);

	/* remove it from values */
	for (i = index; i < a->count-1; i++)
		a->values[i] = a->values[i+1];
	a->values[a->count] = NULL;

	a->count--;
	return array;
}

static int
hash_get_index(const marshal_hash_t *h, const marshal_t *key)
{
	int i;
	for (i = 0; i < h->count; i++)
	{
		if (marshal_equal(h->pairs[i*2], key))
			return i;
	}
	return -1;
}

marshal_t *
marshal_hash_get(const marshal_t *hash, const marshal_t *key)
{
	marshal_hash_t *h = (marshal_hash_t *)hash;
	int index;
	if (MARSHAL_HASH != h->type)
		return NULL;
	index = hash_get_index(h, key);
	return index < 0 ? h->def : h->pairs[index*2+1];
}

marshal_t *
marshal_hash_set(marshal_t *hash, marshal_t *key, marshal_t *value)
{
	marshal_hash_t *h = (marshal_hash_t *)hash;
	int index;
	if (!key || !value || MARSHAL_HASH != h->type)
		return NULL;
	index = hash_get_index(h, key);
	/* no previous value found */
	if (index < 0)
	{
		h->pairs = realloc(h->pairs, (h->count+2) * sizeof(void *));
		/* same as in marshal_array_add, leak is possible */
		if (!h->pairs)
			return NULL;
		h->pairs[h->count*2] = key;
		h->pairs[h->count*2+1] = value;
		h->count++;
	}
	else
	{
		/* hash-key's memory must be freed
		   because argument-key will replace it */
		marshal_free(h->pairs[index*2]);
		h->pairs[index*2] = key;

		/* replace old value with the new one */
		marshal_free(h->pairs[index*2+1]);
		h->pairs[index*2+1] = value;
	}
	return value;
}

MARSHAL_API marshal_t *
marshal_object_get(const marshal_t *marshal, const char *name)
{
	int i;
        if (MARSHAL_OBJECT != marshal->type)
                return NULL;
        for (i = 0; i < marshal->object.count; i++)
        {
                marshal_t *symbol = marshal->object.vars[i*2];
                marshal_t *var = marshal->object.vars[i*2+1];
                if (MARSHAL_SYMBOL != symbol->type)
                        continue;
                if (0 == strcmp(symbol->symbol.name, name))
                        return var;
        }
        return NULL;
}
