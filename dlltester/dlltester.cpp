// dlltester.cpp: This file contains the "main" function. Program execution will start and end here.
//

#include "pch.h"
#include "pck_handle.h"
#include <iostream>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include <thread>
#include <chrono>
#include <atomic>

#define RUN_OPEN_ZUP	0
#define RUN_LIST		0
#define RUN_EXTRACT		0
//#define RUN_CREATE		1
//#define RUN_UPDATE		1
//#define RUN_RENAME		1
//#define RUN_DELETE		1
//#define RUN_REBUILD		1
#define RUN_REBUILD_FILTER		0


#define TEST_BASE_DIR					L"d:\\pcktest\\"
#define TEST_SRC_DIR					L"d:\\pcktest\\src\\"
#define TEST_DST_DIR					L"d:\\pcktest\\dst\\"
#define TEST_SRC_DIR_EXTRACT_ALL_SRC	L"d:\\pcktest\\src\\configs_lug.pck"
#define TEST_SRC_DIR_REBUILD_FILTER_SRC	L"d:\\pcktest\\src\\gfx.pck"
#define TEST_SRC_DIR_LIST_SRC			L"d:\\pcktest\\src\\configs_key.pck"
#define TEST_SRC_DIR_LIST_SRC2			L"d:\\pcktest\\src\\configs_lug.pck"
#define TEST_SRC_DIR_LIST_SRC_ZUP		L"d:\\pcktest\\src\\ec_patch_1372-1378.zup"

#define TEST_SRC_DIR_REBUILD_FILTER		L"d:\\pcktest\\src\\test_script.txt"

#define TEST_DST_DIR_EXTRACT_ALL		L"d:\\pcktest\\dst\\extractAllTest\\"
#define TEST_DST_DIR_PCK				L"d:\\pcktest\\dst\\created.pck"
#define TEST_DST_DIR_REBUILD_NOC		L"d:\\pcktest\\dst\\rebuild_test.pck"
#define TEST_DST_DIR_REBUILD_WC7		L"d:\\pcktest\\dst\\rebuild_test_c7.pck"
#define TEST_DST_DIR_REBUILD_WC12		L"d:\\pcktest\\dst\\rebuild_test_c12.pck"
#define TEST_DST_DIR_REBUILD_FGFX		L"d:\\pcktest\\dst\\rebuild_gfx_c12.pck"

int nVersionCount = 0;

atomic<BOOL> g_exit_show = FALSE;

int show_progress()
{
	char		szMTMemoryUsed[12], szMTMaxMemory[12];
	int			bPrintEnter = 1;

	while (!g_exit_show) {

		while (!pck_isThreadWorking()) {

			if (bPrintEnter) {
				bPrintEnter = 0;
				printf("\n");
			}


			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			if (g_exit_show)
				return 0;
		}

		bPrintEnter = 1;

		uint32_t		dwUIProgress = pck_getUIProgress();
		uint32_t		dwUIProgressUpper = pck_getUIProgressUpper();
		uint32_t		dwMTMemoryUsed = pck_getMTMemoryUsed();
		uint32_t		dwMTMaxMemory = pck_getMTMaxMemory();

		if (0 == dwUIProgress)
			continue;

		if (0 == dwUIProgressUpper)
			dwUIProgressUpper = 1;

		int iNewPos = (int)((dwUIProgress << 10) / dwUIProgressUpper);

		printf(
			"progress: %u/%u %.1f%% cache:%s/%s %.1f%%\t\t\t\t\r",
			dwUIProgress,
			dwUIProgressUpper,
			dwUIProgress * 100.0 / dwUIProgressUpper,
			StrFormatByteSize64A(dwMTMemoryUsed, szMTMemoryUsed, 12),
			StrFormatByteSize64A(dwMTMaxMemory, szMTMaxMemory, 12),
			(dwMTMemoryUsed >> 10) * 100.0 / (dwMTMaxMemory >> 10));

		std::this_thread::sleep_for(std::chrono::milliseconds(200));

	}

	return 0;
}


