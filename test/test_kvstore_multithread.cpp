#include "../Util/Util.h"
#include "KVstore.h"
#define rep(i,k,n) for(int i=(k);i<=(n);i++)
using namespace std;
const int maxn=10000000;
const int thnum=20;
KVstore testkv;

void generate_test_data()
{
	FILE *data_file1=fopen("./data1","w");
	FILE *data_file2=fopen("./data2","w");
	fprintf(data_file1,"%d\n",maxn);
	fprintf(data_file2,"%d\n",maxn);
	for(int i=0;i<maxn;i++)
	{
		string str="";
		char tmp[3];
		tmp[1]=0;
		int k=i;
		for(int j=0;j<7;j++)
		{
			tmp[0]='a'+k%26;
			str.append(tmp);
			k/=26;
		}
		fprintf(data_file1,"%d %s\n",i,str.c_str());
		
		k=i;
		int o=k%800;
		k/=800;
		int p=k%800;
		k/=800;
		int s=k%800;
		fprintf(data_file2,"%d %d %d\n",s,p,o);
	}
	fclose(data_file1);
	fclose(data_file2);
}

//for checking memory
int parseLine(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = 0;
    i = atoi(p);
    return i;
}

int getMemkb(){ //Note: this value is in KB!
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmSize:", 7) == 0){
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return result;
}


//string2id
void test_string2id_insert()
{
	cout<< "testing string2id insert..." <<endl;
	testkv.open_entity2id(KVstore::CREATE_MODE);
	
	FILE *data1=fopen("./data1","r");
	
	timeval start_time,end_time;
	gettimeofday(&start_time,NULL);
	
	int n,id;
	char str[10];
	fscanf(data1,"%d",&n);
	for(int i=0;i<n;i++)
	{
		fscanf(data1,"%d %s",&id,str);
		testkv.setIDByEntity(string(str),id);
	}
	gettimeofday(&end_time,NULL);
	cout<< "time(s) : " << end_time.tv_sec-start_time.tv_sec + (end_time.tv_usec-start_time.tv_usec)/1000000.0 <<endl;
	cout<< "memory(kb) : " << getMemkb() <<endl <<endl;
	
	fclose(data1);
	testkv.close_entity2id();
}

void test_string2id_delete()
{
	cout<< "testing string2id delete..." <<endl;
	testkv.open_entity2id(KVstore::READ_WRITE_MODE);
	
	FILE *data1=fopen("./data1","r");
	
	timeval start_time,end_time;
	gettimeofday(&start_time,NULL);
	
	int n,id;
	char str[10];
	fscanf(data1,"%d",&n);
	for(int i=0;i<n;i++)
	{
		fscanf(data1,"%d %s",&id,str);
		testkv.subIDByEntity(string(str));
	}
	gettimeofday(&end_time,NULL);
	cout<< "time(s) : " << end_time.tv_sec-start_time.tv_sec + (end_time.tv_usec-start_time.tv_usec)/1000000.0 <<endl;
	cout<< "memory(kb) : " << getMemkb() <<endl <<endl;
	
	fclose(data1);
	testkv.close_entity2id();
}

int id[thnum];
char str[thnum][10];

void* th_string2id_search(void *i)
{
	int t=(int)i;
	testkv.getIDByEntity(string(str[t]));
	pthread_exit(NULL);
}

void test_string2id_search()
{
	cout<< "testing string2id search..." <<endl;
	testkv.open_entity2id(KVstore::READ_WRITE_MODE);

	
	timeval start_time,end_time;
	gettimeofday(&start_time,NULL);
	
	FILE *data1=fopen("./data1","r");
	int n;
	fscanf(data1,"%d",&n);
	
	pthread_t pid[thnum];
	
	int cur=0;
	while(cur<n)
	{
		rep(i,0,min(n-cur,thnum)-1)fscanf(data1,"%d %s",&id[i],str[i]);
		for (int i = 0; i < min(n-cur,thnum); i++)
		{
			pthread_create(&pid[i], NULL, th_string2id_search, (void *)i);
		}
		for (int i = 0; i < min(n-cur,thnum); i++)
		{
			pthread_join(pid[i], NULL);
		}
		cur+=thnum;
	}
	
	gettimeofday(&end_time,NULL);
	cout<< "time(s) : " << end_time.tv_sec-start_time.tv_sec + (end_time.tv_usec-start_time.tv_usec)/1000000.0 <<endl;
	cout<< "memory(kb) : " << getMemkb() <<endl <<endl;
	
	fclose(data1);
	testkv.close_entity2id();
}


