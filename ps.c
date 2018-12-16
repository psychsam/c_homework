/*
 *
 * Filename: ps.c
 * Author: imbos4
 * Date: 16.12.2018
 * Description: Software is reading all processes in proc  directory and writing out its pid, name, rss.
 *              But only those which the user is allowed to read.
*/
//----------Inlcudes
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
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
            //Starts reading dirs from here
            //all before are getting ignored
            if(strcmp(de ->d_name,"1") == 0) {
                //set flag on true
                proclist_started = 1;
                get_file(de -> d_name);
            }
        }
    }
    closedir(dir);
}
//Reads trough status file from given proc, extracts name pid and rss
void get_file(char* d_name){
    char pid[256];//process id
    char rss[256];//this can hold 115*10^75 kbytes
    char name[256]; //16 byte long name allowed max, but just to be sure
    char name_temp[256];//temp var for names in file
    char uid[256]; //holds user id
    char print_proc = 0; //flag for printing process
    char filename[sizeof("/proc/") + sizeof(d_name) + sizeof("/status")]; //filename eg. /proc/$pid/status
    FILE *file_pointer;
    //adding single strings together
    strcpy(filename,"/proc/");
    strcat(filename,d_name);
    strcat(filename, "/status");
    //open file
    file_pointer = fopen(filename,"r");
    //while true
    for(;;){
        char line[256];
        if(1 != fscanf(file_pointer, "%255s",line)) break; //while not done, read line
        if(strcmp(S_NAME,line) == 0) {  //this loops trough the file to get full name, if there is a space between
            fscanf(file_pointer, "%255s",name);//read name
            fscanf(file_pointer, "%255s",name_temp);//read next name
            while(strcmp(name_temp,S_AFTERNAME)!= 0){//if next name not next line add next name to name
                strcat(name, " ");
                strcat(name,name_temp);
                fscanf(file_pointer, "%255s",name_temp);
            }
        }else if(strcmp(S_PID,line) == 0){//if name pid then save pid
            fscanf(file_pointer, "%255s",pid);
        }else if(strcmp(S_RSS,line) == 0){
            fscanf(file_pointer, "%255s",rss);//same as above
        }else if(strcmp(S_UID,line) == 0) {
            fscanf(file_pointer,"%255s",uid);//owner
            fscanf(file_pointer,"%255s",uid);//user
            int uidi = atoi(uid); //get uid
            if(uidi == getuid()) print_proc = 1;
        }    
    }
    
      if(print_proc) printf("%s\t%s\t%s\n",pid,name,rss);
}
