/******************************************************************************
 * A stupid little XML parser
 ******************************************************************************/
#ifndef _XML_H
#define _XML_H

typedef enum {
	XML_TAG,
	XML_ATTRIB,
	XML_DATA,
	XML_END,
	XML_BODY
} xml_token_t;

typedef union xml_value_t {
	struct {
		const char *tag;
		union xml_value_t *parent;
	};
	struct {
		char *attrib;
		char *value;
	};
	char *data;
} xml_value_t;

typedef struct xml_file_t {
	enum {
		_XML_ST_BODY,
		_XML_ST_OPEN,
		_XML_ST_CLOSE,
		_XML_ST_ATTRIB,
		_XML_ST_EOF,
	} state;

	char *ptr;
	xml_value_t *stack;

	char *f_buffer;
	const char *f_name;
	unsigned f_lineno;
	unsigned f_latched_lineno;
} xml_file_t;

bool xml_open(xml_file_t *p, const char *name);
void xml_close(xml_file_t *p);
char *xml_context(xml_file_t *p, char *ptr, char *end);
xml_token_t xml_get(xml_file_t *p, xml_value_t *val);
void DECL_NO_RETURN xml_unexpected(xml_file_t *p, xml_token_t tok,
	const xml_value_t *val);
int DECL_VARARGS xml_tags(xml_file_t *p, xml_value_t *val, ...);
void DECL_VARARGS xml_attribs(xml_file_t *p, ...);
void xml_end(xml_file_t *p);

#define xml_optional (&_xml_optional)
extern const char *_xml_optional;

#endif