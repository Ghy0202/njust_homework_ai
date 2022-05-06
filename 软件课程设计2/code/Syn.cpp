#include<bits/stdc++.h>
/*??????????utf-8??vscode??????????????????????????????????????????????????????????????????????????????????*/
using namespace std;

const int maxn=100;
const int rsign=-1000;
char buffer[maxn];
const int failed=-INT_MAX;

string grammarfile="./data/syn_grammar.txt";
string tokenfile="./data/syn_token_false.txt";

struct Grammar{
    string Left;
    vector<string>Rights;
};
//TODO:error: no match for 'operator<' (operand types are 'const Closure' and 'const Closure')
struct Item{                
    string Left;
    vector<string>Rights;
    string Tail;
    int pos=0;              
  
    friend bool operator == (Item a,Item b){
        if(a.Left!=b.Left){
            return false;
        }else if(a.Tail!=b.Tail){
            return false;
        }else if(a.pos!=b.pos){
            return false;
        }else if(a.Rights.size()!=b.Rights.size()){
            return false;
        }else{
            for(int i=0;i<a.Rights.size();i++){
                if(a.Rights[i]!=b.Rights[i]){
                    return false;
                }
            }
            return true;
        }
    }
    friend bool operator <(Item a,Item b){
        if(a.Left!=b.Left){
            return a.Left<b.Left;
        }else if(a.pos!=b.pos){
            return a.pos<b.pos;
        }else if(a.Tail!=b.Tail){
            return a.Tail<b.Tail;
        }else if(a.Rights.size()!=b.Rights.size()){
            return a.Rights.size()<b.Rights.size();
        }else{
            for(int i=0;i<a.Rights.size();i++){
                if(a.Rights[i]!=b.Rights[i]){
                    return a.Rights[i]<b.Rights[i];
                }
            }
            return true;
        }
    }
};
struct Closure{            
    set<Item>items;
    int state;
   
    friend bool operator ==(Closure a,Closure b){
       
        return a.items==b.items;
        
    }
    friend bool operator <(Closure a,Closure b){
        return a.state<b.state;
    }
};
struct Relation{
    set<string>v;
};
struct Table_Item{//LR(1)������
    int STATE;
    map<string,int>ACTION;
    map<string,int>GOTO;
};
struct Table{
    vector<Table_Item>tabel_item;
};
struct Token{
    int line;
    string word;
    int type;
};

vector<Grammar>grammars;
set<string>Vn;
set<string>Vt;
map<string,set<string> >first_set;
map<string,bool>toNull;
set<Closure>closures;
vector<vector<int> >rels(200);
Relation relations[300][300];
vector<Token>tokens;
Table table;
Grammar getLineGrammar(string str){
    int len=str.length();
    Grammar ans;
    bool hasLeft=false;
    bool flag=false;
    string temp="";
    for(int i=0;i<len;i++){
        if(str[i]=='['){
            flag=true;
        }else if(str[i]==']'){
            flag=false;
            if(temp!=""){
                if(!hasLeft){
                    ans.Left=temp;
                    Vn.insert(temp);
                    hasLeft=true;
                }else{
                    if(!(temp[0]<='Z'&&temp[0]>='A')){
                        Vt.insert(temp);
                    }
                    ans.Rights.push_back(temp);
                }
            }
            temp="";
        }else{
            if(flag){
                temp+=str[i];
            }
        }
    }
    if(ans.Rights.size()==1&&ans.Rights[0]=="final"){
        toNull[ans.Left]=true;
    }
    return ans;
}
bool isVT(string str){
    if(str[0]<='Z'&&str[0]>='A'){
        return false;
    }else{
        return true;
    }
}
bool isVn(string str){
    if(str.length()!=0){
        if(str[0]<='Z'&&str[0]>='A'){
            return true;
        }
    }
    return false;
}

//1������ս����First����

