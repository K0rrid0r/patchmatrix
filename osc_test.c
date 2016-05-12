#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <osc.h>
#include <reader.h>
#include <writer.h>

#define BUF_SIZE 8192

typedef void (*test_t)(LV2_OSC_Writer *writer);

static uint8_t buf0 [BUF_SIZE];
static uint8_t buf1 [BUF_SIZE];

const uint8_t raw_0 [] = {
	'/', 0x0, 0x0, 0x0,
	',', 0x0, 0x0, 0x0
};

const uint8_t raw_1 [] = {
	'/', 'p', 'i', 'n',
	'g', 0x0, 0x0, 0x0,
	',', 'i', 'f', 's',
	0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0xc,
	0x40, 0x59, 0x99, 0x9a,
	'w', 'o', 'r', 'l',
	'd', 0x0, 0x0, 0x0
};

const uint8_t raw_2 [] = {
	'/', 'p', 'i', 'n',
	'g', 0x0, 0x0, 0x0,
	',', 'h', 'd', 'S',
	0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0xc,
	0x40, 0x0b, 0x33, 0x33,
	0x33, 0x33, 0x33, 0x33,
	'h', 't', 't', 'p',
	':', '/', '/', 'e',
	'x', 'a', 'm', 'p',
	'l', 'e', '.', 'c',
	'o', 'm',  0x0, 0x0
};

const uint8_t raw_3 [] = {
	'/', 'p', 'i', 'n',
	'g', 0x0, 0x0, 0x0,
	',', 'T', 'F', 'N',
	'I', 0x0, 0x0, 0x0
};

const uint8_t raw_4 [] = {
	'/', 'm', 'i', 'd',
	'i', 0x0, 0x0, 0x0,
	',', 'm', 0x0, 0x0,
	0x0, 0x90, 24, 0x7f
};

const uint8_t raw_5 [] = {
	'/', 'b', 'l', 'o',
	'b', 0x0, 0x0, 0x0,
	',', 'b', 0x0, 0x0,
	0x0, 0x0, 0x0, 0x6,
	0x1, 0x2, 0x3, 0x4,
	0x5, 0x6, 0x0, 0x0
};

const uint8_t raw_6 [] = {
	'#', 'b', 'u', 'n',
	'd', 'l', 'e', 0x0,
	0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x1,

	0x0, 0x0, 0x0, 0x8,
		'/', 0x0, 0x0, 0x0,
		',', 0x0, 0x0, 0x0
};

const uint8_t raw_7 [] = {
	'#', 'b', 'u', 'n',
	'd', 'l', 'e', 0x0,
	0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x1,

	0x0, 0x0, 0x0, 0x1c,
		'#', 'b', 'u', 'n',
		'd', 'l', 'e', 0x0,
		0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x1,

		0x0, 0x0, 0x0, 0x8,
			'/', 0x0, 0x0, 0x0,
			',', 0x0, 0x0, 0x0,

	0x0, 0x0, 0x0, 0x8,
		'/', 0x0, 0x0, 0x0,
		',', 0x0, 0x0, 0x0
};

static void
_dump(const uint8_t *src, const uint8_t *dst, size_t size)
{
	for(size_t i = 0; i < size; i++)
		printf("%zu %02x %02x\n", i, src[i], dst[i]);
	printf("\n");
}

static void
_clone(LV2_OSC_Reader *reader, LV2_OSC_Writer *writer, size_t size)
{
	if(osc_reader_is_bundle(reader))
	{
		LV2_OSC_Item *itm = OSC_READER_BUNDLE_BEGIN(reader, size);
		assert(itm);

		LV2_OSC_Writer_Frame frame_bndl;
		assert(osc_writer_push_bundle(writer, &frame_bndl, itm->timetag));

		OSC_READER_BUNDLE_ITERATE(reader, itm)
		{
			LV2_OSC_Reader reader2;
			osc_reader_initialize(&reader2, itm->body, itm->size);

			LV2_OSC_Writer_Frame frame_itm;
			assert(osc_writer_push_item(writer, &frame_itm));
			_clone(&reader2, writer, itm->size);
			assert(osc_writer_pop_item(writer, &frame_itm));
		}

		assert(osc_writer_pop_bundle(writer, &frame_bndl));
	}
	else if(osc_reader_is_message(reader))
	{
		LV2_OSC_Arg *arg = OSC_READER_MESSAGE_BEGIN(reader, size);
		assert(arg);

		assert(osc_writer_add_path(writer, arg->path));
		assert(osc_writer_add_format(writer, arg->type));

		OSC_READER_MESSAGE_ITERATE(reader, arg)
		{
			switch((LV2_OSC_Type)*arg->type)
			{
				case LV2_OSC_INT32:
					assert(osc_writer_add_int32(writer, arg->i));
					break;
				case LV2_OSC_FLOAT:
					assert(osc_writer_add_float(writer, arg->f));
					break;
				case LV2_OSC_STRING:
					assert(osc_writer_add_string(writer, arg->s));
					break;
				case LV2_OSC_BLOB:
					assert(osc_writer_add_blob(writer, arg->size, arg->b));
					break;

				case LV2_OSC_INT64:
					assert(osc_writer_add_int64(writer, arg->h));
					break;
				case LV2_OSC_DOUBLE:
					assert(osc_writer_add_double(writer, arg->d));
					break;
				case LV2_OSC_TIMETAG:
					assert(osc_writer_add_timetag(writer, arg->t));
					break;

				case LV2_OSC_TRUE:
				case LV2_OSC_FALSE:
				case LV2_OSC_NIL:
				case LV2_OSC_IMPULSE:
					break;

				case LV2_OSC_MIDI:
					assert(osc_writer_add_midi(writer, arg->size, arg->m));
					break;
				case LV2_OSC_SYMBOL:
					assert(osc_writer_add_symbol(writer, arg->S));
					break;
				case LV2_OSC_CHAR:
					assert(osc_writer_add_char(writer, arg->c));
					break;
				case LV2_OSC_RGBA:
					assert(osc_writer_add_rgba(writer, arg->R, arg->G, arg->B, arg->A));
					break;
			}
		}
	}
}

