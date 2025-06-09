//strace.h
#include "../stapi.h"

static void read_line(char* buf, int len, FILE* fp);

static void ZeroTransform(double ref[3][3], double loc[3][3], double eu[3]);

static bool read_sun(FILE* fp, st_context_t cxt);

bool read_optic_surface(FILE* fp, st_context_t cxt, int iopt, int fb);

bool read_optic(FILE* fp, st_context_t cxt);

bool read_element(FILE* fp, st_context_t cxt, int istage, const char* base_dir = "");

bool read_stage(FILE* fp, st_context_t cxt, const char* base_dir = "");

bool read_system(FILE* fp, st_context_t cxt, const char* base_dir = "");

bool write_data_file(const char* file, st_context_t cxt);

int trace_progress(st_uint_t ntracedtotal, st_uint_t ntraced, st_uint_t ntotrace, st_uint_t curstage, st_uint_t nstages, void *data);