//����ս����First����
bool getFirst_init(string vn){
    set<string>ans;
    set<string>nu;
    nu.insert("final");
    bool flag=true;
    //��򵥵����־��Ǳ������?ʼ��ʽ��ֱ�ӵó�First���ϵ�
    for(int i=0;i<grammars.size();i++){
       if(grammars[i].Left==vn){
           string f=grammars[i].Rights[0];
           int len=grammars[i].Rights.size();
           int cnt=0;
           while(cnt<len){
               if(isVT(f)){
                    ans.insert(f);
                    break;
                }else if(first_set.find(f)!=first_set.end()){
               //��Ҫ�жϸ÷��ս���Ƿ�����Ƶ���final��
                    if(toNull.find(f)==toNull.end()){
                    //�Ƶ����գ���Ҫ������һ�����ж�
                    set<string>temp;//����final�Ĳ
                    set_difference( first_set[f].begin(), first_set[f].end(),nu.begin(), nu.end(),inserter( temp, temp.begin() ) );    /*ȡ�����*/
                    ans.insert(temp.begin(),temp.end());
                    cnt++;
                    }else{
                    //û���Ƶ����գ�����ս����First���Ϻϲ�����ǰ
                        ans.insert(first_set[f].begin(),first_set[f].end());
                        break;
                    }
                }else{
                    flag=false;//��û�еõ�����First����
                    break;
                }
           }
           if(cnt>=len){
               //˵����������β��
               ans.insert("final");
           }
       }
    }
    if(flag){
        first_set[vn]=ans;
    }
    return flag;
    
}
void _init_(){
    ifstream input;
    input.open(grammarfile);
    if(!input.is_open()){
        printf("FAILED TO LOAD GRAMMARS\n");
    }else{
        while(input.getline(buffer,maxn-1)){
            grammars.push_back(getLineGrammar(buffer));
        }
    }
    //��First����
    int cnt=1;
    while(cnt){
        cnt=0;
        for(set<string>::iterator it=Vn.begin();it!=Vn.end();it++){
            if(getFirst_init(*it)==false){
                cnt++;
            }
        }
    }
}
//2��LR��Ŀ���Ĺ���
//�õ�����������һ���ַ�
string getNext(Item item){
    int next_pos=item.pos;
    if(next_pos>=item.Rights.size()){
        return "[DANGER]";//���磺A->BC�� û����һ����
    }else if(item.Rights[next_pos]=="final"){
        return "[FINAL]";//���磺A->��final û����һ���ַ�
    }else{
        return item.Rights[next_pos];
    }
}
//����ת������
Closure Go(Closure clo,string str){
    //closure-str->new_closure
    Closure ans;
    Item temp_item;
    //ԭ����clo��������һλ��str���ƶ�����?��
    for(set<Item>::iterator it=clo.items.begin();it!=clo.items.end();it++){
        if(getNext(*it)==str){
            temp_item.Left=(*it).Left;
            temp_item.Rights=(*it).Rights;
            temp_item.pos=(*it).pos+1;
            temp_item.Tail=(*it).Tail;
            ans.items.insert(temp_item);
        }
    }

   return ans;
}