//Version list
int list_version()
{
	nVersionCount = pck_getVersionCount();
	printf("nVersionCount = %d\n", nVersionCount);

	for (int i = 0; i < nVersionCount; i++) {
		wprintf(L"id=%d, name=%s\n", i, pck_getVersionNameById(i));
	}
	return nVersionCount;
}

int main()
{
	setlocale(LC_CTYPE, ".936");

    std::cout << "Hello World!\n"; 

	std::thread t_process(show_progress);

	//Set maximum memory
	pck_setMTMaxMemory(200*1024*1024);

	//Version list
	list_version();
	//pck_addVersionAlgorithmId(232, 0x00020002);
	list_version();

#if RUN_OPEN_ZUP

	printf("\nlist zup\n");
	do_listPathInPck(TEST_SRC_DIR_LIST_SRC_ZUP, L"", NULL, NULL);
	do_listPathInPck(TEST_SRC_DIR_LIST_SRC_ZUP, L"element", NULL, NULL);

#endif

#if RUN_LIST

	printf("\nlist root\n");
	do_listPathInPck(TEST_SRC_DIR_LIST_SRC, L"", NULL, NULL);
	do_listPathInPck(TEST_SRC_DIR_LIST_SRC2, L"", NULL, NULL);

#endif

#define RUN_TESTCASE(instruction, function, success_rtn, ...) \
	printf("\n"##instruction"...\n");\
	printf("\nsuccess = %s\n", success_rtn == ##function(__VA_ARGS__) ? "TRUE" : "FALSE");\
	printf("\n"##instruction" done\n");

#if RUN_EXTRACT

	printf("\n==================================================\n");

	RUN_TESTCASE("unpack all from pck", do_ExtractAllFiles, WINPCK_OK, TEST_SRC_DIR_EXTRACT_ALL_SRC, TEST_DST_DIR_EXTRACT_ALL);

	printf("\nlist root\n");
	do_listPathInPck(TEST_SRC_DIR_EXTRACT_ALL_SRC, L"", NULL, NULL);


#endif 

#if RUN_CREATE
	printf("\n==================================================\n");

	RUN_TESTCASE("pack all to pck", do_CreatePckFile, WINPCK_OK, TEST_DST_DIR_EXTRACT_ALL"gfx", TEST_DST_DIR_PCK, 0, 12);

#endif
#if RUN_UPDATE

	printf("\n==================================================\n");
	printf("\nlist root\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"", NULL, NULL);

	RUN_TESTCASE("add a big file(0.bmp) to root node", do_AddFileToPckFile, WINPCK_OK, TEST_SRC_DIR"0.bmp", TEST_DST_DIR_PCK, L"");

	printf("\nlist root\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"", NULL, NULL);

	printf("\n==================================================\n");

	RUN_TESTCASE("add a folder(smallfile) to root node", do_AddFileToPckFile, WINPCK_OK, TEST_SRC_DIR"smallfile", TEST_DST_DIR_PCK, L"", 12);

	printf("\nlist root\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"", NULL, NULL);

	printf("\n==================================================\n");

	RUN_TESTCASE("add a folder(xyz) to root node", do_AddFileToPckFile, WINPCK_OK, TEST_SRC_DIR"xyz", TEST_DST_DIR_PCK, L"");

	printf("\nlist root\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"", NULL, NULL);

	printf("\n==================================================\n");

	printf("\nlist xyz\\\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"xyz", NULL, NULL);

	RUN_TESTCASE("add a folder(smallfile) to node:xyz", do_AddFileToPckFile, WINPCK_OK, TEST_SRC_DIR"smallfile", TEST_DST_DIR_PCK, L"xyz");

	printf("\nlist xyz\\\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"xyz", NULL, NULL);

	printf("\n==================================================\n");

	RUN_TESTCASE("add a big file(0.bmp) to node:xyz", do_AddFileToPckFile, WINPCK_OK, TEST_SRC_DIR"0.bmp", TEST_DST_DIR_PCK, L"xyz");

	printf("\nlist xyz\\\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"xyz", NULL, NULL);

#endif

#if RUN_RENAME


	printf("\n==================================================\n");

	printf("\nlist xyz\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"xyz", NULL, NULL);

	RUN_TESTCASE("rename xyz\\smallfile -> xyz\\smallfileabc", do_RenameEntry, WINPCK_OK, TEST_DST_DIR_PCK, L"xyz\\smallfile", L"smallfileabc");

	printf("\nlist xyz\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"xyz", NULL, NULL);



	printf("\n==================================================\n");

	printf("\nlist root\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"", NULL, NULL);

	RUN_TESTCASE("rename xyz -> xyzabc", do_RenameEntry, WINPCK_OK, TEST_DST_DIR_PCK, L"xyz", L"xyzabc");

	printf("\nlist root\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"", NULL, NULL);
	
#endif
#if RUN_DELETE

	printf("\n==================================================\n");

	printf("\nlist root\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"", NULL, NULL);

	RUN_TESTCASE("delete a file 0.bmp from ROOT", do_DeleteFromPck, WINPCK_OK, TEST_DST_DIR_PCK, 1, L"0.bmp");

	printf("\nlist root\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"", NULL, NULL);

	printf("\n==================================================\n");

	printf("\nlist xyzabc\\\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"xyzabc", NULL, NULL);

	RUN_TESTCASE("delete a file xyzabc\\0.bmp", do_DeleteFromPck, WINPCK_OK, TEST_DST_DIR_PCK, 1, L"xyzabc\\0.bmp");

	printf("\nlist xyzabc\\\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"xyzabc", NULL, NULL);

	printf("\n==================================================\n");

	RUN_TESTCASE("delete a folder xyzabc\\smallfileabc", do_DeleteFromPck, WINPCK_OK, TEST_DST_DIR_PCK, 1, L"xyzabc\\smallfileabc");

	printf("\nlist xyzabc\\\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"xyzabc", NULL, NULL);

	printf("\n==================================================\n");

	RUN_TESTCASE("delete more files(smallfile, xyzabc\\(1).txt, xyzabc\\(2).txt, xyzabc\\(3).txt)", do_DeleteFromPck, WINPCK_OK, TEST_DST_DIR_PCK, 4, L"smallfile", L"xyzabc\\(1).txt", L"xyzabc\\(2).txt", L"xyzabc\\(3).txt");

	printf("\nlist root\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"", NULL, NULL);

	printf("\nlist xyzabc\\\n");
	do_listPathInPck(TEST_DST_DIR_PCK, L"xyzabc", NULL, NULL);

#endif

#if RUN_REBUILD

	printf("\n==================================================\n");

	RUN_TESTCASE("rebuild test.pck without recompress", do_RebuildPckFileWithScript, WINPCK_OK, TEST_SRC_DIR_EXTRACT_ALL_SRC, NULL, TEST_DST_DIR_REBUILD_NOC, FALSE);

	printf("\n==================================================\n");

	RUN_TESTCASE("rebuild test.pck with recompress 7", do_RebuildPckFileWithScript, WINPCK_OK, TEST_SRC_DIR_EXTRACT_ALL_SRC, NULL, TEST_DST_DIR_REBUILD_WC7, TRUE, 7);

#endif
#if RUN_REBUILD_FILTER
	printf("\n==================================================\n");

	RUN_TESTCASE("rebuild gfx.pck with recompress 12 and filter", do_RebuildPckFileWithScript, WINPCK_OK, TEST_SRC_DIR_REBUILD_FILTER_SRC, TEST_SRC_DIR_REBUILD_FILTER, TEST_DST_DIR_REBUILD_FGFX, TRUE, 12);

#endif

	g_exit_show = TRUE;

	if(t_process.joinable())
		t_process.join();

	printf("exit..\n");
	getchar();
	return 0;
}

// Run the program: Ctrl + F5 or Debug > "Start execution (without debugging)" menu
// Debugger: F5 or Debug > Start Debugging menu

// Getting Started Tips:
// 1. Use the Solution Explorer window to add/manage files
// 2. Connect to source control using the Team Explorer window
// 3. Use the output window to view build output and other messages
// 4. Use the error list window to view errors
// 5. Go to Project > Add New Item to create a new code file, or Project > Add Existing Item to add an existing code file to the project
// 6. In the future, to open this project again, go to File > Open > Project and select the .sln file
