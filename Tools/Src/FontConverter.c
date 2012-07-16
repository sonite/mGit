#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include "Utils.h"
#include "XML.h"
#include "STBImage.h"


/******************************************************************************
 * Convert font character bitmaps and write the results to a C data file
 ******************************************************************************/
enum {
	FONT_MAX_PAGES = 32,
	FONT_MAX_CHARS = 0x100
};

typedef struct {
	const char *name;
	unsigned char *bitmap;
	unsigned width;
	unsigned height;
} font_page_t;

typedef struct {
	unsigned x, y;
	unsigned width, height;
	unsigned xoffset, yoffset;
	unsigned page;

	bool defined;
	const uint8_t *bitmap;
	unsigned long origin;
} font_char_t;

typedef struct {
	const char *face;
	int xmin, ymin;
	int xmax, ymax;
	int width;
	int height;
	font_page_t pages[FONT_MAX_PAGES];
	font_char_t chars[FONT_MAX_CHARS];
} font_t;

static void font_clear(font_t *fnt) {
	memset(fnt, 0, sizeof *fnt);
	fnt->xmin = fnt->ymin = INT_MAX;
}

static void font_cleanup(font_t *fnt) {
	size_t index;

	for(index = 0; index < FONT_MAX_PAGES; ++index)
		free(fnt->pages[index].bitmap);
}

static void font_emit(font_t *fnt, const char *path, const char *name) {
	size_t index;
	char filename[FILENAME_MAX];

	size_t space = fnt->width * fnt->height;
	int align = space % 5;
	unsigned char *bitmap = alloca(space);
	size_t char_size = (space + 4) / 5;
	unsigned char_lim = 0;

	// Process the characters
	for(index = 0; index < FONT_MAX_CHARS; ++index) {
		font_char_t *ch = &fnt->chars[index];
		font_page_t *pg = &fnt->pages[ch->page];

		if(!ch->defined)
			continue;

		char_lim = index + 1;

		// Load the page
		if(!pg->name)
			err_throw("Page #%u has not been defined", ch->page);
		else if(!pg->bitmap) {
			unsigned int comp;

			safe_snprintf(filename, sizeof filename, "%s%s%s", path, name, pg->name);
			pg->bitmap = stbi_load(filename, &pg->width, &pg->height, &comp,
				STBI_grey);
			if(!pg->bitmap) {
				safe_snprintf(filename, sizeof filename, "%s%s", path, pg->name);
				pg->bitmap = stbi_load(filename, &pg->width, &pg->height, &comp,
					STBI_grey);
				if(!pg->bitmap) {
					err_throw("Cannot read bitmap '%s' (%s)", filename,
						stbi_failure_reason());
				}
			}
		}

		if(ch->x + ch->width > pg->width || ch->y + ch->height > pg->height)
			err_throw("Character #%u is outside of the image", index);

		// Extract the character's pixels into the fixed cell
		{
			const unsigned char *src = &pg->bitmap[ch->x + ch->y * pg->width];
			unsigned char *dst = &bitmap[(ch->xoffset - fnt->xmin) + (ch->yoffset - fnt->ymin) * fnt->width];
			unsigned len = ch->height;

			bitmap = memset(bitmap, 0, space);

			while(len--)
			{
				dst = memcpy(dst, src, ch->width);
				dst += fnt->width;
				src += pg->width;
			}
		}

		// Gather the pixels into bits in y-major order
		{
			int x, y;

			unsigned char value;
			unsigned scale;
			unsigned dither = 0;

			unsigned char *dst = alloca(char_size);
			ch->bitmap = dst;

			switch(align) {
			case 0: scale = 81; value =   0; break;
			case 1: scale =  1; value =   0; break;
			case 2: scale =  3; value =  -9; break;
			case 3: scale =  9; value =   0; break;
			case 4: scale = 27; value = -81; break;
			}

			for(x = 0; x < fnt->width; ++x) {
				const unsigned char *src = &bitmap[x];

				for(y = 0; y < fnt->height; ++y) {
					unsigned quant;

					/*
					dither += *src;
					if(dither >= 255)
						dither = 255;
					quant = dither / 86;
					dither %= 86;
					*/
					quant = *src / 86;

					switch(scale) {
					// case 81:
					case 27:
					// case 9:
					case 3:
					// case 1:
						quant = 2 - quant;
					}
					value += quant * scale;

					if(scale /= 3, !scale) {
						*dst++ = value;
						value = 0;
						scale = 81;
					}

					src += fnt->width;
				}
			}
		}
	}

	// Write results to file
	{
		unsigned long origin;
		const unsigned char *previous;
		const char *prefix;

		safe_snprintf(filename, sizeof filename, "%s%s.c", path, name);
		if(!freopen(filename, "w", stdout))
			err_throw("Cannot create file '%s'\n", filename);

		// First write the character bitmaps
		printf (
			"#include \"Font.h\"\n"
			"\n"
			"static const rom unsigned char Bitmap[] =\n"
			"{"
		);

		origin = 0;
		previous = NULL;
		prefix = "\n\t";

		for(;;) {
			size_t overlap;
			unsigned columns;

			// Make a greedy search for the next char with the largest overlap
			font_char_t *best_char = NULL;
			size_t best_overlap = 0;

			for(index = 0; index < FONT_MAX_CHARS; ++index) {
				font_char_t *ch = &fnt->chars[index];

				if(!ch->bitmap)
					continue;

				if(!best_overlap)
					best_char = ch;

				if(previous) {
					overlap = char_size;

					while(overlap > best_overlap) {
						if(!memcmp(&previous[char_size - overlap], ch->bitmap, overlap)) {
							best_char = ch;
							best_overlap = overlap;
							break;
						}

						--overlap;
					}
				}
			}

			if(!best_char)
					break;

			// Write the non-overlapping bytes
			index = best_char - fnt->chars;
			best_char->origin = origin - best_overlap;

			printf (
				"%s/* '%c' @ %lu:%+d */",
				prefix,
				isprint(index) ? (char) index : '.',
				best_char->origin,
				(int) best_overlap
			);

			prefix = "\n\t";
			columns = 0;

			for(index = best_overlap; index < char_size; ++index) {
				printf("%s0x%02X", prefix, best_char->bitmap[index]);

				prefix = ", ";
				if(++columns == 8) {
					prefix = ",\n\t";
					columns = 0;
				}

				++origin;
			}

			if(best_overlap != char_size)
				prefix = ",\n\t";

			previous = best_char->bitmap;
			best_char->bitmap = NULL;
		}

		// Write the index
		printf (
			"\n"
			"};\n"
			"\n"
			"static const rom unsigned char *const rom Charset[] =\n"
			"{\n"
		);

		prefix = "";

		for(index = 0; index < char_lim ; ++index) {
			printf (
				"%s\t/* '%c' */ &Bitmap[%lu]",
				prefix,
				isprint(index) ? (char) index : '.',
				fnt->chars[index].origin
			);
			prefix = ",\n";
		}

		// Write the font header
		printf (
			"\n"
			"};\n"
			"\n"
			"/* %lu bytes */\n"
			"const rom struct Font %s =\n"
			"{\n"
			"\t/* XRes  */ %u,\n"
			"\t/* YRes  */ %u,\n"
			"\t/* Size  */ %u,\n"
			"\t/* Align */ %+d,\n"
			"\t/* Limit */ %d,\n"
			"\t/* Chars */ Charset\n"
			"};",
			(unsigned long) (5 + 2 * (1 + char_lim) + origin),
			name,
			fnt->width,
			fnt->height,
			(unsigned) (space + 4) / 5,
			(align + 3) % 5U,
			char_lim - 1
		);

		/*
		0 => +3: psc_pixel0
		1 => -1: psc_pixel4
		2 =>  0: psc_pixel3
		3 => +1: psc_pixel2
		4 => +2: psc_pixel1
		*/
	}
}


