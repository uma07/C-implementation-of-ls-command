/***

        Implement the ls command with the following options:

        -a            do not ignore entries starting with .

        -A            do not list implied . and ..

        -c            with -lt: sort by, and show, ctime (time of last modification of
                      filename status information) with -l: show ctime and      sort  by  name
                      otherwise: sort by ctime

        -d            list directory entries instead of contents, and do not  derefer-
                      ence symbolic links

        -i            with -l, print the ind_inform number of each filename

        -l            use a long listing format

        -n            like -l, but list numeric user and group IDs

        -R            list subdirectories recursively

        -s            with -l, print size of each filename, in blocks

        -t            sort by modification time


                        *******************************************************************************************
                                                Submitted by UMA SANKAR(Regd. No. 16008)
                        *******************************************************************************************


***/




#include "apue.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>





typedef int Oper(char*, const struct stat *, int);
bool bit_a, bit_A, bit_c, bit_d, bit_i, bit_l, bit_n, bit_R, bit_s, bit_t;
char temp;






void parse_args(int argc, char **argv)
{

        char c;
        while( (c=getopt(argc,argv,"aAcdilnRst")) != -1 )
        {
	       switch(c)
               {
                        case 'a':
                        	        bit_a = true;
                                	break;

                        case 'A':
	                                bit_A = true;
        	                        break;

                        case 'c':
                	                bit_c = true;

                        	        if(!bit_t)
                                	        temp = 'c';

	                                else
	                                        temp = 't';
        	                        break;

                        case 'd':
	                                bit_d = true;
	                                break;

                        case 'i':
	                                bit_i = true;
	                                break;

                        case 'l':
	                                bit_l = true;
		                        break;

                        case 'n':
	                                bit_n = true;
	                                break;

                        case 'R':
	                                bit_R = true;
	                                break;

                        case 's':
	                                bit_s = true;
	                                break;

                        case 't':
	                                bit_t = true;

	                                if(!bit_c)
	                                        temp = 't';

	                                else
	                                        temp = 'c';

	                                break;

                         default:
                                	printf("Invalid option !!!\n");
                }
        }

}







void user_name(unsigned int id)
{

	struct passwd *pd;

	pd = getpwuid(id);
	printf(" %10s", pd->pw_name);

}







void group_name(unsigned int id)
{

	struct group *pd;

	pd = getgrgid(id);
	printf(" %10s", pd->gr_name);

}







int print_data(char* name, const struct stat *statbuf, int count)
{
	int i;
	char time[100];
	struct passwd *pwd;
	struct group *grp;

	if(!bit_l)
		for(i=0; i<count; i++)
			printf("\t");
	if(bit_l)
	{	//printf("hai");

		printf( (S_ISDIR(statbuf->st_mode)) ? "d" : "-" );
		printf( (statbuf->st_mode & S_IRUSR) ? "r" : "-" );
		printf( (statbuf->st_mode & S_IWUSR) ? "w" : "-" );
		printf( (statbuf->st_mode & S_IXUSR) ? "x" : "-" );
		printf( (statbuf->st_mode & S_IRGRP) ? "r" : "-" );
		printf( (statbuf->st_mode & S_IWGRP) ? "w" : "-" );
		printf( (statbuf->st_mode & S_IXGRP) ? "x" : "-" );
		printf( (statbuf->st_mode & S_IROTH) ? "r" : "-" );
		printf( (statbuf->st_mode & S_IWOTH) ? "w" : "-" );
		printf( (statbuf->st_mode & S_IXOTH) ? "x" : "-" );

		if(bit_i)
			printf("%8lu",statbuf->st_ino);

		printf("%3lu",statbuf->st_nlink);

		if(bit_s)
			printf("%4lu",statbuf->st_blocks);

		if(bit_n)
		{
			printf(" %u ",statbuf->st_uid);
			printf(" %u ",statbuf->st_gid);
		}

		else
		{
			pwd = getpwuid(statbuf->st_uid);
			grp = getgrgid(statbuf->st_gid);
			printf(" %10s", pwd->pw_name);
			printf(" %10s ",grp->gr_name);
		}
		
		printf("%7ld",statbuf->st_size);
		
		if(!bit_c)
			strftime(time,100,"%b %d %H:%M",localtime(&statbuf->st_mtime));
		
		else
			strftime(time,100,"%b %d %H:%M",localtime(&statbuf->st_ctime));
		
		printf(" %s ",time);
	}
	
	printf("%s\n",name);

}







