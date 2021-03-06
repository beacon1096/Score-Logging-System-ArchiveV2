#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<time.h>
#include<unistd.h>

#define MAXSTU 61
#define MAXCHN 51
#define MAXNNAMELEN 21
#define MAXNNAMENUM 5
#define CUPROLINEMAX 2004
//Class+Student Info
struct examResult {
	int score;
	int ranking;
};
struct hwResult {
	char grade[11];
};
struct studentInfo {
	char CHN[MAXCHN],NICKNAME[MAXNNAMENUM][MAXNNAMELEN];
	short nameNum;
	struct examResult exRes;
	struct hwResult hwRes;
};
struct sortingInfo {
	bool enabled;
	int algorithm;//1=bucket
};
struct classInfo {
	int num,stuNum;
};
struct studentInfo array[MAXSTU];
struct classInfo cuClass;
//Program Properties
struct settingsGroup {
	bool restartRequired;
	short ELAC;
	short lang;
	bool defaultC;
	short defaultCNum;
	bool defaultT;
	short defaultTNum;//0=examResult,1=exercise/homeworkResult
	bool debugMode;
	short cuT;
	struct sortingInfo sort;
};
struct node {
	int num;
	struct node *next;
};
struct bucket {
	int value;
	struct node *head;
};
struct bucket buckets[121];
struct bucket bucketException;
struct settingsGroup settings;
struct statementGroup {
	//short cuType;
	short cuLoggedStuNum;
};
struct statementGroup states;
//List(*.csv)File Lines Processing
char cuProLine[CUPROLINEMAX+1];
char cuProComponent[21];
//Function List + Declaration
void showDebugInfo(char mode[51]);
void init();
void showVersion();
void loadSettings();
void selectClass(char mode[51]);
void selectType();
void changeCurrentSettings();
void confirmation();
void loadStudentInfo();
void logging();
void printCSV();
void prepare();
void goodbye();
void sorting();
void bucketSorting();
void printBucketSorted();
void clearStudentInfo();
//Always a good friend.
void showDebugInfo(char mode[51]) {
	if(settings.debugMode==0)return;
	int phase=0;
	if(mode=="loadSettings")phase=1;
	if(mode=="loadCuClass")phase=2;
	if(mode=="loadStudents")phase=3;
	if(mode=="bucketSorting")phase=4;
	switch(phase) {
		case 1: {
			printf("[DEBUG]========================\n");
			printf("[DEBUG]The appearing of these info means you're in debug mode.\n");
			printf("[DEBUG]settings.debugMode=%d\n",settings.debugMode);
			printf("[DEBUG]settings.lang=%d\n",settings.lang);
			printf("[DEBUG]settings.defaultC=%d\n",settings.defaultC);
			printf("[DEBUG]settings.defaultCNum=%d\n",settings.defaultCNum);
			printf("[DEBUG]settings.defaultT=%d\n",settings.defaultT);
			printf("[DEBUG]settings.defaultTNum=%d\n",settings.defaultTNum);
			printf("[DEBUG]settings.sort.enabled=%d\n",settings.sort.enabled);
			printf("[DEBUG]settings.sort.algorithm=%d\n",settings.sort.algorithm);
			printf("[DEBUG]Phase 1 over.Smile!\n");
			printf("[DEBUG]========================\n");
			break;
		}
		case 2: {
			printf("[DEBUG]========================\n");
			printf("[DEBUG]cuClass.num=%d\n",cuClass.num);
			printf("[DEBUG]settings.cuT=%d\n",settings.cuT);
			printf("[DEBUG]Phase 2 over.Why not encourage yourself?\n");
			printf("[DEBUG]========================\n");
			break;
		}
		case 3: {
			printf("[DEBUG]========================\n");
			printf("[DEBUG]cuClass.stuNum=%d\n",cuClass.stuNum);
			int i,j;
			for(i=1; i<=cuClass.stuNum; i++) {
				printf("[DEBUG]");
				puts(array[i].CHN);
				printf("[DEBUG]");
				for(j=1; j<=array[i].nameNum; j++) {
					printf("%d:%s;",j,array[i].NICKNAME[j-1]);
				}
				printf("\n[DEBUG]score=%d,grade=%s",array[i].exRes.score,array[i].hwRes.grade);
				printf("\n");
			}
			printf("[DEBUG]Phase 3 over.My task is over.Good night.Love you\n");
			printf("[DEBUG]========================\n");
			break;
		}
		case 4: {
			int i;
			struct node *t=NULL;
			for(i=120; i>=0; i--) {
				if(buckets[i].head==NULL)
					printf("[DEBUG][BucketSorting]%d-NULL\n",i);
				else {
					t=buckets[i].head;
					printf("[DEBUG][BucketSorting]%d",i);
					while(1) {
						printf("-%s,%d",array[t->num].CHN,array[t->num].exRes.score);
						if(t->next==NULL)break;
						t=t->next;
					}
					printf("\n");
				}
				if(i==0)break;
			}
			if(bucketException.head!=NULL){
				t=bucketException.head;
				printf("[DEBUG][BucketSorting]Exception",i);
				while(1) {
					printf("-%s,%d",array[t->num].CHN,array[t->num].exRes.score);
					if(t->next==NULL)break;
					t=t->next;
				}
				printf("\n");
			}
			else printf("[DEBUG][BucketSorting]Exception-NULL");
		}
	}
}
//Initalization begins here.
void init() {
	showVersion();//show current running version info and copyrights
	loadSettings();//load running properties
	showDebugInfo("loadCuClass");
	clearStudentInfo();
LoadStu:
	loadStudentInfo();
	if(settings.restartRequired==true) {
		settings.restartRequired=false;
		selectClass("defaultClassNotFound");
		showDebugInfo("loadStudents");
		goto LoadStu;
	}
	showDebugInfo("loadStudents");
	fclose(stdin);
	switch(settings.lang) {
		case 0: {
			freopen("lang/en_us/initalizing.lang","r",stdin);
			break;
		}
		case 1: {
			freopen("lang/zh_cn/initalizing.lang","r",stdin);
			break;
		}
		default: {
			freopen("lang/zh_cn/initalizing.lang","r",stdin);
			break;
		}
	}
	int i;
	for(i=0; i<=cuClass.stuNum; i++) {
		array[i].exRes.score=0;
		array[i].hwRes.grade[0]='*';
	}
	while(1) {
		fgets(cuProLine,CUPROLINEMAX,stdin);
		if(cuProLine[0]=='&')
			break;
		printf("%s",cuProLine);
	}
	fclose(stdin);
	freopen("CON","r",stdin);
	if(settings.debugMode==true)printf("[init]My job was over.Good night\n");
	usleep(333333);
}
//Shows the title&version at the beginning.
void showVersion() {
	fclose(stdin);
	freopen("about.md","r",stdin);
	char ver[101];
	while(1) {
		fgets(ver,CUPROLINEMAX,stdin);
		getchar();//delete the last \n
		if(ver[0]=='&')break;
		fputs(ver,stdout);
	}
	fclose(stdin);
	freopen("CON","r",stdin);
	if(settings.debugMode==true) {
		printf("[ShowVersion]Destiny finished.Have a good day\n");
	}
	usleep(333333);
}
//Settings are loaded here."options.ini"
void loadSettings() {
	settings.lang=1;
	settings.defaultC=false;
	settings.defaultCNum=0;
	settings.defaultT=false;
	settings.defaultTNum=0;
	settings.debugMode=false;
	settings.sort.enabled=false;
	settings.sort.algorithm=0;
	settings.ELAC=0;
	settings.restartRequired=false;
	int value;
	fclose(stdin);
	freopen("options.ini","r",stdin);
	while(1) {
		//scanf("%s",cuProLine);
		fgets(cuProLine,CUPROLINEMAX,stdin);
		if(cuProLine[0]=='&')break;
		if(cuProLine[0]=='#')continue;
		scanf("%d",&value);
		if(strcmp(cuProLine,"LA\n")==0) {//Language
			switch(value) {
				case 0: {
					settings.lang=0;
					break;
				}
				case 1: {
					settings.lang=1;
					break;
				}
				default: {
					settings.lang=1;
					break;
				}
			}
			continue;
		}
		if(strcmp(cuProLine,"UDC\n")==0) {//useDefaultClass
			switch(value) {
				case 0: {
					settings.defaultC=false;
					break;
				}
				case 1: {
					settings.defaultC=true;
					break;
				}
				default: {
					settings.defaultC=false;
					break;
				}
			}
			continue;
		}
		if(strcmp(cuProLine,"UDT\n")==0) {//useDefaultType
			switch(value) {
				case 0: {
					settings.defaultT=false;
					break;
				}
				case 1: {
					settings.defaultT=true;
					break;
				}
				default: {
					settings.defaultT=false;
					break;
				}
			}
			continue;
		}
		if(strcmp(cuProLine,"DM\n")==0) {//DebugMode
			switch(value) {
				case 0: {
					settings.debugMode=false;
					break;
				}
				case 1: {
					settings.debugMode=true;
					break;
				}
				default: {
					settings.debugMode=false;
					break;
				}
			}
			continue;
		}
		if(strcmp(cuProLine,"SRT\n")==0) { //sorting is enabled
			switch(value) {
				case 0: {
					settings.sort.enabled=false;
					break;
				}
				case 1: {
					settings.sort.enabled=true;
					break;
				}
				default: {
					settings.sort.enabled=false;
					break;
				}
			}
			continue;
		}
		if(strcmp(cuProLine,"ELAC\n")==0) {//DebugMode
			settings.ELAC=value;
			continue;
		}
		if(strcmp(cuProLine,"DC\n")==0 && settings.defaultC==true) { //defaultClass
			settings.defaultCNum=value;
			cuClass.num=value;
			continue;
		}
		if(strcmp(cuProLine,"DT\n")==0 && settings.defaultT==true) { //defaultType
			settings.defaultTNum=value;
			settings.cuT=value;
			continue;
		}

		if(strcmp(cuProLine,"SRTA\n")==0 && settings.sort.enabled==true) { //defaultType
			settings.sort.algorithm=value;
			continue;
		}
	}
	fclose(stdin);
	freopen("CON","r",stdin);
	showDebugInfo("loadSettings");
	if(settings.defaultC==0)selectClass("noDefaultClass");
	if(settings.defaultT==0)selectType("noDefaultType");
	if(settings.debugMode==true)printf("[loadSettings]My job was over.Good night\n");
}
//Select options when default is not provided
void selectClass(char mode[51]) {
	int option=0;
	if(mode=="changingSettings")option=0;
	if(mode=="noDefaultClass")option=1;
	if(mode=="defaultClassNotFound")option=2;
	switch(option) {
		case 0:{
			fclose(stdin);
			switch(settings.lang) {
				case 0: {
					freopen("lang/en_us/acquiringClass.lang","r",stdin);
					break;
				}
				case 1: {
					freopen("lang/zh_cn/acquiringClass.lang","r",stdin);
					break;
				}
				default: {
					freopen("lang/zh_cn/acquiringClass.lang","r",stdin);
					break;
				}
			}
			while(1) {
				fgets(cuProLine,CUPROLINEMAX,stdin);
				if(cuProLine[0]=='&')
					break;
				printf("%s",cuProLine);
			}
			break;
		}
		case 1: {
			fclose(stdin);
			switch(settings.lang) {
				case 0: {
					freopen("lang/en_us/acquiringDefaultClass.lang","r",stdin);
					break;
				}
				case 1: {
					freopen("lang/zh_cn/acquiringDefaultClass.lang","r",stdin);
					break;
				}
				default: {
					freopen("lang/zh_cn/acquiringDefaultClass.lang","r",stdin);
					break;
				}
			}
			while(1) {
				fgets(cuProLine,CUPROLINEMAX,stdin);
				if(cuProLine[0]=='&')
					break;
				printf("%s",cuProLine);
			}
			break;
		}
		case 2:
			fclose(stdin);
			switch(settings.lang) {
				case 0: {
					freopen("lang/en_us/defaultClassNotExist.lang","r",stdin);
					break;
				}
				case 1: {
					freopen("lang/zh_cn/defaultClassNotExist.lang","r",stdin);
					break;
				}
				default: {
					freopen("lang/zh_cn/defaultClassNotExist.lang","r",stdin);
					break;
				}
			}
			while(1) {
				fgets(cuProLine,CUPROLINEMAX,stdin);
				if(cuProLine[0]=='&')
					break;
				printf("%s",cuProLine);
			}
			break;
	}
	fclose(stdin);
	freopen("CON","r",stdin);
	scanf("%d",&cuClass.num);
	if(settings.debugMode==true)printf("[selectClass]Selected.Have a good day\n");
}
void selectSorting()
{
	switch(settings.lang) {
		case 0: {
			freopen("lang/en_us/acquiringSorting.lang","r",stdin);
			break;
		}
		case 1: {
			freopen("lang/zh_cn/acquiringSorting.lang","r",stdin);
			break;
		}
		default: {
			freopen("lang/zh_cn/acquiringSorting.lang","r",stdin);
			break;
		}
	}
	while(1) {
		fgets(cuProLine,CUPROLINEMAX,stdin);
		if(cuProLine[0]=='&')
			break;
		printf("%s",cuProLine);
	}
	fclose(stdin);
	freopen("CON","r",stdin);
	char res;
selSortBegin:
	res=getche();
	printf("\n");
	switch(res) {
		case '0': {
			settings.sort.enabled=0;
			break;
		}
		case '1': {
			settings.sort.enabled=1;
			break;
		}
		default: {
			switch(settings.lang) {
				case 0: {
					freopen("lang/en_us/illegalInput.lang","r",stdin);
					break;
				}
				case 1: {
					freopen("lang/zh_cn/illegalInput.lang","r",stdin);
					break;
				}
				default: {
					freopen("lang/zh_cn/illegalInput.lang","r",stdin);
					break;
				}
			}
			while(1) {
				fgets(cuProLine,CUPROLINEMAX,stdin);
				if(cuProLine[0]=='&')
					break;
				printf("%s",cuProLine);
			}
			fclose(stdin);
			freopen("CON","r",stdin);
			goto selSortBegin;
			break;
		}
	}
	if(settings.debugMode==true)printf("[selectSort]Selected.Have a good day\n");
}
void selectType(char mode[51]) {
	int option=0;
	if(mode=="changingSettings")option=0;
	if(mode=="noDefaultType")option=1;
	fclose(stdin);
	switch(option){
		case 0:{
			switch(settings.lang) {
				case 0: {
					freopen("lang/en_us/acquiringType.lang","r",stdin);
					break;
				}
				case 1: {
					freopen("lang/zh_cn/acquiringType.lang","r",stdin);
					break;
				}
				default: {
					freopen("lang/zh_cn/acquiringType.lang","r",stdin);
					break;
				}
			}
			break;
		}
		case 1:{
			switch(settings.lang) {
				case 0: {
					freopen("lang/en_us/acquiringDefaultType.lang","r",stdin);
					break;
				}
				case 1: {
					freopen("lang/zh_cn/acquiringDefaultType.lang","r",stdin);
					break;
				}
				default: {
					freopen("lang/zh_cn/acquiringDefaultType.lang","r",stdin);
					break;
				}
			}
		}
	}
	while(1) {
		fgets(cuProLine,CUPROLINEMAX,stdin);
		if(cuProLine[0]=='&')
			break;
		printf("%s",cuProLine);
	}
	fclose(stdin);
	freopen("CON","r",stdin);
	char res;
selTypeBegin:
	res=getche();
	printf("\n");
	switch(res) {
		case '0': {
			settings.cuT=0;
			break;
		}
		case '1': {
			settings.cuT=1;
			break;
		}
		default: {
			switch(settings.lang) {
				case 0: {
					freopen("lang/en_us/illegalInput.lang","r",stdin);
					break;
				}
				case 1: {
					freopen("lang/zh_cn/illegalInput.lang","r",stdin);
					break;
				}
				default: {
					freopen("lang/zh_cn/illegalInput.lang","r",stdin);
					break;
				}
			}
			while(1) {
				fgets(cuProLine,CUPROLINEMAX,stdin);
				if(cuProLine[0]=='&')
					break;
				printf("%s",cuProLine);
			}
			fclose(stdin);
			freopen("CON","r",stdin);
			goto selTypeBegin;
			break;
		}
	}
	if(settings.debugMode==true)printf("[selectType]Selected.Have a good day\n");
}
//Stand in lines.We're counting the students.
void loadStudentInfo() {
	fclose(stdin);
	freopen("list.csv","r",stdin);
	int scClass=999,cuStuNum;
	int i,j,k;
	int nnamenum;
	while(scClass!=cuClass.num) {
		if(scanf("C%d,%d",&scClass,&cuStuNum)==EOF) {
			settings.restartRequired=true;
			return;
		}
		scanf("%s",cuProLine);//to clear the "," at the end of the lines
		if(scClass==cuClass.num)break;
		else
			for(i=1; i<=cuStuNum+1; i++) {
				fgets(cuProLine,CUPROLINEMAX,stdin);
				if(cuProLine[0]=='#')i--;
			}
	}
	cuClass.stuNum=cuStuNum;
	for(i=1; i<=cuStuNum; i++) {
		scanf("%s",cuProLine);
		for(j=0; j<=MAXCHN; j++) {
			if(cuProLine[j]==',')break;
			array[i].CHN[j]=cuProLine[j];
		}
		j++;
		k=j;
		array[i].nameNum=1;
		nnamenum=0;
		if(settings.debugMode==1)printf("[DEBUG]%s:",array[i].CHN);
		for(j=0; j<=MAXNNAMELEN*MAXNNAMENUM; j++) {
			if(cuProLine[k]=='\0') {
				if(settings.debugMode==1)printf("\n[DEBUG]brokebyEnter\n");
				break;
			}
			if(nnamenum>MAXNNAMENUM-1) {
				if(settings.debugMode==1)printf("\n[DEBUG]brokebyMAXNNN\n");
				break;
			}
			if(cuProLine[k]==',') {
				nnamenum++;
				if(settings.debugMode==1)printf("\n[DEBUG]nnamenum++");
				j=-1;
				k++;
				array[i].nameNum++;
				continue;
			}
			array[i].NICKNAME[nnamenum][j]=cuProLine[k];
			k++;
		}
	}
	fclose(stdin);
	freopen("CON","r",stdin);
	switch(settings.lang) {
		case 0: {
			freopen("lang/en_us/studentsLoaded.lang","r",stdin);
			break;
		}
		case 1: {
			freopen("lang/zh_cn/studentsLoaded.lang","r",stdin);
			break;
		}
		default: {
			freopen("lang/zh_cn/studentsLoaded.lang","r",stdin);
			break;
		}
	}
	while(1) {
		fgets(cuProLine,CUPROLINEMAX,stdin);
		if(cuProLine[0]=='&')
			break;
		printf("%s",cuProLine);
	}
	fclose(stdin);
	freopen("CON","r",stdin);
	usleep(333333);
}
//What's your score/grade?
void logging() {
	int i,j;
	int score;
	char level[11];
	int esc=0;
	int nnamenum=0;
	int flag;
	i=1;
	bool succeeded=false;
	char maxoutExit;
	states.cuLoggedStuNum=0;
	while(esc==0) {
		if(settings.debugMode==1)printf("[Logging]%d students logged\n",states.cuLoggedStuNum);
		succeeded=false;
		if(states.cuLoggedStuNum>=cuClass.stuNum) {
			switch(settings.lang) {
				case 0: {
					freopen("lang/en_us/loggedStuNumReached.lang","r",stdin);
					break;
				}
				case 1: {
					freopen("lang/zh_cn/loggedStuNumReached.lang","r",stdin);
					break;
				}
				default: {
					freopen("lang/zh_cn/loggedStuNumReached.lang","r",stdin);
					break;
				}
			}
			while(1) {
				fgets(cuProLine,CUPROLINEMAX,stdin);
				if(cuProLine[0]=='&')
					break;
				printf("%s",cuProLine);
			}
			fclose(stdin);
			freopen("CON","r",stdin);
			maxoutExit=getche();
			if(maxoutExit!='a') {
				goto loggingStopped;
			}
			printf("\n");
		}
		printf("%d.",i);
		scanf("%s",cuProLine);
		if(cuProLine[0]=='&') {
			esc=1;
			break;
		}
		if(esc==1)break;
		switch(settings.cuT) {
			case 0: {
				scanf("%d",&score);
				if(settings.debugMode==1)printf("[Logging]score=%d\n",score);
				break;
			}
			case 1: {
				scanf("%s",level);
				if(settings.debugMode==1)printf("[Logging]grade=%s\n",level);
				break;
			}
			default: {
				scanf("%d",&score);
				if(settings.debugMode==1)printf("[Logging]score=%d\n",score);
				break;
			}
		}
		flag=0;
		for(j=1; j<=cuClass.stuNum; j++) {
			//	if(settings.debugMode==1)printf("[Logging]j++,j=%d\n",j);
			if(strcmp(array[j].CHN,cuProLine)==0) { //Compare CHN name input
				if(settings.debugMode==1)printf("[Logging]%d-%s-ScoreLoggedInWithCHN\n",j,array[j].CHN);
				succeeded=true;
				switch(settings.cuT) {
					case 0: {
						if(settings.debugMode==1)printf("[Logging]originalscore=%d\n",array[j].exRes.score);
						if(array[j].exRes.score==0)states.cuLoggedStuNum++;
						array[j].exRes.score=score;
						if(settings.debugMode==1)printf("[Logging]score=%d\n",array[j].exRes.score);
						break;
					}
					case 1: {
						if(array[j].hwRes.grade[0]=='*')states.cuLoggedStuNum++;
						strcpy(array[j].hwRes.grade,level);
						break;
					}
					default: {
						if(settings.debugMode==1)printf("[Logging]originalscore=%d\n",array[j].exRes.score);
						if(array[j].exRes.score==0)states.cuLoggedStuNum++;
						array[j].exRes.score=score;
						if(settings.debugMode==1)printf("[Logging]score=%d\n",array[j].exRes.score);
						break;
					}
				}
				break;
			}
			for(nnamenum=0; nnamenum<=MAXNNAMENUM; nnamenum++) { //Compare NICKNAME input
				if(strcmp(array[j].NICKNAME[nnamenum],cuProLine)==0) {
					if(settings.debugMode==1)printf("[Logging]%d-%s-ScoreLoggedInWithNickname%d\n",j,array[j].CHN,nnamenum);
					succeeded=true;
					switch(settings.cuT) {
						case 0: {
							if(settings.debugMode==1)printf("[Logging]originalscore=%d\n",array[j].exRes.score);
							if(array[j].exRes.score==0)states.cuLoggedStuNum++;
							array[j].exRes.score=score;
							if(settings.debugMode==1)printf("[Logging]score=%d\n",array[j].exRes.score);
							break;
						}
						case 1: {
							if(array[j].hwRes.grade[0]=='*')states.cuLoggedStuNum++;
							strcpy(array[j].hwRes.grade,level);
							break;
						}
						default: {
							if(settings.debugMode==1)printf("[Logging]originalscore=%d\n",array[j].exRes.score);
							if(array[j].exRes.score==0)states.cuLoggedStuNum++;
							array[j].exRes.score=score;
							if(settings.debugMode==1)printf("[Logging]score=%d\n",array[j].exRes.score);
							break;
						}
					}
					flag=1;
					break;
				}
			}
			if(flag==1)break;
		}
		if(succeeded==false) {
			switch(settings.lang) {
				case 0: {
					freopen("lang/en_us/noStudent.lang","r",stdin);
					break;
				}
				case 1: {
					freopen("lang/zh_cn/noStudent.lang","r",stdin);
					break;
				}
				default: {
					freopen("lang/zh_cn/noStudent.lang","r",stdin);
					break;
				}
			}
			while(1) {
				fgets(cuProLine,CUPROLINEMAX,stdin);
				if(cuProLine[0]=='&')
					break;
				printf("%s",cuProLine);
			}
			fclose(stdin);
			freopen("CON","r",stdin);
			continue;
		}
		if(settings.debugMode==1)printf("[Logging]%s,%d,%s\n",array[j].CHN,array[j].exRes.score,array[j].hwRes.grade);
		i++;
	}
loggingStopped:
	printf("\n");
	i++;
	i--;
}
//Who will need to draw a chart now?
void printCSV() {
	int i;
	fclose(stdout);
	freopen("score.csv","w",stdout);
	switch(settings.lang) {
		case 0: {
			freopen("lang/en_us/output-N+S.lang","r",stdin);
			break;
		}
		case 1: {
			freopen("lang/zh_cn/output-N+S.lang","r",stdin);
			break;
		}
		default: {
			freopen("lang/zh_cn/output-N+S.lang","r",stdin);
			break;
		}
	}
	while(1) {
		fgets(cuProLine,CUPROLINEMAX,stdin);
		if(cuProLine[0]=='&')
			break;
		printf("%s",cuProLine);
	}
	for(i=settings.ELAC; i>=1; i--)printf(",\n");
	switch(settings.cuT) {
		case 0: {
			for(i=1; i<=cuClass.stuNum; i++)
				if(array[i].CHN[0]!=' ')
					printf("%s,%d\n",array[i].CHN,array[i].exRes.score);
			break;
		}
		case 1: {
			for(i=1; i<=cuClass.stuNum; i++)
				if(array[i].CHN[0]!=' ')
					printf("%s,%s\n",array[i].CHN,array[i].hwRes.grade);
			break;
		}
		default: {
			for(i=1; i<=cuClass.stuNum; i++)
				if(array[i].CHN[0]!=' ')
					printf("%s,%d\n",array[i].CHN,array[i].exRes.score);
			break;
		}
	}
	fclose(stdout);
	freopen("CON","w",stdout);
}
//After Init is over, perpare program for use
void prepare() {
	if(settings.debugMode==false)
		system("cls");
	showVersion();
	switch(settings.lang) {
		case 0: {
			freopen("lang/en_us/startLogging.lang","r",stdin);
			break;
		}
		case 1: {
			freopen("lang/zh_cn/startLogging.lang","r",stdin);
			break;
		}
		default: {
			freopen("lang/zh_cn/startLogging.lang","r",stdin);
			break;
		}
	}
	while(1) {
		fgets(cuProLine,CUPROLINEMAX,stdin);
		if(cuProLine[0]=='&')
			break;
		printf("%s",cuProLine);
	}
	fclose(stdin);
	freopen("CON","r",stdin);
}
void confirmation() {
	fclose(stdin);
	switch(settings.lang) {
		case 0: {
			freopen("lang/en_us/confirmation.lang","r",stdin);
			break;
		}
		case 1: {
			freopen("lang/zh_cn/confirmation.lang","r",stdin);
			break;
		}
		default: {
			freopen("lang/zh_cn/confirmation.lang","r",stdin);
			break;
		}
	}
	int i=0;
	char changeSettings;
	while(1) {
		fgets(cuProLine,CUPROLINEMAX,stdin);
		if(cuProLine[0]=='&')
			break;
		printf("%s",cuProLine);
		if(i==1)printf("%d\n",cuClass.num);
		if(i==2) {
			switch(settings.cuT) {
				case(0): {
					fgets(cuProLine,CUPROLINEMAX,stdin);
					printf("%s",cuProLine);
					fgets(cuProLine,CUPROLINEMAX,stdin);
					break;
				}
				case(1): {
					fgets(cuProLine,CUPROLINEMAX,stdin);
					fgets(cuProLine,CUPROLINEMAX,stdin);
					printf("%s",cuProLine);
					break;
				}
			}
		}
		if(i==3){
			switch(settings.sort.enabled){
				case(0):{
					fgets(cuProLine,CUPROLINEMAX,stdin);
					fgets(cuProLine,CUPROLINEMAX,stdin);
					printf("%s",cuProLine);
					break;
			}
				case(1):{
					fgets(cuProLine,CUPROLINEMAX,stdin);
					printf("%s",cuProLine);
					fgets(cuProLine,CUPROLINEMAX,stdin);
					break;
				}
			}
		}
		i++;
	}
	fclose(stdin);
	freopen("CON","r",stdin);
	changeSettings=getche();
	if(changeSettings=='a') {
		printf("\n");
		if(settings.debugMode==false)
			system("cls");
		showVersion();
		clearStudentInfo();
		changeCurrentSettings();
		loadStudentInfo();
		showDebugInfo("loadStudents");
	}
}
void changeCurrentSettings()
{
	selectClass("changingSettings");
	clearStudentInfo();
	LoadStuCCS:
	loadStudentInfo();
	if(settings.restartRequired==true) {
		settings.restartRequired=false;
		selectClass("defaultClassNotFound");
		showDebugInfo("loadStudents");
		goto LoadStuCCS;
	}
	selectType("changingSettings");
	selectSorting();
}
void goodbye() {
	fclose(stdin);
	switch(settings.lang) {
		case 0: {
			freopen("lang/en_us/finished.lang","r",stdin);
			break;
		}
		case 1: {
			freopen("lang/zh_cn/finished.lang","r",stdin);
			break;
		}
		default: {
			freopen("lang/zh_cn/finished.lang","r",stdin);
			break;
		}
	}
	while(1) {
		fgets(cuProLine,CUPROLINEMAX,stdin);
		if(cuProLine[0]=='&')
			break;
		printf("%s",cuProLine);
	}
	fclose(stdin);
	freopen("CON","r",stdin);
}
void clearStudentInfo() {
	int i,j,k;
	for(i=0; i<=MAXSTU-1; i++) {
		for(j=0; j<=MAXCHN; j++)
			array[i].CHN[j]='\0';
		for(j=0; j<=MAXNNAMENUM-1; j++) {
			for(k=0; k<=MAXNNAMELEN-1; k++)
				array[i].NICKNAME[j][k]='\0';
		}
		array[i].nameNum=0;
		array[i].exRes.score=0;
		array[i].exRes.ranking=0;
		array[i].hwRes.grade[0]='\0';
	}
}
void bucketSorting() {
	int i;
	struct node *t;
	for(i=1; i<=cuClass.stuNum; i++) {
		if(array[i].exRes.score>=0) {
			if(buckets[array[i].exRes.score].head==NULL) {
				buckets[array[i].exRes.score].head=(struct node *)malloc(sizeof(struct node));
				buckets[array[i].exRes.score].head->num=i;
				buckets[array[i].exRes.score].head->next=NULL;
			} else {
				t=(struct node *)malloc(sizeof(struct node));
				t->num=i;
				t->next= buckets[array[i].exRes.score].head;
				buckets[array[i].exRes.score].head=t;
			}
		}
		if(array[i].exRes.score<0) {
			if(bucketException.head==NULL) {
				bucketException.head=(struct node *)malloc(sizeof(struct node));
				bucketException.head->num=i;
				bucketException.head->next=NULL;
			} else {
				t=(struct node *)malloc(sizeof(struct node));
				t->num=i;
				t->next=bucketException.head;
				bucketException.head=t;
			}
		}
	}
	showDebugInfo("bucketSorting");
}
void sorting() {
	int i;
	if(settings.sort.enabled==false || settings.cuT==1)
		printCSV();
	if(settings.sort.enabled==true) {
		//    if(settings.sort.algorithm==1)
		//    {
		for(i=1; i<=120; i++) {
			buckets[i].value=i;
			buckets[i].head=NULL;
		}
		bucketException.head=NULL;
		bucketSorting();
		printBucketSorted();
		//    }
	}
}
void printBucketSorted() {
	int i,j;
	struct node *t=NULL;
	fclose(stdout);
	freopen("score.csv","w",stdout);
	switch(settings.lang) {
		case 0: {
			freopen("lang/en_us/output-N+S.lang","r",stdin);
			break;
		}
		case 1: {
			freopen("lang/zh_cn/output-N+S.lang","r",stdin);
			break;
		}
		default: {
			freopen("lang/zh_cn/output-N+S.lang","r",stdin);
			break;
		}
	}
	for(i=settings.ELAC; i>=1; i--)printf(",\n");
	for(i=120; i>=0; i--) {
		t=buckets[i].head;
		if(t==NULL)continue;
		else {
			while(1) {
				printf("%s,%d\n",array[t->num].CHN,array[t->num].exRes.score);
				if(t->next==NULL)break;
				t=t->next;
			}
		}
	}
	t=bucketException.head;
	if(t!=NULL) {
		while(1) {
			printf("%s,%d\n",array[t->num].CHN,array[t->num].exRes.score);
			if(t->next==NULL)break;
			t=t->next;
		}
	}
	fclose(stdout);
	freopen("CON","w",stdout);
}
int main() {
	init();
	confirmation();
	prepare();
	logging();
	sorting();
	goodbye();
	system("pause");
	return 0;
}
