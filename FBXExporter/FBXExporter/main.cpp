#include "FBXExporter.h"

int main(int argc, char *argv[])
{
	FBXExporter exporter;
	exporter.Process(argv, argc);
	return 0;
}