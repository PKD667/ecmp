#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "malloc.h"

#include "spm/libspm.h"
#include "spm/utils.h"

#include "hashtable.h"


/*
If you are currently trying to understand what i did below , 
you need o kno that im very proud of it
*/
#define parse_cmd_pattern(dest) \
if (pkg->info.dest != NULL) { \
	pkg->info.dest = realloc(pkg->info.dest, strlen(pkg->info.dest) + strlen(line) + 1);\
	strcat(pkg->info.dest,line);\
} else { \
	pkg->info.dest = calloc(strlen(line) + 1, sizeof(char));\
	strcpy(pkg->info.dest,line);\
} \
return 0;\



int parseinfo(char*line, struct package* pkg);

int parsemake(char* line, struct package* pkg);
int parseinstall(char* line, struct package* pkg);
int parsedownload(char* line, struct package* pkg);
int parseprepare(char* line, struct package* pkg);
int parsespecial(char* line, struct package* pkg);

hashtable* hm;
hashtable* infohm;


int open(char* path,struct package* pkg)
{
	if (access(path,F_OK) != 0) {
		msg(ERROR, "File not found");
        return 1;
	}
	FILE* fp = fopen(path,"r");
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	char* section = NULL;

	void* pairs[][2] = {
		{"info",parseinfo},

		{"make",parsemake},
		{"install",parseinstall},
		{"download",parsedownload},
		{"prepare",parseprepare},
		{"special",parsespecial},
	};
	void* infodict[][2] = {
		{"name",&pkg->name},
		{"version",&pkg->version},
		{"type",&pkg->type},
		{"url",&pkg->url},
		{"license",&pkg->license}	
	};
	hm = hm_init(pairs,sizeof(pairs)/sizeof(pairs[0]));
	infohm = hm_init(infodict,sizeof(infodict)/sizeof(infodict[0]));

	printf("hm : \n");
	hm_visualize(hm);
	printf("infohm : \n");
	hm_visualize(infohm);

	while ((read = getline(&line,&len,fp)) != -1)
	{
		msg(DEBUG,"Read line: %s\n",line);

		if (line[0] == '#') continue;
		
		if (line[0] == '[') {
			if (section != NULL) free(section);
			section = strdup(strtok(line,"[]"));
			printf("Section: %s\n",section);
			continue;
		}
		if (section == NULL) continue;

		int (*parse)(char*,struct package*) = hm_get(hm, section);

		if (parse == NULL) {
			msg(WARNING, "Unknown section %s\n", section);
			continue;
		}
		//printf("parsing %s unsigned int %p\n",section,parse);
		parse(line,pkg);

		line = NULL;

	}


	if (line) free(line);
	if (section) free(section);

	hm_destroy(hm);
	fclose(fp);
	return 0;
}




int parsespecial(char* line, struct package* pkg)
{
	parse_cmd_pattern(special);
}
int parsemake(char* line, struct package* pkg)
{
	parse_cmd_pattern(make);
}
int parseinstall(char* line, struct package* pkg)
{
	parse_cmd_pattern(install);
}
int parsedownload(char* line, struct package* pkg)
{
	parse_cmd_pattern(download);
}
int parseprepare(char* line, struct package* pkg)
{
	parse_cmd_pattern(prepare);
}



int parseinfo(char*line, struct package* pkg)
{
	printf("Parsing info\n");

	// remove newline
	line[strlen(line)-1] = '\0';
	// remove all spaces
	for (int i = 0; i < strlen(line); i++) {
		if (line[i] == ' ') {
			popcharn(line,strlen(line),i);
			break;
		}
	}

	char* key = strtok(line,"=");
	char* value = strtok(NULL,"=");
	if (key == NULL || value == NULL) return -1;

	char** dest = hm_get(infohm,key);
	if (dest == NULL) {
		msg(WARNING, "Unknown key '%s'\n", key);
		return -1;
	}
	printf("Key: %s Value: %s\n",key,value);
	*dest = strdup(value);
	return 0;
}

