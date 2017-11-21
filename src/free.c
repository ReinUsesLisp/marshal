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
#include "marshal.h"

static void
sfree(void *mem)
{
	if (mem)
		free(mem);
}

void
marshal_free(marshal_t *marshal)
{
	int i;
	if (!marshal)
		return;
	switch (marshal->type)
	{
		case MARSHAL_SYMBOL:
			sfree(marshal->symbol.name);
			break;
		case MARSHAL_BIGNUM:
			sfree(marshal->bignum.bytes);
			break;
		case MARSHAL_ARRAY:
			for (i = 0; i < marshal->array.count; i++)
				marshal_free(marshal->array.values[i]);
			sfree(marshal->array.values);
			break;
		case MARSHAL_HASH:
			for (i = 0; i < marshal->hash.count * 2; i++)
				marshal_free(marshal->hash.pairs[i]);
			sfree(marshal->hash.pairs);
			break;
		case MARSHAL_STRING:
			sfree(marshal->string.data);
			for (i = 0; i < marshal->string.count * 2; i++)
				marshal_free(marshal->string.pairs[i]);
			sfree(marshal->string.pairs);
			break;
		case MARSHAL_REGEX:
			/* TODO */
			break;
		case MARSHAL_CLASS:
			sfree(marshal->klass.name);
			break;
		case MARSHAL_MODULE:
			sfree(marshal->module.name);
			break;
		case MARSHAL_OBJECT:
			for (i = 0; i < marshal->object.count * 2; i++)
				marshal_free(marshal->object.vars[i]);
			sfree(marshal->object.vars);
			marshal_free(marshal->object.symbol_instance);
			break;
		case MARSHAL_USERDEF:
			sfree(marshal->userdef.data);
			marshal_free(marshal->userdef.symbol_instance);
	}
	free(marshal);
}
