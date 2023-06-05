#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <libgen.h>

struct arguments {
    int functionCode;
    char function[3];
    char input[384];
    char container[6];
    char output[384];
};

enum functions { dv = 0, da, cv, ca, rv };

int getFunctionCode(char *function);
void progExit(int errCode, char *errMsg);
void terminatePath(char *path);
void replaceSlash(char *str);
void downloadVideo(struct arguments args, char *command);
void downloadAudio(struct arguments args, char *command);
void convertVideo(struct arguments args, char *command);
void convertAudio(struct arguments args, char *command);
void reencodeVideo(struct arguments args, char *command);
char *get_filename(char *str);
void debug(struct arguments args);

int main(int argc, char **argv) {
    
    // static struct arguments args = {dv, "dv", "https://www.youtube.com/watch?v=dQw4w9WgXcQ", "\0", "%userprofile%\\Documents"};
    static struct arguments args = {cv, "cv", "%userprofile%\\Documents\\out.mp3", "\0", "%userprofile%\\Documents"};
    char command[1024];

    int opt;
    while (((opt = getopt(argc, argv, "i:f:c:o:")) != -1)) {
        switch(opt) {
            case 'i':
                strcpy(args.input, argv[optind-1]); break;

            case 'f':
                strcpy(args.function, argv[optind-1]);
                args.functionCode = getFunctionCode(args.function);
                break;

            case 'c':
                strcpy(args.container, argv[optind-1]); break;

            case 'o':
                strcpy(args.output, argv[optind-1]); break;

            case ':':
                puts("temp"); break;
            
            case '?':
                puts("temp"); break;
        }
    }

    

    // TODOs
    if      (args.function[1] == 'v' && args.container[0] == '\0')
        strcpy(args.container, "mp4");
    else if (args.function[1] == 'a' && args.container[0] == '\0')
        strcpy(args.container, "opus");

    // TODO
    // terminatePath(args.input);
    // terminatePath(args.output);

    // TODO
    if (args.functionCode == cv || args.functionCode == ca || args.functionCode == rv) {
        replaceSlash(args.input);
        // replaceSlash(args.output);
    }
    debug(args);

    switch(args.functionCode) {
        case dv:
            downloadVideo(args, command); break;
        case da:
            downloadAudio(args, command); break;
        case cv:
            convertVideo(args, command); break;
        case ca:
            convertAudio(args, command); break;
        case rv:
            reencodeVideo(args, command); break;
    }

    puts(command);
    // system(command);

    system("pause");
    exit(0);
}

void debug(struct arguments args) {
    replaceSlash(args.input);
    puts(args.input);
    char *filename = get_filename(args.input);
    puts(filename);


}

// TODO
int getFunctionCode(char *function) {

    if      (strcmp("dv", function) == 0) // download video
        return dv;
    else if (strcmp("da", function) == 0) // download audio
        return da;
    else if (strcmp("cv", function) == 0) // convert video
        return cv;
    else if (strcmp("ca", function) == 0) // convert audio
        return ca;
    else if (strcmp("rv", function) == 0) // reencode video, doesn't support custom crf
        return rv;
    else 
        progExit(4, "invalid function");

    return -1;
}

// TODO
void progExit(int errCode, char *errMsg) {
    printf("ERR: %s\n", errMsg);
    system("pause");
    exit(errCode);
}

// TODO
void terminatePath(char *path) { // Removes forward and backwards slashes from the ends of filepaths
    int i = 1;
    while (path[strlen(path)-i] == '\\' || path[strlen(path)-i] == '/')
        i++;
    
    path[strlen(path)-i+1] = '\0';
}

// TODO
void replaceSlash(char *str) { // replaces all forward slashes with cancelled backwards slashes
    int currCell = 0;
    while (str[currCell] != '\0') {
        if (str[currCell] == '/')
            str[currCell] = '\\';

        currCell++;
    }
}

void downloadVideo(struct arguments args, char *command) {
    sprintf(command, "yt-dlp %s --remux-video %s -o \"%s\\%%(title)s.%%(ext)s\"", args.input, args.container, args.output);
}

void downloadAudio(struct arguments args, char *command) {
    sprintf(command, "yt-dlp %s --extract-audio --audio-format %s -o \"%s\\%%(title)s.%%(ext)s\"", args.input, args.container, args.output); 
}

void convertVideo(struct arguments args, char *command) {

    char *filename = get_filename(args.input);
    sprintf(command, "ffmpeg -i \"%s\" -c:v copy -c:a copy \"%s\\%s.%s\"", args.input, args.output, filename, args.container);
}

void convertAudio(struct arguments args, char *command) {

    char *filename = get_filename(args.input);
    sprintf(command, "ffmpeg -i \"%s\" -c:a libopus \"%s\\%s.%s\"", args.input, args.output, filename, args.container);
}

void reencodeVideo(struct arguments args, char *command) {

    char *filename = get_filename(args.input);
    sprintf(command, "ffmpeg -i \"%s\" -c:v libx264 -crf 19 -preset medium -c:a copy \"%s\\%s.%s\"", args.input, args.output, *filename, args.container); 
}

// TODO
char *get_filename(char *str) {

    char *filename = strrchr(str, '\\') + 1;
    char *dot_in_str = strrchr(filename, '.');
    if (dot_in_str != NULL) 
        *dot_in_str = '\0';

    return filename;
}

// TODO
void getFilename(char *str, char *filename) {

    str = basename(str);
    int extBeginPos = 0;
    int curPos = strlen(str)-1;

    do {
        char chr = str[curPos];

        if (chr == '.' && extBeginPos == 0) {
            extBeginPos = curPos;
            break;
        }
        curPos--;

    } while (curPos > 0);

    // if (extBeginPos == 0)
    //     progExit(1, "ERR:");

    if (curPos == 0)
        extBeginPos = strlen(str);

    str[extBeginPos] = '\\';
    strcpy(filename, dirname(str));

    if (filename[0] == '.')
        progExit(1, "no filename"); // no filename exit
}