//id2string
void test_id2string_insert()
{
	cout<< "testing id2string insert..." <<endl;
	testkv.open_id2entity(KVstore::CREATE_MODE);
	
	timeval start_time,end_time;
	gettimeofday(&start_time,NULL);
	
	FILE *data1=fopen("./data1","r");
	int n,id;
	char str[10];
	fscanf(data1,"%d",&n);
	for(int i=0;i<n;i++)
	{
		fscanf(data1,"%d %s",&id,str);
		testkv.setEntityByID(id,string(str));
	}
	gettimeofday(&end_time,NULL);
	cout<< "time(s) : " << end_time.tv_sec-start_time.tv_sec + (end_time.tv_usec-start_time.tv_usec)/1000000.0 <<endl;
	cout<< "memory(kb) : " << getMemkb() <<endl <<endl;
	
	fclose(data1);
	testkv.close_id2entity();
}

void test_id2string_delete()
{
	cout<< "testing id2string delete..." <<endl;
	testkv.open_id2entity(KVstore::READ_WRITE_MODE);
	
	timeval start_time,end_time;
	gettimeofday(&start_time,NULL);
	
	FILE *data1=fopen("./data1","r");
	int n,id;
	char str[10];
	fscanf(data1,"%d",&n);
	for(int i=0;i<n;i++)
	{
		testkv.subEntityByID(i);
	}
	gettimeofday(&end_time,NULL);
	cout<< "time(s) : " << end_time.tv_sec-start_time.tv_sec + (end_time.tv_usec-start_time.tv_usec)/1000000.0 <<endl;
	cout<< "memory(kb) : " << getMemkb() <<endl <<endl;
	
	fclose(data1);
	testkv.close_id2entity();
}

void* th_id2string_search(void *i)
{
	int t=(int)i;
	testkv.getEntityByID(id[t]);
	pthread_exit(NULL);
}

void test_id2string_search()
{
	cout<< "testing id2string search..." <<endl;
	testkv.open_id2entity(KVstore::READ_WRITE_MODE);
	
	timeval start_time,end_time;
	gettimeofday(&start_time,NULL);
	
	FILE *data1=fopen("./data1","r");
	int n;
	fscanf(data1,"%d",&n);
	
	pthread_t pid[thnum];
	
	int cur=0;
	while(cur<n)
	{
		rep(i,0,min(n-cur,thnum)-1)fscanf(data1,"%d %s",&id[i],str[i]);
		for (int i = 0; i < min(n-cur,thnum); i++)
		{
			pthread_create(&pid[i], NULL, th_id2string_search, (void *)i);
		}
		for (int i = 0; i < min(n-cur,thnum); i++)
		{
			pthread_join(pid[i], NULL);
		}
		cur+=thnum;
	}
	
	gettimeofday(&end_time,NULL);
	cout<< "time(s) : " << end_time.tv_sec-start_time.tv_sec + (end_time.tv_usec-start_time.tv_usec)/1000000.0 <<endl;
	cout<< "memory(kb) : " << getMemkb() <<endl <<endl;
	
	fclose(data1);
	testkv.close_id2entity();
}


//subID2values
void test_subID2values_insert()
{
	cout<< "testing subID2values insert..." <<endl;
	testkv.open_subID2values(KVstore::CREATE_MODE);
	
	timeval start_time,end_time;
	gettimeofday(&start_time,NULL);
	
	FILE *data2=fopen("./data2","r");
	int n,s,p,o;
	fscanf(data2,"%d",&n);
	for(int i=0;i<n;i++)
	{
		fscanf(data2,"%d%d%d",&s,&p,&o);
		testkv.updateInsert_s2values(s,p,o);
	}
	gettimeofday(&end_time,NULL);
	cout<< "time(s) : " << end_time.tv_sec-start_time.tv_sec + (end_time.tv_usec-start_time.tv_usec)/1000000.0 <<endl;
	cout<< "memory(kb) : " << getMemkb() <<endl <<endl;
	
	fclose(data2);
	testkv.close_subID2values();
}