static void
_test_a(LV2_OSC_Writer *writer, const uint8_t *raw, size_t size)
{
	size_t len;
	assert(osc_writer_finalize(writer, &len) == buf0);
	assert(len == size);
	//_dump(raw, buf0, size);
	assert(memcmp(raw, buf0, size) == 0);

	LV2_OSC_Reader reader;
	osc_reader_initialize(&reader, buf0, size);
	osc_writer_initialize(writer, buf1, BUF_SIZE);

	_clone(&reader, writer, size);

	assert(osc_writer_finalize(writer, &len) == buf1);
	assert(len == size);
	//_dump(raw, buf0, size);
	assert(memcmp(raw, buf1, size) == 0);
}

static void
test_0_a(LV2_OSC_Writer *writer)
{
	assert(osc_writer_message_vararg(writer, "/", ""));
	_test_a(writer, raw_0, sizeof(raw_0));
}

static void
test_1_a(LV2_OSC_Writer *writer)
{
	assert(osc_writer_message_vararg(writer, "/ping", "ifs",
		12, 3.4f, "world"));
	_test_a(writer, raw_1, sizeof(raw_1));
}

static void
test_2_a(LV2_OSC_Writer *writer)
{
	assert(osc_writer_message_vararg(writer, "/ping", "hdS",
		12, 3.4, "http://example.com"));
	_test_a(writer, raw_2, sizeof(raw_2));
}

static void
test_3_a(LV2_OSC_Writer *writer)
{
	assert(osc_writer_message_vararg(writer, "/ping", "TFNI"));
	_test_a(writer, raw_3, sizeof(raw_3));
}

static void
test_4_a(LV2_OSC_Writer *writer)
{
	uint8_t m [] = {0x00, 0x90, 24, 0x7f};
	assert(osc_writer_message_vararg(writer, "/midi", "m", 4, m));
	_test_a(writer, raw_4, sizeof(raw_4));
}

static void
test_5_a(LV2_OSC_Writer *writer)
{
	uint8_t b [] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6};
	assert(osc_writer_message_vararg(writer, "/blob", "b", 6, b));
	_test_a(writer, raw_5, sizeof(raw_5));
}

static void
test_6_a(LV2_OSC_Writer *writer)
{
	LV2_OSC_Writer_Frame frame_bndl, frame_itm;

	assert(osc_writer_push_bundle(writer, &frame_bndl, LV2_OSC_IMMEDIATE));
	{
		assert(osc_writer_push_item(writer, &frame_itm));
		{
			assert(osc_writer_message_vararg(writer, "/", ""));
		}
		assert(osc_writer_pop_item(writer, &frame_itm));
	}
	assert(osc_writer_pop_bundle(writer, &frame_bndl));

	_test_a(writer, raw_6, sizeof(raw_6));
}

static void
test_7_a(LV2_OSC_Writer *writer)
{
	LV2_OSC_Writer_Frame frame_bndl[2], frame_itm[2];

	assert(osc_writer_push_bundle(writer, &frame_bndl[0], LV2_OSC_IMMEDIATE));
	{
		assert(osc_writer_push_item(writer, &frame_itm[0]));
		{
			assert(osc_writer_push_bundle(writer, &frame_bndl[1], LV2_OSC_IMMEDIATE));
			{
				assert(osc_writer_push_item(writer, &frame_itm[1]));
				{
					assert(osc_writer_message_vararg(writer, "/", ""));
				}
				assert(osc_writer_pop_item(writer, &frame_itm[1]));
			}
			assert(osc_writer_pop_bundle(writer, &frame_bndl[1]));
		}
		assert(osc_writer_pop_item(writer, &frame_itm[0]));

		assert(osc_writer_push_item(writer, &frame_itm[0]));
		{
			assert(osc_writer_message_vararg(writer, "/", ""));
		}
		assert(osc_writer_pop_item(writer, &frame_itm[0]));
	}
	assert(osc_writer_pop_bundle(writer, &frame_bndl[0]));

	_test_a(writer, raw_7, sizeof(raw_7));
}

static test_t tests [] = {
	test_0_a,
	test_1_a,
	test_2_a,
	test_3_a,
	test_4_a,
	test_5_a,
	test_6_a,
	test_7_a,

	NULL
};

int
main(int argc, char **argv)
{
	LV2_OSC_Writer writer;

	for(test_t *test=tests; *test; test++)
	{
		test_t cb = *test;

		memset(buf0, 0x0, BUF_SIZE);
		memset(buf1, 0x0, BUF_SIZE);

		osc_writer_initialize(&writer, buf0, BUF_SIZE);

		cb(&writer);
	}

	return 0;
}
