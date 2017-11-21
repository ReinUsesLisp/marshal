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
#include "marshal.h"

void
marshal_print(const marshal_t *m, void *stream)
{
	FILE *s = stream ? stream : stdout;
	int i;

	switch (m->type)
	{
		case MARSHAL_NIL:
			fprintf(s, "nil");
			break;
		case MARSHAL_BOOLEAN:
			fprintf(s, "%s", m->boolean.value ? "true" : "false");
			break;
		case MARSHAL_INTEGER:
			fprintf(s, "%d", m->integer.value);
			break;
		case MARSHAL_BIGNUM:
			fprintf(s, m->bignum.sign >= 0 ? "+" : "-");
			fprintf(s, "bignum(");
			for (i = 0; i < m->bignum.length; i++)
			{
				fprintf(s, "0x%02X", m->bignum.bytes[i]);
				if (i+1 < m->bignum.length)
					fprintf(s, " ");
			}
			fprintf(s, ")");
			break;
		case MARSHAL_SYMBOL:
			fprintf(s, ":%s", m->symbol.name);
			break;
		case MARSHAL_ARRAY:
			fprintf(s, "[");
			for (i = 0; i < m->array.count; i++)
			{
				marshal_print(m->array.values[i], s);
				if (i + 1 < m->array.count)
					fprintf(s, ", ");
			}
			fprintf(s, "]");
			break;
		case MARSHAL_HASH:
			fprintf(s, "{");
			for (i = 0; i < m->hash.count; i++)
			{
				marshal_print(m->array.values[i*2], s);
				fprintf(s, "=>");
				marshal_print(m->array.values[i*2+1], s);
				if (i + 1 < m->array.count)
					fprintf(s, ", ");
			}
			fprintf(s, "}");
			break;
		case MARSHAL_FLOAT:
			fprintf(s, "%f", m->float_no.value);
			break;
		case MARSHAL_CLASS:
			fprintf(s, "%s", m->klass.name);
			break;
		case MARSHAL_MODULE:
			fprintf(s, "%s", m->module.name);
			break;
		case MARSHAL_STRING:
			fprintf(s, "\"%s\"", (char *)m->string.data);
			break;
		case MARSHAL_OBJECT:
			fprintf(s, "#<%s:%p ", m->object.klass, m);
			for (i = 0; i < m->object.count; i++)
			{
				marshal_t *sym = m->object.vars[i*2];
				fprintf(s, "%s=", sym->symbol.name);
				marshal_print(m->object.vars[i*2+1], s);
				if (i+1 < m->object.count)
					fprintf(s, ", ");
			}
			fprintf(s, ">");
			break;
		case MARSHAL_USERDEF:
			fprintf(s, "#<%s:%p>", m->userdef.klass,
				m->userdef.data);
			break;
		default:
			fprintf(s, "unknown");
			break;
	}
}
