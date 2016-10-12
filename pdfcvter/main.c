#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hpdf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <setjmp.h>
#define MAX_FILE_NAME_LEN 255


jmp_buf env;
void error_handler(HPDF_STATUS   error_no,
                HPDF_STATUS   detail_no,
                void         *user_data)
{
    printf ("ERROR: error_no=%04X, detail_no=%u\r\n", (HPDF_UINT)error_no,
        (HPDF_UINT)detail_no);
    longjmp(env, 1);
}

HPDF_Page pdfAddPage(HPDF_Doc pdf)
{
	HPDF_Page page;
    HPDF_Destination dst;
    HPDF_Font font;
	font = HPDF_GetFont (pdf, "Helvetica", NULL);
	/* add a new page object. */
	page = HPDF_AddPage (pdf);
	HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
	dst = HPDF_Page_CreateDestination (page);
	HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
	HPDF_SetOpenAction(pdf, dst);		
	//HPDF_Page_SetWordSpace (page, 2.5);
	HPDF_Page_SetFontAndSize (page, font, 12);
	return page;
}

int main(int argc, char *argv[])
{
	FILE *file_handle;	
	char StrLine[1024];
	char *strLine = StrLine;
	int i,count,strLineSize;
	//float tw;//, actual_width;
	//HPDF_INT char_width;
	HPDF_Doc pdf;
	HPDF_Page page;
	for(i=0;i<1024;i++)
		StrLine[i] = 0;
	if (argc != 3)
	{
		printf("Syntax Error: pdfcvter <input> <output>\n");
		return 1;
	}
	else
	{
		if((strlen(argv[1]) > MAX_FILE_NAME_LEN) || (strlen(argv[2]) > MAX_FILE_NAME_LEN))
		{
			printf("Syntax Error: Input or output file name too long!\n");
			return 1;
		}

		file_handle=fopen(argv[1],"r");
		if(file_handle == NULL)
		{
			printf("Open input file failed!\r\n");
			return -1;
		}

		pdf = HPDF_New(error_handler, NULL);
		//printf("To here!\r\n");
    	if (!pdf) {
        	printf ("ERROR: cannot create pdf object.\n");
        	return 1;
    	}
		if (setjmp(env)) {
        	HPDF_Free (pdf);
        	return 1;
    	}
		HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);
		page = pdfAddPage(pdf);
		
		
		count = 30;
		strLineSize = 0;
		//char_width = HPDF_Font_GetUnicodeWidth (font, UNICODE);
		//actual_width = char_width * FONT_SIZE / 1000;
		//printf("Actual character width:%d\r\n",actual_width);
		while (!feof(file_handle)) 
		{
			if(strLineSize > 0)
				memset(strLine, 0 ,strLineSize);
			
			if((fgets(strLine,1024,file_handle)) == NULL);
			strcat(strLine,"\x00");
			strLineSize = strlen(strLine);
			//Auto add page
			if((count + 30) > HPDF_Page_GetHeight(page))
			{
				page = pdfAddPage(pdf);
				count = 30;
			}
			HPDF_Page_BeginText (page);
			HPDF_Page_TextOut (page, 20, HPDF_Page_GetHeight(page) - count, strLine);
			count = count + 15;
			HPDF_Page_MoveTextPos (page, 20, HPDF_Page_GetHeight(page) - count);
			HPDF_Page_EndText (page);
		}

		
		/* save the document to a file */
    	HPDF_SaveToFile(pdf, argv[2]);   
    	/* clean up */
    	HPDF_Free (pdf);
		fclose(file_handle);
	}
	return 0;
}