void test_subID2values_delete()
{
	cout<< "testing subID2values delete..." <<endl;
	testkv.open_subID2values(KVstore::READ_WRITE_MODE);
	
	timeval start_time,end_time;
	gettimeofday(&start_time,NULL);
	
	FILE *data2=fopen("./data2","r");
	int n,s,p,o;
	fscanf(data2,"%d",&n);
	for(int i=0;i<n;i++)
	{
		fscanf(data2,"%d%d%d",&s,&p,&o);
		testkv.updateRemove_s2values(s,p,o);
	}
	gettimeofday(&end_time,NULL);
	cout<< "time(s) : " << end_time.tv_sec-start_time.tv_sec + (end_time.tv_usec-start_time.tv_usec)/1000000.0 <<endl;
	cout<< "memory(kb) : " << getMemkb() <<endl <<endl;
	
	fclose(data2);
	testkv.close_subID2values();
}

unsigned s[thnum],p[thnum],o[thnum];

void* th_s2p_search(void *i)
{
	unsigned *plist=NULL;
	unsigned len;
	int t=(int)i;
	testkv.getpreIDlistBysubID(s[t],plist,len);
	delete[] plist;
	pthread_exit(NULL);
}

void test_subID2values_get_s2p()
{
	cout<< "testing subID2values get s2p..." <<endl;
	testkv.open_subID2values(KVstore::READ_WRITE_MODE);
	
	timeval start_time,end_time;
	gettimeofday(&start_time,NULL);
	
	FILE *data2=fopen("./data2","r");
	int n;
	fscanf(data2,"%d",&n);
	
	pthread_t pid[thnum];
	
	int cur=0;
	while(cur<n)
	{
		rep(i,0,min(n-cur,thnum)-1)fscanf(data2,"%d%d%d",&s[i],&p[i],&o[i]);
		for (int i = 0; i < min(n-cur,thnum); i++)
		{
			pthread_create(&pid[i], NULL, th_s2p_search, (void *)i);
		}
		for (int i = 0; i < min(n-cur,thnum); i++)
		{
			pthread_join(pid[i], NULL);
		}
		cur+=thnum;
	}
	gettimeofday(&end_time,NULL);
	cout<< "time(s) : " << end_time.tv_sec-start_time.tv_sec + (end_time.tv_usec-start_time.tv_usec)/1000000.0 <<endl;
	cout<< "memory(kb) : " << getMemkb() <<endl <<endl;
	
	fclose(data2);
	testkv.close_subID2values();
}

void* th_s2o_search(void *i)
{
	unsigned *olist;
	unsigned len;
	int t=(int)i;
	testkv.getobjIDlistBysubID(s[t],olist,len);
	delete[] olist;
	pthread_exit(NULL);
}

void test_subID2values_get_s2o()
{
	cout<< "testing subID2values get s2o..." <<endl;
	testkv.open_subID2values(KVstore::READ_WRITE_MODE);
	
	timeval start_time,end_time;
	gettimeofday(&start_time,NULL);
	
	FILE *data2=fopen("./data2","r");
	int n;
	fscanf(data2,"%d",&n);
	pthread_t pid[thnum];
	
	int cur=0;
	while(cur<n)
	{
		rep(i,0,min(n-cur,thnum)-1)fscanf(data2,"%d%d%d",&s[i],&p[i],&o[i]);
		for (int i = 0; i < min(n-cur,thnum); i++)
		{
			pthread_create(&pid[i], NULL, th_s2o_search, (void *)i);
		}
		for (int i = 0; i < min(n-cur,thnum); i++)
		{
			pthread_join(pid[i], NULL);
		}
		cur+=thnum;
	}
	
	gettimeofday(&end_time,NULL);
	cout<< "time(s) : " << end_time.tv_sec-start_time.tv_sec + (end_time.tv_usec-start_time.tv_usec)/1000000.0 <<endl;
	cout<< "memory(kb) : " << getMemkb() <<endl <<endl;
	
	fclose(data2);
	testkv.close_subID2values();
}

