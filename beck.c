#include "beck.h"
#include "quiz.h"

int main(int argc, char* argv[]) {
    FILE* file;
    int total;
    QUIZ quiz;

#ifdef _WIN32
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 0x1f);
#endif

    if (!should_begin()) {
        return 0;
    }

    file = open_beck(argv[0]);
    if (file == NULL) {
        puts("unable to open beck.txt");
        return 1;
    }

    make_quiz(&quiz, file, PROMPT_DELIM);

    fclose(file);

    total = do_quiz(&quiz);

    printf("Total: %i\n", total);
    get_suggestion(total);

    free_quiz(&quiz);

    return 0;
}

FILE* open_beck(const char* arg0) {
    char path[MAX_PATH];
    GetQuizPath(arg0, path, MAX_PATH);
    return fopen(path, "r");
}

bool should_begin(void)
{
    char yn;
    puts("Beck's Depression Inventory\n");
    puts("This quiz can help determine your level of depression.");
    puts("There are 20 questions. Enter the number corresponding to how");
    puts("you felt over the last two weeks. If you feel in between two answers,");
    puts("enter the larger number.");
    do {
        fputs("Continue? [Y/N] ", stdout);
        scanf("%c%*c", &yn);
        yn = toupper(yn);
        fputs("\n", stdout);
        if (yn == 'N') {
            return false;
        }
    } while (yn != 'Y');
    return true;
}

void get_suggestion(int total)
{
    if (total > 40) {
        puts("Your depression seems to be extreme. You should seek medical help immediately.");
    }
    else if (total > 30) {
        puts("You may have severe depression.");
    }
    else if (total > 20) {
        puts("You suffer from moderate depression.");
    }
    else if (total > 16) {
        puts("Your mood disturbance is borderline clinical depression.");
    }
    else if (total > 10) {
        puts("You may suffer from mild mood disturbances.");
    }
    else if (total < 1) {
        puts("You do not seem to have any mood disturbance.");
    }
    else {
        puts("These ups and downs are considered normal.");
    }
}

void* erealloc(void* ptr, size_t count)
{
    void* newptr = realloc(ptr, count);
    if (newptr == NULL) {
        puts("Out of memory");
        exit(1);
    }
    return newptr;
}

void* emalloc(size_t count)
{
    void* ptr = malloc(count);
    if (ptr == NULL) {
        puts("Out of memory");
        exit(1);
    }
    return ptr;
}

size_t GetQuizPath(const char* arg0, char* buff, size_t buff_size)
{
	char* temp_path = emalloc(buff_size);

#ifdef _WIN32
	/* if windows, define things in terms of DWORD */
	DWORD size;
	DWORD index;
	size = GetModuleFileNameA(NULL, temp_path, buff_size);
#elif defined __linux__
	ssize_t size;
	ssize_t index;
	size = readlink("/proc/self/exe", temp_path, buff_size); /* hopefully it's a linux version that supports this */
#else
	size_t size;
	size_t index;
	strcpy(temp_path, arg0); /* hope that the path is in arg0 */
	size = strlen(temp_path);
#endif


	for(index = size - 1; index > 0; --index) { /* go backwards until we reach a directory separator */
		if (temp_path[index] == SEPARATOR[0]) {
	        	temp_path[index + 1] = '\0'; /* set where we want the string to end (one after the separator) */
			strcpy(buff, temp_path); /* copy the temp path to the buff */
			strcat(buff, "beck.txt"); /* append the file we're looking for */
			free(temp_path);
			return strlen(buff);
		}
	}

	strcpy(buff, ".");
	strcat(buff, SEPARATOR);
	strcat(buff, "beck.txt"); /* fail safe, hopefully it's in the startup path */

	free(temp_path);
	return strlen(buff);
}