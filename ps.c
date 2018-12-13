//----------Inlcudes
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
//----------Defines
#define S_PID "Pid:"
#define S_NAME "Name:"
#define S_RSS "VmRSS:"
#define S_UID "Uid:"
#define S_USERID "1000"
#define S_AFTERNAME "Umask:"
//----------Prototypes
void read_procs();
void get_file(char* dirname);
//----------Main
int main () {;
    read_procs();
    return 0;
}
//----------Functions
//Reads trough every proc
void read_procs(){
    char proclist_started = 0;
    DIR* dir ;
    struct dirent* de ;
    dir = opendir ("/proc") ;
    while(NULL != (de=readdir(dir))){
        if(proclist_started){
            get_file(de -> d_name);
        }else{
            if(strcmp(de ->d_name,"1") == 0) {
                proclist_started = 1;
                get_file(de -> d_name);
            }
        }
    }
    closedir(dir);
}
//Reads trough status file from given proc, extracts name pid and rss
void get_file(char* d_name){
    char pid[256];
    char rss[256];//this can hold 115*10^75 kbytes
    char name[256]; //16 byte long name allowed max, but just to be sure
    char name_temp[256];
    char uid[256];
    char print_proc = 0;
    char filename[sizeof("/proc/") + sizeof(d_name) + sizeof("/status")];
    FILE *file_pointer;

    strcpy(filename,"/proc/");
    strcat(filename,d_name);
    strcat(filename, "/status");
    file_pointer = fopen(filename,"r");
    
    for(;;){
        char line[256];
        if(1 != fscanf(file_pointer, "%255s",line)) break;
        if(strcmp(S_NAME,line) == 0) {
            fscanf(file_pointer, "%255s",name);
            fscanf(file_pointer, "%255s",name_temp);
            while(strcmp(name_temp,S_AFTERNAME)!= 0){
                strcat(name, " ");
                strcat(name,name_temp);
                fscanf(file_pointer, "%255s",name_temp);
            }
        }else if(strcmp(S_PID,line) == 0){
            fscanf(file_pointer, "%255s",pid);
        }else if(strcmp(S_RSS,line) == 0){
            fscanf(file_pointer, "%255s",rss);
        }else if(strcmp(S_UID,line) == 0) {
            fscanf(file_pointer,"%255s",uid);//owner
            fscanf(file_pointer,"%255s",uid);//user
            if(strcmp(uid,S_USERID) == 0) print_proc = 1;
        }    
    }
    
      if(print_proc) printf("%s\t%s\t%s\n",pid,name,rss);
}