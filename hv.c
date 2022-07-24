/*
** Hex Viewer for AmigaOS 4.1
** Version 1.0.0
*/

///
/* INCLUDES */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dos/dos.h>
#include <dos/rdargs.h>

///
/* DEFINES */
#define ARGTEMPLATE "FILE/A"
#define ARG_FILE 0
#define MAX_ARGS 1
#define INBUF_SIZE	(64*1024)

///
/* GLOBALS */
int RetCode = RETURN_OK;
BPTR InFile;
char InFilename[MAX_DOS_PATH];

///
/* PROTOTYPES */
#include <proto/dos.h>

BOOL init(int, char **);
void cleanup(void);
void dumpFile(BPTR);

///
/* main */
int main(int argc, char *argv[])
{
    atexit(cleanup);

    if(init(argc, argv))
    {
        InFile = IDOS->FOpen(InFilename, MODE_OLDFILE, INBUF_SIZE);
        if(InFile != ZERO)
        {
			/* Disable semaphore locks since we won't change between buffered
			   and unbuffered reads. Note that if the file will be accessed from
			   multiple processes or also uses unbuffered access, remove this call. */
			IDOS->SetFileHandleAttrTags(InFile, FH_Locking, DOSFALSE, TAG_END);
			
            // We can only accept files of less than 64KB.
            struct ExamineData *ed;
            ed = IDOS->ExamineObjectTags(EX_FileHandleInput, InFile, TAG_END);
            if(ed)
            {
                if(EXD_IS_FILE(ed))
                {
	                if(ed->FileSize & 0xFFFFFFFFFFFF0000)
	                {
	                    puts("ERROR: Sorry. Only files less than 64KB are accepted.");
	                    RetCode = 5;
	                }
	                else
	                {
			            dumpFile(InFile);
            		}
            	}
            	else
            	{
            	    printf("ERROR: Only files can be examined.\n");
            	    RetCode = 5;
            	}
                
                IDOS->FreeDosObject(DOS_EXAMINEDATA, ed);
            }
            else
            {
                IDOS->PrintFault(IDOS->IoErr(), "ERROR:");
                RetCode = 10;
            }
        }
        else
        {
            printf("ERROR: Could not open %s for input!\n", InFilename);
        }
    }
    return RetCode;
}

///
/* init */
BOOL init(int argc, char *argv[])
{
    struct RDArgs *rdArgs;
    LONG args[MAX_ARGS];
    BOOL ok = TRUE;

    // This program is to be run from the CLI only.
    if(argc == 0)
    {
        ok = FALSE;
    }
    else
    {
        // Get the arguments from the command line.
        rdArgs = IDOS->ReadArgs(ARGTEMPLATE, args, NULL);
        if(rdArgs != NULL)
        {
            char *fileName = (char *)args[ARG_FILE];
            if(strlen(fileName) < 256)
            {
                strncpy(InFilename, fileName, 256);
            }
            else
            {
                puts("ERROR: Filename is too long\n");
                ok = FALSE;
                RetCode = 10;
            }
            IDOS->FreeArgs(rdArgs);
        }
        else
        {
            printf("USAGE: %s %s\n", argv[0], ARGTEMPLATE);
            ok = FALSE;
        }
    }
    return ok;
}

///
/* cleanup */
void cleanup(void)
{
    if(InFile != ZERO) IDOS->FClose(InFile);
}

///
/* dumpFile */
void dumpFile(BPTR fh)
{
    int i;
    int offset = 0;
    BOOL done = FALSE;
    char hexLine[80];
    char hex8Str[3];
    UBYTE asciiChar;

    while(!done)
    {
        for(i=0; i<16; ++i)
        {
            LONG ch = IDOS->FGetC(fh);

            if(ch == -1)
            {
                LONG err = IDOS->IoErr();
                
                if(err != 0)
                    IDOS->PrintFault(err, "ERROR: ");

                done = TRUE;
                break;
            }

            if(i == 0)
            {
                // First char of the line. Init and place address.
                snprintf(hexLine, sizeof(hexLine), "%04X:                                                 |                 ", offset);
            }

            // Write out the hex value.
            asciiChar = (UBYTE)ch;
            snprintf(hex8Str, sizeof(hex8Str), "%02X", (UBYTE)ch);
            strncpy(&hexLine[6+(3*i)], hex8Str, 2);

            // Write out the ASCII value.
            if(asciiChar < ' ' || asciiChar > '~')
                asciiChar = '.';
            
            memcpy(&hexLine[56+i], &asciiChar, 1);
        }

        puts(hexLine);
        offset += 16;
    }
}
