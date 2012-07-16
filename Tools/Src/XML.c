#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include "utils.h"
#include "XML.h"


bool xml_open(xml_file_t *p, const char *name) {
	p->f_buffer = file_open(name);

	p->state = _XML_ST_BODY;
	p->ptr = p->f_buffer;
	p->stack = NULL;
	p->f_name = name;
	p->f_lineno = 1;
	p->f_latched_lineno = 1;

	return !!p->f_buffer;
}

void xml_close(xml_file_t *p) {
	file_close(p->f_buffer);
}

// Report the location of the most recently parsed token
char *xml_context(xml_file_t *p, char *buf, char *end) {
	const xml_value_t *stack;

	buf += safe_snprintf(buf, end - buf, "[%s:%u", p->f_name, p->f_latched_lineno);
	if(stack = p->stack, stack) {
		char separator = ' ';
		do {
			buf += safe_snprintf(buf, end - buf, "%c%s", separator, stack->tag);
			separator = '.';
		} while(stack = stack->parent, stack);
	}
	buf += safe_snprintf(buf, end - buf, "] ");
	return buf;
}

// Eat whitespace
static char *_xml_whitespace(xml_file_t *p, char *ptr) {
	char ch;

	while(ch = *ptr, isspace(ch)) {
		if(ch == '\n')
			++p->f_lineno;
		++ptr;
	}

	return ptr;
}

// Replace special characters after and ampersand has been matched
static char *_xml_escape(xml_file_t *p, char *ptr, char *out) {
	static const struct {
		const char *key;
		char value;
	} table[] = {
		{ "amp;",  '&'  },
		{ "apos;", '\'' },
		{ "quot;", '\"' },
		{ "lt;",   '<'  },
		{ "gt;",   '>'  },
		{ NULL,    '\0' }
	}, *seek;

	for(seek = table; seek->key; ++seek) {
		char *cmp = ptr;
		const char *key = seek->key;

		while(*cmp++ == *key++) {
			if(!*key) {
				*out = seek->value;
				return cmp;
			}
		}
	}

	err_throw("Unknown or malformed special character");
}

// Accept valid identifier characters
static char *_xml_identifier(xml_file_t *p, char *ptr) {
	char ch;
	while(ch = *ptr, isalnum(ch) || ch == '_')
		++ptr;
	return ptr;
}

// The tag name needs to be terminated immediately so check whether the next
// character is valid before clearing it, and defer the handling of it
static void _xml_defer_inner(xml_file_t *p, char *ptr) {
	char *org = ptr;
	ptr = _xml_whitespace(p, ptr);

	switch(*ptr++) {
	case '>':
		p->state = _XML_ST_BODY;
		break;
	case '/':
	case '?':
		p->state = _XML_ST_CLOSE;
		break;
	default:
		p->state = _XML_ST_ATTRIB;
		if(--ptr == org)
			err_throw("Invalid character in tag, attribute expected");
		break;
	}

	p->ptr = ptr;
}

// Parse the start of a tag after the leading '<'
static xml_token_t _xml_open_tag(xml_file_t *p, xml_value_t *val, char *ptr) {
	char ch;

	// Try to get a tag name
	val->tag = ptr;
	ptr = _xml_identifier(p, ptr);

	// Eat comments
	if(val->tag == ptr) {
		if(ptr[0] != '!' || ptr[1] != '-' || ptr[2] != '-')
			err_throw("Tag name expected");
		ptr += 3;

		do {
			ch = *ptr++;

			if(ch == '\n')
				++p->f_lineno;
			else if(ch == '\0')
				err_throw("Unexpected end of file in comment");
		} while(ch != '-' || ptr[0] != '-' || ptr[1] != '>');

		ptr += 2;
		p->ptr = ptr;
		p->state = _XML_ST_BODY;
		return xml_get(p, val);
	}

	// Push the new tag onto the stack
	val->parent = p->stack;
	p->stack = val;

	// Process the next char ahead of time and then terminate the tag
	_xml_defer_inner(p, ptr);
	*ptr = '\0';
	return XML_TAG;
}

// Close the currently open tag
static xml_token_t _xml_close_implicit_tag(xml_file_t *p, xml_value_t *val, char *ptr) {
	val->tag = p->stack->tag;

	if(ptr = _xml_whitespace(p, ptr), *ptr++ != '>')
		err_throw("Expected '>' to close tag '%s'", val->tag);

	p->stack = p->stack->parent;
	p->ptr = ptr;
	p->state = _XML_ST_BODY;
	return XML_END;
}

// Parse a named tag terminator
static xml_token_t _xml_close_named_tag(xml_file_t *p, xml_value_t *val, char *ptr) {
	const char *tag;

	tag = ptr;
	ptr = _xml_identifier(p, ptr);

	if(ptr == val->tag)
		err_throw("Tag name expected");
	else if(!p->stack)
		err_throw("No open tag to close");
	else if(memcmp(tag, p->stack->tag, ptr - tag)) {
		err_throw("Attempting to close the wrong tag, '%s' expected",
			p->stack->tag);
	}

	return _xml_close_implicit_tag(p, val, ptr);
}

