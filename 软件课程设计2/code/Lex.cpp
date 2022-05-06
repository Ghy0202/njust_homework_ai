#include<bits/stdc++.h>
using namespace std;
/*
1、首先对于原程序按照空格切分，找出关键字
2、先找到诸如''和""组成的字符、字符串（判定为常量），接着找到运算符，界符，对剩下的字符进一步切分
3、切分后的字符按照DFA判断是否是正确的常量和字符串
*/
//定义文件
string KFILE="./data/keywords.txt";
string GFILE="./data/lex_grammar.txt";
string codefile="./data/input_equal.txt";
//定义结构体
struct Twostate{
    string a;
    string b;
};
struct NFA{
    string start="START";
    //vector<Relation>relations;//a与b的连接
    map<string,vector<Twostate> >relations;//中间桥梁是i的关系
    vector<string>ends;
};
struct DFA{
    set<string>start;
    vector<set<string> >closures;//状态形成的闭包
    vector<string>middle;
    vector<int>ends;
    int rels[200][200];//闭包有自己的代号
    DFA(){
        //初始化
        for(int i=0;i<200;i++){
            for(int j=0;j<200;j++){
                rels[i][j]=-1;
            }
        }
    }
};
DFA dfa;
struct Token{
    int line;//行号
    string name;//符号
    int type;//种类:0-界符 1-关键词 2-运算符 3-常量 4-标识符 
};
//定义全局变量
const int maxn=100;
char buffer[maxn];
string number[]={"0","1","2","3","4","5","6","7","8","9"};
string alpha[]={"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z",
"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
string single_operator[]={"+","-","*","/","%","|","&","^","!","=","?","<",">"};
string double_operator[]={"<=",">=","+=","-=","*=","/=","%=","|=","&=","^=","!","==","++","--","||","&&","<<",">>"};
char limited[]={';',',','#','[',']','{','}',':','\'','\"','(',')'};
string kind[]={"limited","keyword","operator","const","sign"};
set<string>middles;//存放从一个状态到另一个状态的中间字符
vector<string>keywords;
//NFA的数量根据START标志判别
NFA nfa;
vector<Token>tokens;
string getRight(string str){
    int posl=str.find(' ');
    int len=str.length();
    str=str.substr(posl+1,len);
    return str.substr(str.find(' ')+1,len);
}
vector<string> splitG(string line){
    //拆分rules中的规则
    vector<string>temp;
    bool flag=false;
    string temp_str="";
    for(int i=0;i<line.length();i++){
        if(line[i]==']'){
            flag=false;
            temp.push_back(temp_str);
            temp_str="";
        }else if(line[i]=='['){
            flag=true;
        }else if(flag){
            temp_str=temp_str+line[i];
        }
    }
    return temp;
}
string getFromSign1(string str){
    //从'_'中获取字符
    string ans="";
    for(int i=1;i<str.length()-1;i++){
        ans+=str[i];
    }
    return ans;
}
void _init_(){
    /*初始化得到NFA与关键词*/
    //读取关键词
    ifstream input;
    input.open(KFILE);
    if(!input.is_open()){
        printf("FAILED TO LOAD KEYWORDS！");
    }else{
        while(input.getline(buffer,maxn-1)){
            string keyword=getRight(buffer);
            
            keywords.push_back(keyword);
        }
    }
    input.close();
    //大写的字母表示状态，''内的表示字符,小写的指代一类字符
    input.open(GFILE);
    if(!input.is_open()){
        printf("FAILED TO LOAD GRAMMAR!");
    }else{
        while(input.getline(buffer,maxn-1)){
            vector<string>temp=splitG(buffer);
            if(temp.size()==2){
                //TODO:这里默认F->final
                nfa.ends.push_back(temp[0]);
                if(temp[1]!="final"){
                    temp[1]=getFromSign1(temp[1]);
                    middles.insert(temp[1]);
                
                }
                
            }else{
                //其余的情况是F->fA
                //这里分特殊字符，或者统称
                if(temp[1]!="letter"&&temp[1]!="number"){
                    //这里需要对‘-’做特殊处理
                    temp[1]=getFromSign1(temp[1]);
                }
                middles.insert(temp[1]);
                Twostate two;
                two.a=temp[0];two.b=temp[2];
                nfa.relations[temp[1]].push_back(two);
            }
        }
    }
}
void showNFA(NFA nfa){
    //展示NFA构建是否正确
    cout<<"Start Point:"<<nfa.start<<endl;
    cout<<"Show Relations:*******************************"<<endl;
    for(map<string,vector<Twostate> >::iterator it=nfa.relations.begin();it!=nfa.relations.end();it++){
        vector<Twostate>temp=it->second;
        cout<<"middle: "<<it->first<<endl;
        for(int i=0;i<temp.size();i++){
            cout<<temp[i].a<<"--"<<temp[i].b<<endl;
        }
    }
    cout<<"Show End Points:*******************************"<<endl;
    for(int i=0;i<nfa.ends.size();i++){
        cout<<nfa.ends[i];
    }
}
/*下面的函数全部为NFA-DFA服务**********************************************************************************************************************/
set<string> moving(set<string>closure,string sign){
    /*得到当前closure移动一个sign可以抵达的下一个状态*/
    set<string>new_closure;
    vector<Twostate>temp=nfa.relations[sign];
    for(int i=0;i<temp.size();i++){
        if(closure.count(temp[i].a)){
            new_closure.insert(temp[i].b);
        }
    }
    return new_closure;
}
bool isEnd(set<string>closure){
    for(set<string>::iterator it=closure.begin();it!=closure.end();it++){
        if(count(nfa.ends.begin(),nfa.ends.end(),*it)){
            return true;
        }
    }
    return false;
}
void toDFA(){
    /*将NFA转化成DFA*/
    set<set<string> >worked_closure;//已经标记过的closure
    stack<set<string> >Closure;
    set<string>clo;//当前的clo
    clo.insert(nfa.start);
    Closure.push(clo);
    //中间符号
    for(set<string>::iterator it=middles.begin();it!=middles.end();it++){
        dfa.middle.push_back(*it);
    }
    //将第一个加入dfa中
    dfa.start=clo;
    dfa.closures.push_back(clo);//编号为0
    int id,tid;
    //在处理的过程中需要更新dfa
    while(!Closure.empty()){
        clo=Closure.top();
        Closure.pop();
        if(worked_closure.count(clo)!=0){
            //说明已经做过了
            continue;
        }else{
            worked_closure.insert(clo);
        }
        id=find(dfa.closures.begin(),dfa.closures.end(),clo)-dfa.closures.begin();//返回查询的下标
        if(isEnd(clo)){
            dfa.ends.push_back(id);
        }
        for(int i=0;i<dfa.middle.size();i++){
            string sign=dfa.middle[i];
            set<string>new_closure=moving(clo,sign);
            if(new_closure.size()==0){
                continue;
            }
            if(worked_closure.count(new_closure)==0){//之前没有出现过这种闭包，则加入到待检测闭包中
                Closure.push(new_closure);
                if(count(dfa.closures.begin(),dfa.closures.end(),new_closure)==0)//原来也没有
                    dfa.closures.push_back(new_closure);
            }
            tid=find(dfa.closures.begin(),dfa.closures.end(),new_closure)-dfa.closures.begin();
            //定义闭包之间的关系
            dfa.rels[id][i]=tid;
            //debug
            // cout<<"{";
            // for(set<string>::iterator it=clo.begin();it!=clo.end();it++){
            //     cout<<*it<<" ";
            // }
            // cout<<"}   "<<sign;
            // cout<<"{";
            // for(set<string>::iterator it=new_closure.begin();it!=new_closure.end();it++){
            //     cout<<*it<<" ";
            // }
            // cout<<"}   "<<endl;
        }
        
    }
    //TODO:最小化DFA（暂时不影响，属于后期优化的问题） 
}
//DEBUG:查看是否符合要求
void showClosure(int index){
    printf("{");
    for(set<string>::iterator it=dfa.closures[index].begin();it!=dfa.closures[index].end();it++){
        cout<<*it<<" ";
    }
    cout<<"}";
}
void showDFA(){
    /*
    struct DFA{
    set<string>start;
    vector<set<string> >closures;//状态形成的闭包
    vector<int>ends;
    string rels[200][200];//闭包有自己的代号
    };
    */
   int size=dfa.closures.size();
   cout<<"Start Points:\n";
   for(set<string>::iterator it=dfa.start.begin();it!=dfa.start.end();it++){
       cout<<*it<<" ";
   }
   cout<<"\nClosures:\n";
   for(int i=0;i<size;i++){
       cout<<"id="<<i<<" :";
       for(set<string>::iterator it=dfa.closures[i].begin();it!=dfa.closures[i].end();it++){
            cout<<*it<<" ";
        }
        cout<<"\n";
   }
    cout<<"Relations:\n";
    for(int i=0;i<size;i++){
        for(int j=0;j<dfa.middle.size();j++){
            if(dfa.rels[i][j]!=-1){
                showClosure(i);
                cout<<"--"<<dfa.middle[j]<<":";
                showClosure(dfa.rels[i][j]);
                cout<<endl;
            }
        }
    }
    cout<<"Termination:\n";
    for(int i=0;i<dfa.ends.size();i++){
        cout<<dfa.ends[i]<<" ";
    }
    cout<<"\nmiddle:\n";
    for(vector<string>::iterator it=dfa.middle.begin();it!=dfa.middle.end();it++){
        cout<<*it<<" ";
    }
    cout<<"\n";
}
bool isEndState(int index){
    if(count(dfa.ends.begin(),dfa.ends.end(),index)!=0){
        return true;
    }
    return false;
}
int isOktoStart(int index){
    if(index>=dfa.middle.size()){
        return -1;
    }else{
        return dfa.rels[0][index];
    }
    return -1;
}
bool isOp(string str){
    int len=str.length();
    if(len>=3)return false;
    if(len==1){
        for(int i=0;i<sizeof(single_operator)/sizeof(single_operator[0]);i++){
        if(str==single_operator[i]){
            return true;
        }
        }
    }else{
        for(int i=0;i<sizeof(double_operator)/sizeof(double_operator[0]);i++){
        if(str==double_operator[i]){
            return true;
        }
        }
    }
    return false;
}
bool isLimit(char ch){
    for(int i=0;i<strlen(limited);i++){
        if(ch==limited[i]){
            return true;
        }
    }
    return false;
}
Token newToken(string str,int line,bool isok){
    Token token;
    token.line=line;
    token.name=str;
    //如果判断为非法的字符，不需要进行下面的步骤
    if(isok==false){
        //识别失败
        token.type=-1;
        return token;
    }
    //成功的字符，需要简单归类
    if(str.length()==1&&isLimit(str[0])){
        token.type=0;//界符
    }else if(count(keywords.begin(),keywords.end(),str)){
        token.type=1;//关键字
    }else if(isOp(str)){
        token.type=2;//运算符
    }else if(isdigit(str[0])||str[0]=='+'||str[0]=='-'){
        token.type=3;//常量：数字（+-小数、整数）、虚数
    }else if(isalpha(str[0])||str[0]=='_'){
        token.type=4;//标识符
    }
    return token;
}
void DFA_Process_str(string str,int line){
    Token temp_token;
    temp_token.line=line;
    //TODO:处理的是空格和界符切分好的字符串
    int len=str.length();
    string temp="";
    int pre_state=-1;//之前的状态
    int cur_state=0;//现在的状态
    int mid;
    int smid;
    vector<string>::iterator it;
    vector<string>::iterator special_it;
    bool flag=true;//正确
    for(int i=0;i<len;i++){
        
        //首先找到当前符号的mid
        if(isalpha(str[i])){
            //是字母:这边存在的问题是——‘e’‘E’‘i’同样是合法的中间字符
            string s="";
            s+=str[i];
            it=find(dfa.middle.begin(),dfa.middle.end(),"letter");
            special_it=find(dfa.middle.begin(),dfa.middle.end(),s);
        }else if(isdigit(str[i])){
            it=find(dfa.middle.begin(),dfa.middle.end(),"number");
        }else{
            string s="";s+=str[i];
            it=find(dfa.middle.begin(),dfa.middle.end(),s);
        }
        bool notFind=false;
        if(it==dfa.middle.end()&&special_it==dfa.middle.end()){
            mid=-1;
            smid=-1;
            notFind=true;
        }else{
            mid=it-dfa.middle.begin();
            smid=special_it-dfa.middle.begin();
            //没找到
            if(mid>=dfa.middle.size()){
                mid=-1;
            }
            if(smid>=dfa.middle.size()){
                smid=-1;
            }
        }
        cout<<temp<<" "<<cur_state<<" "<<str[i]<<endl;
        if(mid==-1||dfa.rels[cur_state][mid]==-1||notFind){
            //接收当前字符导致下一个状态不合理
            cout<<"mo\n";
            if(/*isalpha(str[i])&&*/smid==-1||dfa.rels[cur_state][smid]==-1||notFind/*||!isalpha(str[i])||notFind*/){
                
                string s="";
                s+=str[i];
                if(isEndState(cur_state)&&(isOp(s)||isOp(temp))){//这样截断有一个问题89_认为是对的
                    if(temp!=""){
                        /*合理状态--加入*/
                        cout<<"1add:"<<temp<<endl;
                        temp_token=newToken(temp,line,true);
                        tokens.push_back(temp_token);
                        temp="";
                    }
                }else{
                    cout<<"2add:"<<temp<<endl;
                    //需要保留错误的符号
                    temp+=str[i];
                    flag=false;
                    continue;
                }
                
                int state=isOktoStart(mid);
                cout<<"MID: "<<str[i]<<" "<<mid<<" "<<smid<<endl;
                if(state!=-1){
                    //cout<<"normal:"<<state<<" "<<mid<<endl;
                    //如果可以正常跳转,但是之前没有达到final
                    if(!isEndState(cur_state)){
                        if(temp!=""){
                            temp_token=newToken(temp,line,isOp(temp));
                            tokens.push_back(temp_token);
                            temp="";
                        }

                    }
                    
                    temp+=str[i];
                    pre_state=cur_state;
                    cur_state=state;
                }else{
                    //如果不可以正常跳转
                    cout<<"!"<<str[i]<<endl;
                    temp+=str[i];
                    if(temp!=""){
                        cout<<cur_state<<":"<<temp<<endl;
                        temp_token=newToken(temp,line,isOp(temp));
                        tokens.push_back(temp_token);
                        //cout<<temp<<endl;
                        temp="";
                    }
                    pre_state=cur_state;
                    cur_state=0;
                    printf("next is unsuccess!\n");
                }
                
            }else if(dfa.rels[cur_state][smid]!=-1){
                //实际上的另一种正常跳转
                if(cur_state==0&&temp!=""){
                    temp_token=newToken(temp,line,true);
                    tokens.push_back(temp_token);
                    //cout<<temp<<endl;
                    temp="";
                }
                
                temp+=str[i];
                pre_state=cur_state;
                cur_state=dfa.rels[cur_state][smid];
            }
        }else{
            //正常跳转到下一个状态
            // if(cur_state==0&&temp!=""){
            //     temp_token=newToken(temp,line,true);
            //     tokens.push_back(temp_token);
            //     //cout<<temp<<endl;
            //     temp="";
            // }
            //cout<<"normal:"<<temp<<endl;
            temp+=str[i];
            pre_state=cur_state;
            cur_state=dfa.rels[cur_state][mid];
        }

    }
    //最后收尾
    if(temp!=""){
        cout<<temp<<" "<<cur_state<<endl;
        if(isEndState(cur_state)&&flag){
            temp_token=newToken(temp,line,true);
        }else{
            cout<<cur_state<<":"<<temp<<endl;
            temp_token=newToken(temp,line,false);
        }
        //cout<<temp<<endl;
        tokens.push_back(temp_token);
    }
    
}

vector<string> split_limit(string str,int line){
    //按照界符分割字符串
    vector<string>ans;
    string temp="";
    for(int i=0;i<str.length();i++){
        if(!isLimit(str[i])){
            temp+=str[i];
        }else{
            //如果是界符
            if(temp!=""){
                ans.push_back(temp);
                temp="";
            }
            //FIXME:一个bug,ans.push_back(to_string(str[i]));这种写法转回的是ascii
            string s="";
            s+=str[i];
            ans.push_back(s);
        }
    }
    if(temp!=""){
        ans.push_back(temp);
    }
    return ans;
}
//读取代码txt,利用字符串以及DFA处理相应每一行
vector<string> Splitby_tab_limit(string str,int line){
    vector<string>ans;
    //按照空格切分
    stringstream input(buffer);
    string word;
    while(input>>word){
        vector<string>temp=split_limit(word,line);
        ans.insert(ans.end(),temp.begin(),temp.end());
    }
    return ans;
}
void words2tokens(vector<string> words,int line){
    Token token;
    token.line=line;
    for(int i=0;i<words.size();i++){
        //如果是界符
        if(words[i].length()==1&&isLimit(words[i][0])){
            token.name="";
            token.name+=words[i];
            token.type=0;
            tokens.push_back(token);
        }else{
            DFA_Process_str(words[i],line);
            
        }
    }
}

void Lex(string codefile){
    //处理
    ifstream input;
    input.open(codefile);
    if(!input.is_open()){
        printf("FAILED TO LOAD CODEFILES！");
    }else{
        //按照分割符号读入
        int line=0;
        while(input.getline(buffer,maxn-1)){
            //按照空格、界符将字符串划分
            line++;
            vector<string>words=Splitby_tab_limit(buffer,line);
            
            words2tokens(words,line);
        } 
    }
}
/*时间复杂度的计算************************************************************************************/
int gettime()
{
    time_t rawtime;
    struct tm *ptminfo;
    time(&rawtime);
    ptminfo = localtime(&rawtime);
    printf("current: %02d-%02d-%02d %02d:%02d:%02d\n",
    ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
    ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
        
    return ptminfo->tm_sec;
}
void showTokens(){
    printf("\n\n------------------------------\n\n");
    printf("tokens:%d\n",tokens.size());
    printf("\n\n------------------------------\n\n");
    //printf("TOKENS\t\t\t\tLINE\t\t\t\tTYPE\n");
    cout<<"\033[34mTOKENS\033[0m"<<"\t\t\t\t"<<"\033[33mLINE\033[0m"<<"\t\t\t\t"<<"\033[32mTYPE\033[0m"<<endl;
    for(int i=0;i<tokens.size();i++){
        //cout<<tokens[i].name<<"\t\t\t\t"<<tokens[i].line<<"\t\t\t\t";
        cout<<"\033[34m"<<tokens[i].name<<"\033[0m"<<"\t\t\t\t"<<"\033[33m"<<tokens[i].line<<"\033[0m"<<"\t\t\t\t";
        if(tokens[i].type>=0){
            cout<<"\033[32m"<<kind[tokens[i].type]<<"\033[0m"<<endl;
        }else{
            cout<<"\033[31m"<<"FALSE!"<<"\033[0m"<<endl;
        }
    }
}
void Writer(string filename){
    //将tokens写入txt中
    ofstream fout(filename);
    int len=tokens.size();
    for(int i=0;i<len;i++){
        fout<<tokens[i].line<<"\t"<<tokens[i].name<<"\t"<<kind[tokens[i].type];
        if(i!=len-1){
            fout<<"\n";
        }
    }
    fout.close();
}
int main(){
    int start=gettime();
    _init_();
    showNFA(nfa);
    toDFA();
    showDFA();
    Lex(codefile);
    showTokens();
    int ends=gettime();
    printf("Total Time Cost:");
    printf("%d",ends-start);

    Writer("./data/token_table_equal.txt");

    //测试DFA判断写得是否正确
    /*
    printf("\ntest:\n");
    string str="x=1.90e9";
    vector<string>ans=DFA_Process_str(str,1);
    for(int i=0;i<ans.size();i++){
        cout<<ans[i]<<endl;
    }
    */
    return 0;
}