void* th_sp2o_search(void *i)
{
	unsigned *olist;
	unsigned len;
	int t=(int)i;
	testkv.getobjIDlistBysubIDpreID(s[t],p[t],olist,len);
	delete[] olist;
	pthread_exit(NULL);
}

void test_subID2values_get_sp2o()
{
	cout<< "testing subID2values get sp2o..." <<endl;
	testkv.open_subID2values(KVstore::READ_WRITE_MODE);
	
	timeval start_time,end_time;
	gettimeofday(&start_time,NULL);
	
	FILE *data2=fopen("./data2","r");
	int n;
	fscanf(data2,"%d",&n);
	pthread_t pid[thnum];
	
	int cur=0;
	while(cur<n)
	{
		rep(i,0,min(n-cur,thnum)-1)fscanf(data2,"%d%d%d",&s[i],&p[i],&o[i]);
		for (int i = 0; i < min(n-cur,thnum); i++)
		{
			pthread_create(&pid[i], NULL, th_sp2o_search, (void *)i);
		}
		for (int i = 0; i < min(n-cur,thnum); i++)
		{
			pthread_join(pid[i], NULL);
		}
		cur+=thnum;
	}
	gettimeofday(&end_time,NULL);
	cout<< "time(s) : " << end_time.tv_sec-start_time.tv_sec + (end_time.tv_usec-start_time.tv_usec)/1000000.0 <<endl;
	cout<< "memory(kb) : " << getMemkb() <<endl <<endl;
	
	fclose(data2);
	testkv.close_subID2values();
}

void* th_s2po_search(void *i)
{
	unsigned *polist;
	unsigned len;
	int t=(int)i;
	testkv.getpreIDobjIDlistBysubID(s[t],polist,len);
	delete[] polist;
	pthread_exit(NULL);
}

void test_subID2values_get_s2po()
{
	cout<< "testing subID2values get s2po..." <<endl;
	testkv.open_subID2values(KVstore::READ_WRITE_MODE);
	
	timeval start_time,end_time;
	gettimeofday(&start_time,NULL);
	
	FILE *data2=fopen("./data2","r");
	int n;
	fscanf(data2,"%d",&n);
	pthread_t pid[thnum];
	
	int cur=0;
	while(cur<n)
	{
		rep(i,0,min(n-cur,thnum)-1)fscanf(data2,"%d%d%d",&s[i],&p[i],&o[i]);
		for (int i = 0; i < min(n-cur,thnum); i++)
		{
			pthread_create(&pid[i], NULL, th_s2po_search, (void *)i);
		}
		for (int i = 0; i < min(n-cur,thnum); i++)
		{
			pthread_join(pid[i], NULL);
		}
		cur+=thnum;
	}
	
	gettimeofday(&end_time,NULL);
	cout<< "time(s) : " << end_time.tv_sec-start_time.tv_sec + (end_time.tv_usec-start_time.tv_usec)/1000000.0 <<endl;
	cout<< "memory(kb) : " << getMemkb() <<endl <<endl;
	
	fclose(data2);
	testkv.close_subID2values();
}

int main(int argc, char *argv[])
{
	if(argc==2 && strcmp(argv[1],"generate")==0)
		generate_test_data();
	
	//string2id
	test_string2id_insert();
	test_string2id_search();
	test_string2id_delete();
	
	//id2string
	test_id2string_insert();
	test_id2string_search();
	test_id2string_delete();
	
	//subID2values
	test_subID2values_insert();
	test_subID2values_get_s2p();
	test_subID2values_get_s2o();
	test_subID2values_get_sp2o();
	test_subID2values_get_s2po();
	test_subID2values_delete();
	
	return 0;
}