// Parse attribute definition
static xml_token_t _xml_attrib(xml_file_t *p, xml_value_t *val, char *ptr) {
	char *value;
	char ch;

	val->attrib = ptr;
	ptr = _xml_identifier(p, ptr);

	if(ptr == val->attrib)
		err_throw("Invalid character, attribute expected");

	value = ptr;
	ptr = _xml_whitespace(p, ptr);
	ch = *ptr++;
	*value = '\0';
	if(ch != '=')
		err_throw("Missing '=' for attribute '%s'", val->attrib);

	ptr = _xml_whitespace(p, ptr);
	ch = *ptr++;
	if(ch != '"')
		err_throw("Missing '\"' for attribute '%s'", val->attrib);

	val->value = value = ptr;

	while(ch = *ptr++, ch != '"') {
		if(iscntrl(ch)) {
			err_throw("Invalid character in value of attribute '%s'", val->attrib);
		} else if(ch == '&')
			ptr = _xml_escape(p, ptr, value++);
		else
			*value++ = ch;
	}

	*value = '\0';

	_xml_defer_inner(p, ptr);
	return XML_ATTRIB;
}

// Parse the next element
xml_token_t xml_get(xml_file_t *p, xml_value_t *val) {
	char *ptr = p->ptr;

	p->f_latched_lineno = p->f_lineno;

	switch(p->state) {
		char ch;
		char *data;

	// Look for the start of a tag and return any intermediate data
	case _XML_ST_BODY:
		val->data = data = ptr;

		while(ch = *ptr++, ch) {
			if(ch == '<') {
				// Flush data
				if(data != val->data) {
					*data = '\0';
					p->ptr = ptr;
					p->state = _XML_ST_OPEN;
					return XML_DATA;
				}

				// Start or end of tag?
	case _XML_ST_OPEN:
				ptr = _xml_whitespace(p, ptr);
				if(ch = *ptr, ch == '/')
					return _xml_close_named_tag(p, val, ++ptr);
				else if(ch == '?')
					++ptr;
				return _xml_open_tag(p, val, ptr);
			} else if(ch == '&') {
				ptr = _xml_escape(p, ptr, data++);
			} else {
				*data++ = ch;
				if(ch == '\n')
					++p->f_lineno;
			}
		}

		// Clean up at the end of a file
		p->ptr = ptr;
		p->state = _XML_ST_EOF;
		if(data != val->data)
			return XML_DATA;
		if(p->stack)
			err_throw("Unexpected end of file inside tag '%s'", p->stack->tag);
		return XML_END;

	case _XML_ST_CLOSE:
		return _xml_close_implicit_tag(p, val, ptr);

	case _XML_ST_ATTRIB:
		return _xml_attrib(p, val, ptr);

	case _XML_ST_EOF:
		return XML_END;

	DECL_NO_DEFAULT
	}
}

// Report unhandled token
void xml_unexpected(xml_file_t *p, xml_token_t tok, const xml_value_t *val) {
	switch(tok) {
	case XML_TAG:    err_throw("Unexpected tag '%s'", val->tag);
	case XML_ATTRIB: err_throw("Unexpected attribute '%s'", val->attrib);
	case XML_DATA:   err_throw("Unexpected data");
	case XML_END:    err_throw("Unexpected end of file");
	DECL_NO_DEFAULT
	}
}

// Accept only tag tokens and only from the supplied range, and return the
// index of the identified tag (or minus one)
int DECL_VARARGS xml_tags(xml_file_t *p, xml_value_t *val, ...) {
	xml_token_t tok;
	va_list args;
	int index;
	const char *tag;

retry:
	switch(tok = xml_get(p, val), tok) {
		const char *data;
		char ch;

	// Don't accept attributes or (non-empty) data blocks
	case XML_DATA:
		data = val->data;
		do {
			ch = *data++;
			if(!ch)
				goto retry;
		} while(isspace(ch));
	case XML_ATTRIB:
		xml_unexpected(p, tok, val);

	case XML_END:
		return -1;
	case XML_TAG:
		break;

	DECL_NO_DEFAULT
	}

	index = -1;

	va_start(args, val);
	do {
		++index;
		if(tag = va_arg(args, const char *), !tag)
			xml_unexpected(p, tok, val);
	} while(strcmp(tag, val->tag));
	va_end(args);

	return index;
}

// Helper function used to parse a mandatory set of attributes for a tag
const char *_xml_optional;

void DECL_VARARGS xml_attribs(xml_file_t *p, ...) {
	unsigned long set = 0;
	va_list args;
	char **field;
	bool optional;

	while(p->state == _XML_ST_ATTRIB) {
		xml_value_t token;
		unsigned long bit;

		xml_get(p, &token);

		va_start(args, p);
		bit = 1;
		optional = false;

		for(;;) {
 			if(field = va_arg(args, char **), !field)
				xml_unexpected(p, XML_ATTRIB, &token);
			if(field == xml_optional) {
				optional = true;
				continue;
			}
			if(!(set & bit) && !strcmp(token.attrib, *field))
				break;
			bit <<= 1;
		}

		set |= bit;
		*field = token.value;
		va_end(args);
	}

	optional = false;
	va_start(args, p);
	while(field = va_arg(args, char **), field) {
		if(field == xml_optional) {
			optional = true;
			continue;
		}
		if(!(set & 1)) {
			if(!optional)
				err_throw("Missing '%s' attribute", *field);
			else
				*field = NULL;
		}
		set >>= 1;
	}
	va_end(args);
}

// Leave a tag and make sure there's nothing more in there
void xml_end(xml_file_t *p) {
	xml_value_t val;
	xml_tags(p, &val, NULL);
}