//�õ�A->��Ban,op�е�anop
vector<string> getPreTail(Item item){
    vector<string>ans;
    for(int i=item.pos+1;i<item.Rights.size();i++){
        ans.push_back(item.Rights[i]);
    }
    //��������ԭ����β��
    ans.push_back(item.Tail);
    return ans;
}
//�õ�First(ba)
set<string> getFirst(vector<string>ba){
    set<string>ans;
    for(int i=0;i<ba.size();i++){
        if(isVT(ba[i])){
            ans.insert(ba[i]);
            break;
        }else if(toNull.find(ba[i])!=toNull.end()){
            //���¿���Ϊ��
            ans.insert(first_set[ba[i]].begin(),first_set[ba[i]].end());
        }else{
            ans.insert(first_set[ba[i]].begin(),first_set[ba[i]].end());
            break;
        }
    }
    return ans;
}
//����հ�����?
void getClosure(Closure &clo){
    //A->a��Bb,c
    bool flag;
    while(1){
        flag=false;
        for(set<Item>::iterator it=clo.items.begin();it!=clo.items.end();it++){//������Ŀ����
            string str=getNext(*it);
            Grammar temp_grammar;
            if(str=="[DANGER]"||str=="[FINAL]"){
                continue;
            }else if(isVn(str)){
                //Ѱ��B->����
                //�ҵ���Ӧ��Tails
                for(int i=0;i<grammars.size();i++){
                    if(grammars[i].Left==str){
                        temp_grammar=grammars[i];
                        //�ҵ�grammar��Ӧ��Tails
                        vector<string>ba=getPreTail(*it);//�����ҵ�ba
                        set<string>b=getFirst(ba);
                        //�����µ�items
                        Item new_item;
                        new_item.Left=temp_grammar.Left;
                        new_item.Rights=temp_grammar.Rights;
                        new_item.pos=0;
                        for(set<string>::iterator sit=b.begin();sit!=b.end();sit++){
                            new_item.Tail=*sit;
                            //������Ŀ�����Ƿ�������ɵ��µ����?
                            if(find(clo.items.begin(),clo.items.end(),new_item)==clo.items.end()){
                                //���������?
                                flag=true;
                                clo.items.insert(new_item);
                            }
                    }
                }else{
                    continue;
                }
            }
        }
        }
        //�հ���������
        if(flag==false){
            break;
        }
    }
}
void build_LR(){
    //����LR(1)��Ŀ���հ�����
    Item item;
    item.Left=grammars[0].Left;
    item.Rights=grammars[0].Rights;
    item.pos=0;
    item.Tail="final";
    Closure clo;
    clo.items.insert(item);
    clo.state=0;
    getClosure(clo);//���ڵ�һ����бհ�?
    closures.insert(clo);
    stack<Closure>st;//�洢û��GO����closure
    st.push(clo);
    Closure temp_clo;//���ɵ�
    int cnt=0;//��¼��Ŀ�����?
    while(!st.empty()){
        clo=st.top();
        st.pop();
        int nid=clo.state;
        int pid;
        //���ȱ����õ����п��ܵ���һ��str
        set<string>strs;
        for(set<Item>::iterator it=clo.items.begin();it!=clo.items.end();it++){
            string nx=getNext(*it);
            if(nx!="[FINAL]"&&nx!="[DANGER]"){
                strs.insert(nx);
            }
        }
        //Go,�½�һ����Ŀ
        printf("***********************next str:%d\n",strs.size());
        for(set<string>::iterator it=strs.begin();it!=strs.end();it++){
            temp_clo=Go(clo,*it);
            getClosure(temp_clo);
            if(temp_clo.items.size()==0){
                //����Ŀ����һ��
                continue;
            }
            //�ж������Ŀ�?ǰ�Ƿ��й�
            set<Closure>::iterator cit=find(closures.begin(),closures.end(),temp_clo);
            if(cit!=closures.end()){
                //֮ǰ�й�
                pid=(*cit).state;
            }else{
                //֮ǰû�й�
                cnt++;
                pid=cnt;
                temp_clo.state=cnt;
                closures.insert(temp_clo);
                st.push(temp_clo);
            }
            //��¼һ��
            printf("%d %d\n",pid,nid);
            rels[nid].push_back(pid);
            relations[nid][pid].v.insert(*it);
        }
        printf("ok\n");
        
    }
}

int getRid(string Left,vector<string>Rights){
    for(int i=0;i<grammars.size();i++){
        if(grammars[i].Left==Left){
            if(Rights.size()==grammars[i].Rights.size()){
                bool flag=true;
                for(int i=0;i>Rights.size();i++){
                    if(Rights[i]!=grammars[i].Rights[i]){
                        flag=false;
                        break;
                    }
                }
                if(flag){
                    return i;
                }
            }
        }
    }
    return -1;
}