/******************************************************************************
 * Parse the font's XML description file into a font structure
 ******************************************************************************/
static long _parse_integer(const char *attrib) {
	char *end;
	long value = strtol(attrib, &end, 0);

	if(end == attrib || *end)
		err_throw("'%s' is not an integer", attrib);

	return value;
}

static void _parse_font(xml_file_t *xml, font_t *fnt) {
	for(;;) {
		xml_value_t val;

		switch(xml_tags(xml, &val, "info", "common", "pages", "chars", NULL)) {
		// Info
		case 0:
			{
				const char *face = "face";
				const char *size = "size";
				const char *bold = "bold";
				const char *italic = "italic";
				const char *charset = "charset";
				const char *unicode = "unicode";
				const char *stretchh = "stretchH";
				const char *smooth = "smooth";
				const char *aa = "aa";
				const char *padding = "padding";
				const char *spacing = "spacing";
				const char *outline = "outline";

				xml_attribs(xml, &face, xml_optional, &size, &bold, &italic,
					&charset, &unicode, &stretchh, &smooth, &aa, &padding, &spacing,
					&outline, &stretchh, &smooth, &aa, &padding, &spacing, &outline,
					NULL);
				xml_end(xml);

				fnt->face = face;
			}
			continue;

		// Common
		case 1:
			{
				const char *lineheight = "lineHeight";
				const char *base = "base";
				const char *scaleW = "scaleW";
				const char *scaleH = "scaleH";
				const char *pages = "pages";
				const char *packed = "packed";
				const char *alphachnl = "alphaChnl";
				const char *redchnl = "redChnl";
				const char *greenchnl = "greenChnl";
				const char *bluechnl = "blueChnl";

				xml_attribs(xml, xml_optional, &lineheight, &base, &scaleW,
					&scaleH, &pages, &packed, &alphachnl, &redchnl, &greenchnl,
					&bluechnl, NULL);
				xml_end(xml);
			}
			continue;

		// Pages
		case 2:
			{
				xml_value_t page_val;
				while(xml_tags(xml, &page_val, "page", NULL) >= 0) {
					const char *id = "id";
					const char *file = "file";
					size_t index;

					xml_attribs(xml, &id, &file, NULL);

					index = _parse_integer(id);
					if(index >= FONT_MAX_PAGES)
						err_throw("Too many font pages");
					if(fnt->pages[index].name)
						err_throw("Font page #%u redefined", index);

					fnt->pages[index].name = file;

					xml_end(xml);
				}
			}
			continue;

		// Chars
		case 3:
			{
				xml_value_t char_val;

				const char *count = "count";
				xml_attribs(xml, xml_optional, &count, NULL);

				while(xml_tags(xml, &char_val, "char", NULL) >= 0) {
					size_t index;
					font_char_t *ch;
					int edge;

					const char *id = "id";
					const char *x = "x";
					const char *y = "y";
					const char *width = "width";
					const char *height = "height";
					const char *xoffset = "xoffset";
					const char *yoffset = "yoffset";
					const char *page = "page";
					const char *xadvance = "xadvance";
					const char *chnl = "chnl";

					xml_attribs(xml, &id, &x, &y, &width, &height, &xoffset,
						&yoffset, &page, xml_optional, &xadvance, &chnl, NULL);

					index = _parse_integer(id);
					if(index >= FONT_MAX_CHARS)
						err_throw("Too many characters");
					ch = &fnt->chars[index];

					ch->x = _parse_integer(x);
					ch->y = _parse_integer(y);
					ch->width = _parse_integer(width);
					ch->height = _parse_integer(height);
					ch->xoffset = _parse_integer(xoffset);
					ch->yoffset = _parse_integer(yoffset);
					ch->page = _parse_integer(page);
					ch->defined = true;

					if(ch->page >= FONT_MAX_PAGES)
						err_throw("Invalid page index");

					if(edge = ch->xoffset, fnt->xmin > edge)
						fnt->xmin = edge;
					if(edge += ch->width, fnt->xmax < edge)
						fnt->xmax = edge;
					if(edge = ch->yoffset, fnt->ymin > edge)
						fnt->ymin = edge;
					if(edge += ch->height, fnt->ymax < edge)
						fnt->ymax = edge;

					xml_end(xml);
				}
			}

			fnt->width = fnt->xmax - fnt->xmin;
			fnt->height = fnt->ymax - fnt->ymin;

			xml_end(xml);
			continue;
		}

		break;
	}
}

