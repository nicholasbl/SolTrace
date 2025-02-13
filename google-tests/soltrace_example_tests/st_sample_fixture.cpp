#include <gtest/gtest.h>

class sample_file_input : public ::testing::Test
{
protected:
	int code = 0;
	const char* aperture_ex;

	st_context_t cxt = ::st_create_context();

	void SetUp() override
	{
		aperture_ex = "Apertures Example.stinput";

		FILE* fp = fopen(aperture_ex, "r");
		if (!fp)
		{
			printf("failed to open system input file\n");
			code = -1;
		}

		printf("input file: %s\n", aperture_ex);
		if (!read_system(fp, cxt))
		{
			printf("error in input file.\n");
			fclose(fp);
			code = -1;
		}

		fclose(fp);
	}

	void TearDown() override
	{
		delete aperture_ex;
		aperture_ex = nullptr;
	}
};