int traversing(char *par, Oper *op)
{
	static int count = -1;
	count++;
	struct stat statbuf;
	struct stat** buf;
	DIR *dp;
	struct dirent** entry;
	struct dirent* tempEntry;
	int i,count1 = 0;
	char *filename;

	if( (filename = (char*)malloc(sizeof(char) * (strlen(par) + pathconf(par,_PC_PATH_MAX) + 1))) == NULL )
	{
		err_sys("\nNo memory\n");
		goto end;
	}

	if( lstat(par,&statbuf)<0 )
	{
		printf("\nNot statable\n");
		goto end;
	}

	if(S_ISDIR(statbuf.st_mode) == 0)
	{	
		op(par, &statbuf, 0);
		goto end;
	}

	if(bit_d)
	{
		op(par, &statbuf, 0);
		goto end;
	}

	count1 = scandir(par, &entry, NULL, NULL);
	buf = (struct stat**)calloc(count1, sizeof(struct statbuf*));
	
	for(i=0; i<count1; i++)
	{
		strcpy(filename,par);
		if(filename[ strlen(filename)-1 ] != '/')
			strcat(filename,"/");
		strcat(filename,entry[i]->d_name);
		buf[i] = (struct stat*)calloc(1,sizeof(struct stat));
		lstat(filename,buf[i]);
	}

	if(bit_t || bit_c)
		sort(entry,buf,count1);

	if(bit_l && bit_R)
		printf("\n%s\n",par);
	
	for(i=0; i<count1; i++)
	{
		bool a, b;

		a = (entry[i]->d_name[0]!='.' || bit_a || bit_A);
		b = ((strcmp(entry[i]->d_name,".") && strcmp(entry[i]->d_name,"..")) || !bit_A);

		if(a && b)
			op(entry[i]->d_name, buf[i], count);

		if(bit_l)
			continue;

		if(bit_R)
			if(S_ISDIR(buf[i]->st_mode))
				if( strcmp(entry[i]->d_name,".") && strcmp(entry[i]->d_name,"..") )
				{
					strcpy(filename,par);

					if(filename[ strlen(filename)-1 ] != '/')
						strcat(filename,"/");

					strcat(filename,entry[i]->d_name);
					traversing(filename,op);
				}
	}

	if(bit_R && bit_l)
		for(i=0; i<count1; i++)
		{
			if(S_ISDIR(buf[i]->st_mode))
				if( strcmp(entry[i]->d_name,".") && strcmp(entry[i]->d_name,"..") )
				{
					strcpy(filename,par);

					if(filename[ strlen(filename)-1 ] != '/')
						strcat(filename,"/");

					strcat(filename,entry[i]->d_name);
					traversing(filename,op);
				}
		}

	end:
		count--;
		free(filename);

	return 0;

}






void sort(struct dirent **dir, struct stat **buf, int length)
{

	int i,j;
	struct dirent *t_dir;
	struct stat *tempStat;
	char st = 0;

	if(!bit_l)
	{
		if(bit_c)
		{
			if(bit_t)
				st = temp;
			else
				st = 'c';
		}

		else
			st = 't';
	}

	else
	{
		if(bit_c && bit_t)
			st = 'c';

		else if(bit_c)
			st = 'n';
		
		else
			st = 't';	
	}

	if(st == 't')
	{
		for(i=0; i<length; i++)
			for(j=0; j<length-i-1; j++)
				if(buf[j]->st_mtime > buf[j+1]->st_mtime)
				{	
					t_dir = dir[j];
					dir[j] = dir[j+1];
					dir[j+1] = t_dir;
					
					tempStat = buf[j];
					buf[j] = buf[j+1];
					buf[j+1] = tempStat;
				}
	}

	else if(st == 'c')
	{
		for(i=0; i<length; i++)
			for(j=0; j<length-i-1; j++)
				if(buf[j]->st_ctime > buf[j+1]->st_ctime)
				{	
					t_dir = dir[j];
					dir[j] = dir[j+1];
					dir[j+1] = t_dir;
					
					tempStat = buf[j];
					buf[j] = buf[j+1];
					buf[j+1] = tempStat;
				}
	}

	else
	{
		for(i=0; i<length; i++)
			for(j=0; j<length-i-1; j++)
				if(strcmp(dir[j]->d_name, dir[j+1]->d_name) > 0)
				{	
					t_dir = dir[j];
					dir[j] = dir[j+1];
					dir[j+1] = t_dir;
					
					tempStat = buf[j];
					buf[j] = buf[j+1];
					buf[j+1] = tempStat;
				}
	}

}









int main(int argc, char **argv)
{

	parse_args(argc, argv);

	if(optind >= argc )
		traversing(".", print_data);
	else
		traversing(argv[optind], print_data);
	
	return 0;
}