static void parse(xml_file_t *xml, font_t *fnt) {
	for(;;) {
		xml_value_t val;

		switch(xml_tags(xml, &val, "xml", "font", NULL)) {
		case 0:
			{
				const char *version = "version";
				xml_attribs(xml, xml_optional, &version, NULL);
				xml_end(xml);
			}
			continue;

		case 1:
			_parse_font(xml, fnt);
			continue;
		}

		break;
	}
}


/******************************************************************************
 * Parse the command line and safe-guard the conversion
 ******************************************************************************/
int DECL_VARARGS main(int argc, char *argv[]) {
	xml_file_t xml;
	font_t font;
	char namebuf[FILENAME_MAX];

	volatile int status = EXIT_FAILURE;
	char *name = argv[1];

	if(argc != 2) {
		err_format("syntax: %s {font}\n", argv[0]);
	} else if(xml_open(&xml, name)) {
		err_frame_t frame;
		font_clear(&font);

		if(err_try_in(&frame, xml_context, &xml)) {
			parse(&xml, &font);

			// Extract the relative input path and the file name without extension
			{
				char *path = name;
				char *file = name;
				char *ext = NULL;

				char ch;

				while(ch = *path++, ch) {
					if(ch == '.')
						ext = path;
					else if(ch == '/' || ch =='\\' || ch == ':') {
						file = path;
						ext = NULL;
					}
				}

				if(ext)
					*--ext = '\0';
				safe_snprintf(namebuf, sizeof namebuf, "%s", file);
				*file = '\0';
			}

			font_emit(&font, name, namebuf);
			err_unwind();

			status = EXIT_SUCCESS;
		}

		font_cleanup(&font);
		xml_close(&xml);
	}

	if(status != EXIT_SUCCESS)
		fprintf(stderr, "%s\n", err_message);
	return status;
}