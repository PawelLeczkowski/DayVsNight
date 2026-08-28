#include <stdio.h>
#include <time.h>
#include <wchar.h>

#include "pcl_Unicode.h"
#include "pcl/pcl.h"

struct position {
	int row, column, row_speed, column_speed;
};

int main(void) {
	struct Console* console = start();
	struct UnicodeScreen * unicode = initunicode(console);
	setinputblock(console, FALSE);
	hidecursorunicode(unicode);
	srand(time(NULL));

	const unsigned int width = 40;
	const unsigned int height = 10;

	struct position white;
	white.row = rand() % (height / 2);
	white.column = rand() % (width / 2);

	do {
		white.row_speed = rand() % 3 - 1;
		white.column_speed = rand() % 3 - 1;
	} while (white.row_speed == 0 && white.column == 0);

	struct position black;
	black.row = rand() % (height / 2);
	black.column = rand() % (width / 2) + width / 2;

	do {
		black.row_speed = rand() % 3 - 1;
		black.column_speed = rand() % 3 - 1;
	} while (black.row_speed == 0 && black.column_speed == 0);

	wchar_t array[width * height];
	// filling 2 sides
	for (int i = 0; i < width * height; i += width) {
		wmemset(array + i, L'#', width / 2);
	}
	for (int i = width / 2; i < width * height; i += width) {
		wmemset(array + i, L'.', width / 2);
	}

	while (TRUE) {
		const int input = getcharacter(console);
		if (input == 'q') {
			break;
		}

		// white movement
		int next_w_row = white.row + white.row_speed;
		int next_w_col = white.column + white.column_speed;

		// white wall bounce
		if (next_w_row < 0 || next_w_row >= height) {
			white.row_speed *= -1;
			next_w_row = white.row + white.row_speed;
		}
		if (next_w_col < 0 || next_w_col >= width) {
			white.column_speed *= -1;
			next_w_col = white.column + white.column_speed;
		}

		// white clamp safety
		if (next_w_row < 0) {
			next_w_row = 0;
		}
		if (next_w_row >= height) {
			next_w_row = height - 1;
		}
		if (next_w_col < 0) {
			next_w_col = 0;
		}
		if (next_w_col >= width) {
			next_w_col = width - 1;
		}

		// white color change
		if (array[next_w_row * width + next_w_col] == L'.') {
			array[next_w_row * width + next_w_col] = L'#';
			white.column_speed *= -1;
			white.row_speed *= -1;

			next_w_row = white.row;
			next_w_col = white.column;
		}
		white.row = next_w_row;
		white.column = next_w_col;

		// black movement
		int next_b_row = black.row + black.row_speed;
		int next_b_col = black.column + black.column_speed;

		// black wall bounce
		if (next_b_row < 0 || next_b_row >= height) {
			black.row_speed *= -1;
			next_b_row = black.row + black.row_speed;
		}
		if (next_b_col < 0 || next_b_col >= width) {
			black.column_speed *= -1;
			next_b_col = black.column + black.column_speed;
		}

		// black clamp safety
		if (next_b_row < 0) {
			next_b_row = 0;
		}
		if (next_b_row >= height) {
			next_b_row = height - 1;
		}
		if (next_b_col < 0) {
			next_b_col = 0;
		}
		if (next_b_col >= width) {
			next_b_col = width - 1;
		}

		// black color change
		if (array[next_b_row * width + next_b_col] == L'#') {
			array[next_b_row * width + next_b_col] = L'.';
			black.column_speed *= -1;
			black.row_speed *= -1;

			next_b_row = black.row;
			next_b_col = black.column;
		}
		black.row = next_b_row;
		black.column = next_b_col;

		set2darrayunicode(unicode, array, 0, 0, width, height);

		setcharcursorunicode(unicode, L".", white.row, white.column);
		setcharcursorunicode(unicode, L"#", black.row, black.column);

		unsigned int day = 0, night = 0;
		for (int i = 0; i < width * height; ++i) {
			array[i] == '.' ? day++ : night++;
		}

		setstringformattedcursorunicode(unicode, height, 0, L"Day: %d | Night: %d", day, night);

		refreshunicode(console, unicode);

		_sleep(100);
	}

	end(console);
	return 0;
}