void build_Table(){
   
    for(set<Closure>::iterator it=closures.begin();it!=closures.end();it++){
        Table_Item titem;
        
        int nid=(*it).state;
        titem.STATE=nid;
        for(int i=0;i<rels[nid].size();i++){
            int j=rels[nid][i];
            for(set<string>::iterator it=relations[nid][j].v.begin();it!=relations[nid][j].v.end();it++){
                string str=*it;
                if(isVT(str)){
                    titem.ACTION[str]=j;
                }else{
                    titem.GOTO[str]=j;
                }
            }
        }
        //����Ŀ������r:��ʲô��־��-1000+r:��Ϊ0��ʶ����
        for(set<Item>::iterator tit=(*it).items.begin();tit!=(*it).items.end();tit++){
            if((*tit).pos==(*tit).Rights.size()){//A->a��
                int rid=getRid((*tit).Left,(*tit).Rights);
                titem.ACTION[(*tit).Tail]=rsign+rid;
            }
        }
        table.tabel_item.push_back(titem);
    }
}
//LR����ʷ����������������ڳ�����?�Ƿ�Ϸ����ж�?
//��ȡ�ʷ��������Ĵ������?
Token getToken(string str){
    Token token;
    stringstream sstream(str);
    int cnt=0;
    while(sstream){
        string substr;
        sstream>>substr;
        cnt++;
        if(cnt==1){
            token.line=stoi(substr);
        }
        if(cnt==2){
            token.word=substr;
        }else if(cnt==3){
            //0-���? 1-�ؼ��� 2-�����? 3-���� 4-��ʶ�� 
            if(substr=="keyword"){
                token.type=1;
            }else if(substr=="sign"){
                token.type=4;
            }else if(substr=="limited"){
                token.type=0;
            }else if(substr=="const"){
                token.type=3;
            }else if(substr=="operator"){
                token.type=2;
            }else{
                token.type=-1;//�Ƿ�
            }
        }
    }
    return token;
}
void readWord_res(){
    ifstream input;
    input.open(tokenfile);
    if(!input.is_open()){
        printf("FAILED TO LOAD TOKENS\n");
    }else{
        while(input.getline(buffer,maxn-1)){
            tokens.push_back(getToken(buffer));
        }
    }
}
//���뵱ǰ״̬�Լ�������ַ���?
int getStateId(int cur_state_id,string str){
    //TODO:����Ҫȷ��һ���Ƿ����ֱ��������?
    for(int i=0;i<table.tabel_item.size();i++){
        if(table.tabel_item[i].STATE==cur_state_id){
            if(table.tabel_item[i].ACTION.find(str)!=table.tabel_item[i].ACTION.end()){
                return table.tabel_item[i].ACTION[str];
            }else{
                printf("%d Failed to find ",cur_state_id);
                cout<<str<<"!\n";
                return failed;
            }
            
        }
    }
    return failed;
    
}
int getGotoId(int state_id,string str){
    //TODO:����Ҫȷ��һ���Ƿ����ֱ��������?
    for(int i=0;i<table.tabel_item.size();i++){
        if(table.tabel_item[i].STATE==state_id){
            if(table.tabel_item[i].GOTO.find(str)!=table.tabel_item[i].GOTO.end()){
                return table.tabel_item[i].GOTO[str];
            }else{
                return failed;
            }
            break;
        }
    }
    return failed;
}
//LR(1)����
/*
ע�⣺
0-���? 
1-�ؼ��� 
2-�����? 
3-���� 
4-��ʶ�� 
*/
void ERR(int cur_state_id,string str){
    //��ǰ��״̬�±꣬�Լ���һ���±꣬ԭ��
    //TODO:����Ҫȷ��һ���Ƿ����ֱ��������?
    for(int i=0;i<table.tabel_item.size();i++){
        if(table.tabel_item[i].STATE==cur_state_id){
            //������ǰ״̬���ܵ�ACTION
            for(map<string,int>::iterator it=table.tabel_item[i].ACTION.begin();it!=table.tabel_item[i].ACTION.end();it++){
                cout  << "\033[31m"<<it->first<<"\t"<<"\033[0m"<< endl ;
            }
            
        }
    }
}
bool Analyse_LR(vector<Token>tokens){
    stack<int>states;//״̬ջ
    stack<string>symbols;//����ջ
    
    states.push(0);//��ʼ״̬��Ϊ0
    string str;//��ǰ������ַ���?
    for(int i=0;i<tokens.size();i++){
        //���ȷ��Ż�һ�ַ�ʽ����
        if(tokens[i].type==0){
            //����ǽ������Ҫ�����?
            str="\'";
            str+=tokens[i].word;
            str+="\'";
        }else if(tokens[i].type==1){
            //����ǹؼ��֣�Ҳ���?�����?
            str="\'";
            str+=tokens[i].word;
            str+="\'";
        }else if(tokens[i].type==2){
            //��������������Ҫ�����?
            str="\'";
            str+=tokens[i].word;
            str+="\'";
        }else if(tokens[i].type==3){
            //����ǳ������߱�ʶ����Ҳ�����?�����?
            str="const";
        }else if(tokens[i].type==4){
            str="sign";
        }
        
        //��Table���ҵ���ǰ״̬��Ӧ����һ��ACtION
        int cur_state_id=states.top();
        int next_state_id=getStateId(cur_state_id,str);
        //cout<<"cur_str\t\t"<<str<<" "<<cur_state_id<<" "<<next_state_id<<"\n\n";
        if(next_state_id==failed){
            printf("\033[31mFAILED!\t\tline:%d expect input:\033[0m",tokens[i].line);
            ERR(cur_state_id,str);
            printf("\n");
            return false;
        }else if(next_state_id<0){
            //�������˼�ǣ�r,��ȡr�ı�ʾ
            next_state_id-=rsign;
            //��ȡr��Ӧ�Ĳ���ʽ
            Grammar grammar=grammars[next_state_id];

            


            //״̬��ջ������ջ��ջ
            for(int i=grammar.Rights.size()-1;i>=0;i--){
                symbols.pop();
                states.pop();
            }
            //�µķ���ѹջ
            symbols.push(grammar.Left);
            //Ȼ����״̬��GOTOת��
            int goto_id=getGotoId(states.top(),grammar.Left);
            states.push(goto_id);
            i--;//���ǣ����봮����
        }else{
            //��ͨ��S��ת
            states.push(next_state_id);
            symbols.push(str);
        }
    }
    return true;
}
//����LR(1)�����Ƿ���ȷ
void Test(){
    if(Analyse_LR(tokens)){
        printf("YES\n");
    }else{
        printf("NO\n");
    }
}
/*һЩ��bug�ĺ������ǵ�ɾ��*/
void show_init(){
    //չ�ֳ�ʼ������Ƿ����?
    printf("Grammar:\n");
    for(int i=0;i<grammars.size();i++){
        cout<<grammars[i].Left<<" ";
        for(int j=0;j<grammars[i].Rights.size();j++){
            cout<<grammars[i].Rights[j]<<",";
        }
        cout<<"\n";
    }
    printf("Vn:\n");
    for(set<string>::iterator it=Vn.begin();it!=Vn.end();it++){
        cout<<*it<<endl;
    }
    printf("Vt:\n");
    for(set<string>::iterator it=Vt.begin();it!=Vt.end();it++){
        cout<<*it<<endl;
    }
    printf("First:\n");
    for(map<string,set<string> >::iterator it=first_set.begin();it!=first_set.end();it++){
        cout<<it->first<<":";
        for(set<string>::iterator mit=(it->second).begin();mit!=(it->second).end();mit++){
            cout<<*mit<<" ";
        }
        cout<<"\n";
    }
    //չʾtoken��
    printf("Tokens:\n");
    for(int i=0;i<tokens.size();i++){
        cout<<tokens[i].type<<" : "<<tokens[i].word<<"\n";
    }
}
//չʾһ��LR��Ŀ��
void showLR(){
    printf("***************************************%d\n",closures.size());
    for(set<Closure>::iterator it=closures.begin();it!=closures.end();it++){
        //չ��closure
        printf("%d:\n",(*it).state);
        //������Ŀ��
        for(set<Item>::iterator iit=(*it).items.begin();iit!=(*it).items.end();iit++){
            cout<<(*iit).Left<<" : ";
            for(int i=0;i<(*iit).Rights.size();i++){
                if(i){
                    printf(",");
                }
                cout<<(*iit).Rights[i];
            }
            cout<<"\t\t\t|"<<(*iit).Tail<<endl;
        }
    }
}
void showTable(){
    //չ��Table
    for(int i=0;i<table.tabel_item.size();i++){
        printf("-----------------------------------------------------------\n");
        printf("%d:\n",table.tabel_item[i].STATE);
        printf("ACTION:\n");
        for(map<string,int>::iterator it=table.tabel_item[i].ACTION.begin();it!=table.tabel_item[i].ACTION.end();it++){
            cout<<"["<<it->first<<"]:"<<it->second<<endl;
        }
        printf("GOTO:\n");
        for(map<string,int>::iterator it=table.tabel_item[i].GOTO.begin();it!=table.tabel_item[i].GOTO.end();it++){
            cout<<"["<<it->first<<"]:"<<it->second<<endl;
        }
    }
}
int main(){
    _init_();
    readWord_res();
    show_init();
    build_LR();
    printf("-----------------------------------show---------------------------------------\n");
    showLR();
    build_Table();
    showTable();
    //�����Ƿ���ȷ
    Test();

    return 0;  
}