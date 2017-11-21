/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <marshal/marshal.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	marshal_t *m;
	if (argc < 2)
	{
		fprintf(stderr, "Please provide a file as argument.\n");
		return 1;
	}

	m = marshal_decode_file(argv[1]);
	if (m)
	{
		marshal_print(m, NULL);
		printf("\n");
		marshal_free(m);
	}
	else
		fprintf(stderr, "Failed to decode.\n");
	return 0;
}
